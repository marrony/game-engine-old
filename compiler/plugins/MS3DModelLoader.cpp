/*
 * MS3DModelLoader.cpp
 *
 *  Created on: 18/09/2011
 *      Author: marrony
 */
#include "ResourcePlugin.h"
#include "ResourceCompiler.h"

#include "Material.h"
#include "MS3DModel.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Model.h"
#include "FileUtil.h"

#include <vector>
#include <fstream>

#include "MemoryManager.h"

using namespace engine;
using namespace compiler;

class MS3DModelLoader : public ResourceLoader {
	ResourceCompiler* compiler;
	ResourceManager* manager;
public:
	MS3DModelLoader() {
	}

	virtual ~MS3DModelLoader() {
	}

	virtual void release() {
		delete this;
	}

	virtual void initialize(ResourceCompiler* compiler, ResourceManager* manager) {
		this->compiler = compiler;
		this->manager = manager;

		compiler->registerLoader(this, "Milkshape Loader", "ms3d");
	}

	virtual void compileResource(const char* fileName, std::map<std::string, std::string>& options) {
		std::ifstream file(fileName, std::ios::in | std::ios::binary);

		if(file.eof())
			return;

		MS3DModel ms3d;

		ms3d.Load(file);

		Model* model = new Model(file::getFilename(fileName), manager);

		for(int i = 0; i < ms3d.GetNumMaterials(); i++) {
			ms3d_material_t* material = ms3d.GetMaterial(i);

			//compiler->compile(material->name);
		}

		for(size_t i = 0; i < ms3d.GetNumGroups(); i++) {
			ms3d_group_t* group = ms3d.GetGroup(i);

			std::map<unsigned short, unsigned short> fromTo;
			std::vector<unsigned short> indices;
			std::vector<MeshVertex> vertices;

			for(size_t j = 0; j < group->triangleIndices.size(); j++) {
				ms3d_triangle_t* triangle = ms3d.GetTriangle(group->triangleIndices[j]);

				for(size_t k = 0; k < 3; k++) {
					MeshVertex vertex;

					ms3d_vertex_t* v = ms3d.GetVertex(triangle->vertexIndices[k]);

					unsigned short vertexIndex;
					if(fromTo.find(triangle->vertexIndices[k]) == fromTo.end()) {
						vertexIndex = vertices.size();
						fromTo.insert(std::make_pair(triangle->vertexIndices[k], vertexIndex));

						vertex.position.x = v->vertex[0];
						vertex.position.y = v->vertex[1];
						vertex.position.z = v->vertex[2];

						vertex.normal.x = triangle->vertexNormals[k][0];
						vertex.normal.y = triangle->vertexNormals[k][1];
						vertex.normal.z = triangle->vertexNormals[k][2];

						vertex.texCoord.x = triangle->s[k];
						vertex.texCoord.y = triangle->t[k];

						if(v->weights[0] != 0 && v->weights[1] != 0 && v->weights[1] != 0) {
							unsigned int w = v->weights[0] + v->weights[1] + v->weights[2];

							vertex.weights.x = v->weights[0] / 100.0;
							vertex.weights.y = v->weights[1] / 100.0;
							vertex.weights.z = v->weights[2] / 100.0;
							vertex.weights.w = (100 - w) / 100.0;
						} else {
							vertex.weights.x = 1.0;
							vertex.weights.y = 0.0;
							vertex.weights.z = 0.0;
							vertex.weights.w = 0.0;
						}

						vertex.boneIds.x = v->boneId;
						vertex.boneIds.y = v->boneIds[0];
						vertex.boneIds.z = v->boneIds[2];
						vertex.boneIds.w = v->boneIds[3];

						vertices.push_back(vertex);
					} else {
						vertexIndex = fromTo[triangle->vertexIndices[k]];
					}

					indices.push_back(vertexIndex);
				}
			}

			int flags = MeshVertex::POSITION | MeshVertex::NORMAL | MeshVertex::TEXTURE | MeshVertex::BONES;
			ms3d_material_t* ms3dMaterial = ms3d.GetMaterial(group->materialIndex);

			//TODO carregar
			Material* material = (Material*)manager->loadResource(MaterialKey(ms3dMaterial->name));
			model->addVertexData(vertices, indices, material, flags);
		}

		model->modelData->calculateTangent();
		model->modelData->calculateBoundingBox();

		model->getAnimation().setFrameProperties(ms3d.GetAnimationFps(), ms3d.GetTotalFrames());

		if(ms3d.GetNumJoints() > 0) {
			model->getAnimation().setBoneCount(ms3d.GetNumJoints());

			for(size_t i = 0; i < ms3d.GetNumJoints(); i++) {
				ms3d_joint_t* joint = ms3d.GetJoint(i);

				Bone* bone = model->getAnimation().getBone(i);

				bone->parentIndex = ms3d.FindJointByName(joint->parentName);
				bone->name = joint->name;

				math::EulerAngle angle(joint->rot);
				math::Vector3 position(joint->pos);

				bone->localSkeleton = math::Matrix4::transformationMatrix(angle, position);
				bone->globalSkeleton = math::Matrix4::IDENTITY;

				bone->positions.resize(joint->positionKeys.size());
				for(size_t j = 0; j < joint->positionKeys.size(); j++) {
					bone->positions[j].time = joint->positionKeys[j].time;
					bone->positions[j].position = math::Vector3(joint->positionKeys[j].key);
				}

				bone->rotations.resize(joint->rotationKeys.size());
				for(size_t j = 0; j < joint->rotationKeys.size(); j++) {
					const float x = joint->rotationKeys[j].key[0];
					const float y = joint->rotationKeys[j].key[1];
					const float z = joint->rotationKeys[j].key[2];
					const float s = joint->rotationKeys[j].key[3];

					bone->rotations[j].time = joint->rotationKeys[j].time;
					bone->rotations[j].rotation = math::Quaternion(s, x, y, z);
				}
			}

			model->getAnimation().updateBones();
		}

		std::string outputName = file::getPath(fileName) + "/" + file::getFilename(fileName) + ".model";
		FileStream fileStream(outputName);
		ResourceBinStream resourceStream(fileStream);
		ModelUtils::write(resourceStream, *manager, model);

		delete model;
	}
};

static Module* dynamicLib;

extern "C" Module* getModule() {
	return dynamicLib;
}

extern "C" EXPIMP Plugin* initializePlugin(Module* dynLib) {
	dynamicLib = dynLib;
	return new MS3DModelLoader;
}
