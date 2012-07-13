/*
 * ObjSceneLoader.cpp
 *
 *  Created on: 03/02/2011
 *      Author: marrony
 */

#include "loader/ObjSceneLoader.h"
#include "Mesh.h"
#include "Exception.h"
#include "GameEntity.h"
#include "Geometry.h"
#include "TransformationModifier.h"
#include "Model.h"
#include "SceneManager.h"
#include "ResourceManager.h"

#include <sstream>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <fstream>
#include <cctype>

#include "GeneralHashFunction.h"

namespace engine {

	struct Vertex {
		int positionIndex;
		int normalIndex;
		int textureIndex;
		int smoothGroup;

		bool operator<(const Vertex& other) const {
			if(positionIndex < other.positionIndex) {
				return true;
			} else if(positionIndex == other.positionIndex) {
				if(normalIndex < other.normalIndex) {
					return true;
				} else if(normalIndex == other.normalIndex) {
					if(textureIndex < other.textureIndex) {
						return true;
					} else if(textureIndex == other.textureIndex) {
						return smoothGroup < other.smoothGroup;
					}
				}
			}

			return false;
		}

		void adjustIndex(size_t vertexCount, size_t textureCount, size_t normalCount) {
			if(positionIndex < 0) {
				positionIndex += vertexCount;
			} else {
				positionIndex--;
			}

			if(normalIndex < 0) {
				normalIndex += normalCount;
			} else {
				normalIndex--;
			}

			if(textureIndex < 0) {
				textureIndex += textureCount;
			} else {
				textureIndex--;
			}
		}
	};

	struct Group {
		typedef std::map<Vertex, size_t> HashTable;

		std::string name;
		std::string material;
		HashTable hashTable;
		std::vector<MeshVertex> vertices;
		std::vector<unsigned short> index;

		bool hasPosition;
		bool hasNormal;
		bool hasTexture;

		Group() :
				hasPosition(false), hasNormal(false), hasTexture(false) {
		}

		void addVertex(MeshVertex& vertex, Vertex& temp) {
			unsigned int newIndex;

			HashTable::iterator ite;

			if(temp.smoothGroup == 0) {
				ite = hashTable.end();
			} else {
				ite = hashTable.find(temp);
			}

			if(ite == hashTable.end()) {
				newIndex = vertices.size();

				vertices.push_back(vertex);

				hashTable.insert(std::make_pair(temp, newIndex));
			} else {
				newIndex = ite->second;
			}

			hasPosition = temp.positionIndex >= 0;
			hasNormal = temp.normalIndex >= 0;
			hasTexture = temp.textureIndex >= 0;

			index.push_back(newIndex);
		}
	};

	enum TokenType {
		INVALID, ID, NUMBER, COMMENT, SLASH, END_OF_FILE,
	};

	struct Token {
		TokenType type;
		std::string lexeme;

		Token() :
			type(INVALID) {
		}

		int getInt() const {
			int value = 0;
			sscanf(lexeme.c_str(), "%d", &value);
			return value;
		}

		float getFloat() const {
			float value = 0;
			sscanf(lexeme.c_str(), "%f", &value);
			return value;
		}

		const std::string& getString() const {
			return lexeme;
		}
	};

	class ObjMtlLexer {
		void parseIdentifier(Token& token) {
			std::stringstream stream;

			stream << (char) arquivo.get();

			while(!arquivo.eof()) {
				int ch = arquivo.get();

				if(ch != ' ' && ch != '\r' && ch != '\n' /*std::isalnum(ch) || ch == '.' || ch == '_'*/) {
					stream << (char) ch;
				} else {
					arquivo.unget();
					break;
				}
			}

			token.type = ID;
			token.lexeme = stream.str();
		}

		void parseNumber(Token& token) {
			std::stringstream stream;

			stream << (char) arquivo.get();

			while(!arquivo.eof()) {
				int ch = arquivo.get();

				if(std::isdigit(ch) || ch == '.') {
					stream << (char) ch;
				} else {
					arquivo.unget();
					break;
				}
			}

			token.type = NUMBER;
			token.lexeme = stream.str();
		}

		void parseComment(Token& token) {
			std::stringstream stream;

			while(!arquivo.eof()) {
				int ch = arquivo.get();

				if(ch != 0x0d && ch != 0x0a) {
					stream << (char) ch;
				} else {
					arquivo.unget();
					break;
				}
			}

			token.type = COMMENT;
			token.lexeme = stream.str();
		}

		void parseEof(Token& token) {
			token.type = END_OF_FILE;
			token.lexeme = "<eof>";
		}

		std::istream& arquivo;
		Token currentToken;

	public:
		ObjMtlLexer(std::istream& newStream) :
			arquivo(newStream) {
		}

		math::Vector2 parseVertex2() {
			math::Vector2 vertex;

			Token token;

			if(nextToken(token)) {
				vertex.x = token.getFloat();
			}

			if(nextToken(token)) {
				vertex.y = token.getFloat();
			}

			return vertex;
		}

		math::Vector3 parseVertex3() {
			math::Vector3 vertex(0, 0, 0);

			Token token;

			if(nextToken(token)) {
				vertex.x = token.getFloat();
			}

			if(nextToken(token)) {
				vertex.y = token.getFloat();
			}

			if(nextToken(token)) {
				vertex.z = token.getFloat();
			}

			return vertex;
		}

		void ungetToken(const Token& token) {
			currentToken = token;
		}

		bool nextToken(Token& token) {
			if(currentToken.type != INVALID) {
				token = currentToken;
				currentToken.type = INVALID;
				return true;
			}

			if(arquivo.eof()) {
				parseEof(token);
				return false;
			}

			while(!arquivo.eof()) {
				int ch = arquivo.get();

				if(ch == -1) {
					parseEof(token);
					return false;
				} else if(std::isalpha(ch) || ch == '_') {
					arquivo.unget();
					parseIdentifier(token);
					break;
				} else if(std::isdigit(ch) || ch == '-' || ch == '+') {
					arquivo.unget();
					parseNumber(token);
					break;
				} else if(ch == '#') {
					parseComment(token);
					break;
				} else if(ch == '/') {
					token.type = SLASH;
					token.lexeme = "/";
					break;
				}
			}

			return true;
		}

		bool parseFace(Vertex& vertex) {
			vertex.positionIndex = 0;
			vertex.textureIndex = 0;
			vertex.normalIndex = 0;

			Token token;

			nextToken(token);
			if(token.type == NUMBER) {
				vertex.positionIndex = token.getInt(); //  v

				nextToken(token);
				if(token.type == SLASH) {
					nextToken(token);

					if(token.type == SLASH) {
						nextToken(token);
						vertex.normalIndex = token.getInt(); //  v//n
					} else {
						vertex.textureIndex = token.getInt(); //  v/t

						nextToken(token);
						if(token.type == SLASH) {
							nextToken(token);

							vertex.normalIndex = token.getInt(); // v/t/n
						} else {
							ungetToken(token);
						}
					}
				} else {
					ungetToken(token);
				}
			} else {
				ungetToken(token);
				return false;
			}

			return true;
		}

	};

	void parseMaterial(ObjMtlLexer& parser, SceneManager* sceneManager) {
		Token token;

		std::string textureSpecular, textureDiffuse, textureAmbient, textureNormal;
		math::Vector3 colorSpecular, colorDiffuse, colorAmbient;
		std::string materialName;
		float shineness;

		parser.nextToken(token);

		materialName = token.lexeme;

		while(parser.nextToken(token)) {
			if(token.type == COMMENT) {
				continue;
			}

			if(token.type == ID && token.lexeme == "newmtl") {
				parser.ungetToken(token);
				break;
			}

			if(token.type == ID && token.lexeme == "map_Ka") {
				parser.nextToken(token);
				textureAmbient = token.lexeme;
				continue;
			}

			if(token.type == ID && token.lexeme == "map_Kd") {
				parser.nextToken(token);
				textureDiffuse = token.lexeme;
				continue;
			}

			if(token.type == ID && token.lexeme == "map_Ks") {
				parser.nextToken(token);
				textureSpecular = token.lexeme;
				continue;
			}

			if(token.type == ID && (token.lexeme == "bump" || token.lexeme == "map_bump")) {
				parser.nextToken(token);
				textureNormal = token.lexeme;
				continue;
			}

			if(token.type == ID && token.lexeme == "Ks") {
				colorSpecular = parser.parseVertex3();
				continue;
			}

			if(token.type == ID && token.lexeme == "Kd") {
				colorDiffuse = parser.parseVertex3();
				continue;
			}

			if(token.type == ID && token.lexeme == "Ka") {
				colorAmbient = parser.parseVertex3();
				continue;
			}

			if(token.type == ID && token.lexeme == "Ns") {
				parser.nextToken(token);
				shineness = token.getFloat();
				continue;
			}
		}

		if(!textureDiffuse.empty() && !textureNormal.empty()) {
			//sceneManager->createMaterial(materialName, textureDiffuse, textureNormal);
			Material* material = sceneManager->getMaterial(materialName);
		}
	}

	void parseMaterialFile(const std::string& filename, SceneManager* sceneManager) {
		std::ifstream materialFile(("scene\\" + filename).c_str());

		if(!materialFile.is_open()) {
			throw Exception("Material not found");
		}

		ObjMtlLexer lexer(materialFile);

		Token token;

		while(lexer.nextToken(token)) {
			if(token.type == COMMENT) {
				continue;
			}

			if(token.type == ID && token.lexeme == "newmtl") {
				//TODO nao criar o material no SceneManager
				parseMaterial(lexer, sceneManager);
			}
		}
	}

	ObjModelLoader::ObjModelLoader(SceneManager* sceneManager, ResourceManager* resourceManager, GraphicManager& graphicManager) :
			sceneManager(sceneManager), resourceManager(resourceManager), graphicManager(graphicManager) {
	}

	Model* ObjModelLoader::loadModel(std::istream& sceneStream) {
		std::vector<math::Vector2> allTextures;
		std::vector<math::Vector3> allNormals;
		std::vector<math::Vector3> allPositions;
		std::vector<Group> groups;
		Token token;

		ObjMtlLexer lexer(sceneStream);

		int smoothGroup = 1;

		groups.push_back(Group());

		while(lexer.nextToken(token)) {
			if(token.type == COMMENT) {
				continue;
			}

			if(token.type == ID) {
				if(token.lexeme == "v") {
					allPositions.push_back(lexer.parseVertex3());
					continue;
				}

				if(token.lexeme == "vn") {
					allNormals.push_back(lexer.parseVertex3());
					continue;
				}

				if(token.lexeme == "vt") {
					allTextures.push_back(lexer.parseVertex2());
					continue;
				}

				if(token.lexeme == "f") {
					Vertex v[3];

					for(int i = 0; i < 3; i++) {
						v[i].smoothGroup = smoothGroup;

						lexer.parseFace(v[i]);

						v[i].adjustIndex(allPositions.size(), allTextures.size(), allNormals.size());

						MeshVertex vertex;

						vertex.position = allPositions[v[i].positionIndex];

						if(v[i].normalIndex >= 0) {
							vertex.normal = allNormals[v[i].normalIndex];
						}

						if(v[i].textureIndex >= 0) {
							vertex.texCoord = allTextures[v[i].textureIndex];
						}

						groups.back().addVertex(vertex, v[i]);
					}

					while(true) {
						v[1] = v[2];

						if(!lexer.parseFace(v[2])) {
							break;
						}

						v[2].adjustIndex(allPositions.size(), allTextures.size(), allNormals.size());

						for(int i = 0; i < 3; i++) {
							MeshVertex vertex;

							vertex.position = allPositions[v[i].positionIndex];

							if(v[i].normalIndex >= 0) {
								vertex.normal = allNormals[v[i].normalIndex];
							}

							if(v[i].textureIndex >= 0) {
								vertex.texCoord = allTextures[v[i].textureIndex];
							}

							groups.back().addVertex(vertex, v[i]);
						}
					}

					continue;
				}

				if(token.lexeme == "g") {
					lexer.nextToken(token);

					if(!groups.back().vertices.empty()) {
						groups.push_back(Group());
					}

					groups.back().name = token.lexeme;
					continue;
				}

				if(token.lexeme == "s") {
					lexer.nextToken(token);
					smoothGroup = token.getInt();
					continue;
				}

				if(token.lexeme == "usemtl") {
					lexer.nextToken(token);

					if(groups.back().vertices.empty()) {
						groups.back().material = token.lexeme;
					} else {
						Group group;
						group.name = groups.back().name;
						group.material = token.lexeme;

						groups.push_back(group);
					}

					continue;
				}

				if(token.lexeme == "mtllib") {
					lexer.nextToken(token);

					parseMaterialFile(token.lexeme, sceneManager);
					continue;
				}
			}
		}

		Model* model = new Model("");

		for(size_t i = 0; i < groups.size(); i++) {
			Group& group = groups[i];

			int flags = Mesh::POSITION;

			if(group.hasNormal) {
				flags |= Mesh::NORMAL;
			}

			if(group.hasTexture) {
				flags |= Mesh::TEXTURE;
			}

			model->addVertexData(group.vertices, group.index, group.material, flags);
		}

		model->calculateTangent();
		model->compileVertices();
		model->createBuffers();

		return model;
	}

} // namespace engine
