/*
 * Model.cpp
 *
 *  Created on: 13/07/2011
 *      Author: marrony
 */

#include "Model.h"
#include "Exception.h"
#include "ResourceManager.h"
#include "GraphicManager.h"
#include "Stream.h"

#include "MemoryManager.h"

namespace engine {
	const Type Model::TYPE("model");

	void Model::addVertexData(const std::vector<MeshVertex>& vertexArray, const std::vector<unsigned short>& newIndices, Material* material, int flags) {
		if(!modelData)
			modelData = new ModelData;

		Mesh* indexMesh = new Mesh;

		size_t lastVertexCount = modelData->position.size();

		indexMesh->offset = modelData->indices.size();
		indexMesh->count = newIndices.size();
		indexMesh->start = lastVertexCount + *std::min_element(newIndices.begin(), newIndices.end());
		indexMesh->end = lastVertexCount + *std::max_element(newIndices.begin(), newIndices.end());
		indexMesh->material = material;

		meshes.push_back(indexMesh);

		for(size_t i = 0; i < newIndices.size(); i++) {
			modelData->indices.push_back(lastVertexCount + newIndices[i]);
		}

		size_t vertexCount = lastVertexCount + vertexArray.size();

		modelData->position.resize(vertexCount);
		for(size_t i = 0; i < newIndices.size(); i++) {
			size_t index = newIndices[i];

			modelData->position[lastVertexCount + index] = vertexArray[index].position;
		}

		if(flags & MeshVertex::NORMAL) {
			modelData->normal.resize(vertexCount);
			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				modelData->normal[lastVertexCount + index] = vertexArray[index].normal;
			}
		}

		if(flags & MeshVertex::TEXTURE) {
			modelData->texCoord.resize(vertexCount);
			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				modelData->texCoord[lastVertexCount + index] = vertexArray[index].texCoord;
			}
		}

		if(flags & MeshVertex::sTANGENT) {
			modelData->sTangent.resize(vertexCount);
			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				modelData->sTangent[lastVertexCount + index] = vertexArray[index].sTangent;
			}
		}

		if(flags & MeshVertex::tTANGENT) {
			modelData->tTangent.resize(vertexCount);
			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				modelData->tTangent[lastVertexCount + index] = vertexArray[index].tTangent;
			}
		}

		if(flags & MeshVertex::COLOR) {
			modelData->color.resize(vertexCount);
			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				modelData->color[lastVertexCount + index] = vertexArray[index].color;
			}
		}

		if(flags & MeshVertex::BONES) {
			modelData->boneIds.resize(vertexCount);
			modelData->weights.resize(vertexCount);

			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				modelData->boneIds[lastVertexCount + index] = vertexArray[index].boneIds;
				modelData->weights[lastVertexCount + index] = vertexArray[index].weights;
			}
		}
	}

	ModelData::ModelData() :
			vertexBuffer(0), indexBuffer(0) {
	}

	ModelData::~ModelData() {
		if(graphicManager) {
			graphicManager->destroyBuffer(vertexBuffer);
			graphicManager->destroyBuffer(indexBuffer);
		}
	}

	void ModelData::uploadData(GraphicManager* graphicManager) {
		this->graphicManager = graphicManager;

		calculateAttributeOffsetsAndElementsPerVertex();

		vertexBuffer = graphicManager->createBuffer(elementsPerVertex * position.size() * sizeof(float), BufferType::VertexBuffer, FrequencyAccess::Static, NatureAccess::Draw);
		indexBuffer = graphicManager->createBuffer(indices.size() * sizeof(unsigned short), BufferType::IndexBuffer, FrequencyAccess::Static, NatureAccess::Draw);

		void* indexPtr = graphicManager->mapBuffer(indexBuffer, AccessType::WriteOnly);
		memcpy(indexPtr, indices.data(), indices.size() * sizeof(unsigned short));
		graphicManager->unmapBuffer(indexBuffer);

		float* vertexPtr = (float*) graphicManager->mapBuffer(vertexBuffer, AccessType::WriteOnly);
		float* begin = vertexPtr;

		for(size_t i = 0; i < position.size(); i++) {
			if(!boneIds.empty() && !weights.empty()) {
				math::Vector3 pos(0, 0, 0);

				if(boneIds[i].x > 0)
					pos += bindPose[(int) boneIds[i].x] * position[i] * weights[i].x;

				if(boneIds[i].y > 0)
					pos += bindPose[(int) boneIds[i].y] * position[i] * weights[i].y;

				*vertexPtr++ = pos.x;
				*vertexPtr++ = pos.y;
				*vertexPtr++ = pos.z;

				*vertexPtr++ = boneIds[i].x;
				*vertexPtr++ = boneIds[i].y;
				*vertexPtr++ = boneIds[i].z;
				*vertexPtr++ = boneIds[i].w;

				*vertexPtr++ = weights[i].x;
				*vertexPtr++ = weights[i].y;
				*vertexPtr++ = weights[i].z;
				*vertexPtr++ = weights[i].w;
			} else {
				*vertexPtr++ = position[i].x;
				*vertexPtr++ = position[i].y;
				*vertexPtr++ = position[i].z;

				*vertexPtr++ = 0;
				*vertexPtr++ = -1;
				*vertexPtr++ = -1;
				*vertexPtr++ = -1;

				*vertexPtr++ = 1;
				*vertexPtr++ = 0;
				*vertexPtr++ = 0;
				*vertexPtr++ = 0;
			}

			if(!normal.empty()) {
				*vertexPtr++ = normal[i].x;
				*vertexPtr++ = normal[i].y;
				*vertexPtr++ = normal[i].z;
			}

			if(!sTangent.empty() && !tTangent.empty()) {
				*vertexPtr++ = sTangent[i].x;
				*vertexPtr++ = sTangent[i].y;
				*vertexPtr++ = sTangent[i].z;

				*vertexPtr++ = tTangent[i].x;
				*vertexPtr++ = tTangent[i].y;
				*vertexPtr++ = tTangent[i].z;
			}

			if(!color.empty()) {
				*vertexPtr++ = color[i].x;
				*vertexPtr++ = color[i].y;
				*vertexPtr++ = color[i].z;
			}

			if(!texCoord.empty()) {
				*vertexPtr++ = texCoord[i].x;
				*vertexPtr++ = texCoord[i].y;
			}
		}

		graphicManager->unmapBuffer(vertexBuffer);
	}

	void ModelData::calculateAttributeOffsetsAndElementsPerVertex() {
		elementsPerVertex = 3;

		attributeOffsets[PositionOffset] = 0;

		attributeOffsets[BoneIdsOffset] = elementsPerVertex * sizeof(float);
		elementsPerVertex += 4;

		attributeOffsets[WeigthsOffset] = elementsPerVertex * sizeof(float);
		elementsPerVertex += 4;

		if(!normal.empty()) {
			attributeOffsets[NormalOffset] = elementsPerVertex * sizeof(float);
			elementsPerVertex += 3;
		}

		if(!sTangent.empty() && !tTangent.empty()) {
			attributeOffsets[STangentOffset] = elementsPerVertex * sizeof(float);
			elementsPerVertex += 3;

			attributeOffsets[TTangentOffset] = elementsPerVertex * sizeof(float);
			elementsPerVertex += 3;
		}

		if(!color.empty()) {
			attributeOffsets[ColorOffset] = elementsPerVertex * sizeof(float);
			elementsPerVertex += 3;
		}

		if(!texCoord.empty()) {
			attributeOffsets[TexCoordOffset] = elementsPerVertex * sizeof(float);
			elementsPerVertex += 2;
		}
	}

	void ModelData::calculateTangent() {
		if(texCoord.empty()) {
			return;
		}

		if(!sTangent.empty() && !tTangent.empty()) {
			return;
		}

		if(normal.empty()) {
			calculateNormal();
		}

		sTangent.resize(position.size(), math::Vector3(0, 0, 0));
		tTangent.resize(position.size(), math::Vector3(0, 0, 0));

		preCalculateTangent();

		for(unsigned int i = 0; i < position.size(); i++) {
			const math::Vector3 n = normal[i];
			const math::Vector3 t = sTangent[i];

			const math::Vector3 newTangent = (t - (n * math::dot(n, t))).normalize();
			float mul = math::dot(math::cross(n, t), tTangent[i]) < 0 ? -1.0 : 1.0;

			sTangent[i] = newTangent;
			tTangent[i] = (math::cross(newTangent, n) * mul).normalize();
		}
	}

	void ModelData::preCalculateTangent() {
		for(size_t j = 0; j < indices.size(); j += 3) {
			unsigned int i1 = indices[j+0];
			unsigned int i2 = indices[j+1];
			unsigned int i3 = indices[j+2];

			const math::Vector3 P1 = position[i1];
			const math::Vector3 P2 = position[i2];
			const math::Vector3 P3 = position[i3];

			const math::Vector2 UV1 = texCoord[i1];
			const math::Vector2 UV2 = texCoord[i2];
			const math::Vector2 UV3 = texCoord[i3];

			const math::Vector3 Q1 = P2 - P1;
			const math::Vector3 Q2 = P3 - P1;
			const math::Vector2 Q1uv = UV2 - UV1;
			const math::Vector2 Q2uv = UV3 - UV1;

			float cp = Q1uv.y * Q2uv.x - Q1uv.x * Q2uv.y;

			if(cp != 0.0f) {
				float mul = 1.0f / cp;

				const math::Vector3 sdir = (Q2 * Q1uv.y - Q1 * Q2uv.y) * mul;
				const math::Vector3 tdir = (Q2 * Q1uv.x - Q1 * Q2uv.x) * mul;

				sTangent[i1] += sdir;
				sTangent[i2] += sdir;
				sTangent[i3] += sdir;

				tTangent[i1] += tdir;
				tTangent[i2] += tdir;
				tTangent[i3] += tdir;
			}
		}
	}

	void ModelData::calculateNormal() {
		if(!normal.empty()) {
			return;
		}
	}

	Model::Model(const std::string& name, ResourceManager* manager) :
			Resource(name, manager), modelData(0) {
	}

	Model::~Model() {
		for(Mesh* mesh : meshes) {
			manager->unloadResource(mesh->material);
			delete mesh;
		}
		delete modelData;
	}

	void Model::uploadData(GraphicManager* graphicManager) {
		modelData->uploadData(graphicManager);
	}

	void* ModelUtils::read(ResourceStream& stream, ResourceManager& manager, void* instance) {
		std::vector<math::Vector3> position;
		std::vector<math::Vector3> normal;
		std::vector<math::Vector3> color;
		std::vector<math::Vector3> sTangent;
		std::vector<math::Vector3> tTangent;
		std::vector<math::Vector2> texCoord;
		std::vector<math::Vector4> boneIds;
		std::vector<math::Vector4> weights;
		std::vector<unsigned short> indices;
		std::vector<Mesh*> indicesMesh;

		math::Vector3 aabboxMin, aabboxMax;
		std::vector<Bone> bones;
		float animationFps;
		float currentTime;
		int totalFrames;

		stream.pushGroup("model");

		std::string type = stream.readString("type");
		std::string name = stream.readString("name");

		position.resize(stream.readInt("vertexCount"));
		stream.readArray("positionData", (float*)position.data(), position.size() * 3);

		normal.resize(stream.readInt("vertexCount"));
		stream.readArray("normalData", (float*)normal.data(), normal.size() * 3);

		sTangent.resize(stream.readInt("vertexCount"));
		stream.readArray("sTangetData", (float*)sTangent.data(), sTangent.size() * 3);

		tTangent.resize(stream.readInt("vertexCount"));
		stream.readArray("tTangetData", (float*)tTangent.data(), tTangent.size() * 3);

		color.resize(stream.readInt("vertexCount"));
		stream.readArray("colorData", (float*)color.data(), color.size() * 3);

		texCoord.resize(stream.readInt("vertexCount"));
		stream.readArray("texCoordData", (float*)texCoord.data(), texCoord.size() * 2);

		boneIds.resize(stream.readInt("vertexCount"));
		stream.readArray("boneIdsData", (float*)boneIds.data(), boneIds.size() * 4);

		weights.resize(stream.readInt("vertexCount"));
		stream.readArray("weightsData", (float*)weights.data(), weights.size() * 4);

		indices.resize(stream.readInt("indexCount"));
		stream.readArray("indexData", (short*) indices.data(), indices.size());

		indicesMesh.resize(stream.readInt("indexMeshCount"));
		for(size_t i = 0; i < indicesMesh.size(); i++) {
			Mesh* indexMesh = new Mesh;
			indicesMesh[i] = indexMesh;

			stream.pushGroup("indexMesh");

			indexMesh->offset = stream.readShort("offset");
			indexMesh->count = stream.readShort("count");
			indexMesh->start = stream.readShort("start");
			indexMesh->end = stream.readShort("end");
			indexMesh->material = (Material*)manager.loadResource(MaterialKey(stream.readString("material")));

			stream.popGroup();
		}

		bones.resize(stream.readInt("bonesCount"));
		for(size_t i = 0; i < bones.size(); i++) {
			Bone& bone = bones[i];

			stream.pushGroup("bone");

			bone.name = stream.readString("name");
			bone.parentIndex = stream.readInt("parentIndex");
			stream.readArray("localSkeleton", bone.localSkeleton.matrix, 16);

			bone.positions.resize(stream.readInt("positionsCount"));
			for(size_t p = 0; p < bone.positions.size(); p++) {
				KeyFramePosition& position = bone.positions[p];

				stream.pushGroup("keyframePosition");

				position.time = stream.readFloat("time");
				stream.readArray("position", position.position.vector, 3);

				stream.popGroup();
			}

			bone.rotations.resize(stream.readInt("rotationsCount"));
			for(size_t r = 0; r < bone.rotations.size(); r++) {
				KeyFrameRotation& rotation = bone.rotations[r];

				stream.pushGroup("keyframeRotation");

				rotation.time = stream.readFloat("time");
				stream.readArray("rotation", rotation.rotation.quat, 4);

				stream.popGroup();
			}

			stream.popGroup();
		}

		animationFps = stream.readFloat("animationFps");
		currentTime = stream.readFloat("currentTime");
		totalFrames = stream.readInt("totalFrames");

		stream.popGroup();

		Model* model = new Model(name, &manager);

		model->modelData = new ModelData;

		model->meshes = indicesMesh;
		model->modelData->position = position;
		model->modelData->normal = normal;
		model->modelData->color = color;
		model->modelData->sTangent = sTangent;
		model->modelData->tTangent = tTangent;
		model->modelData->texCoord = texCoord;
		model->modelData->boneIds = boneIds;
		model->modelData->weights = weights;
		model->modelData->indices = indices;

		model->animation.bones = bones;
		model->animation.animationFps = animationFps;
		model->animation.currentTime = currentTime;
		model->animation.totalFrames = totalFrames;

		model->animation.updateBones();

		model->modelData->bindPose.resize(model->animation.bones.size());
		for(size_t i = 0; i < model->animation.bones.size(); i++) {
			Bone& bone = model->animation.bones[i];

			model->modelData->bindPose[i] = bone.globalSkeleton.inverse();
			model->modelData->bindPose[i] = math::Matrix4::IDENTITY;
		}

		model->modelData->calculateTangent();
		model->modelData->calculateBoundingBox();
		//model->geometry->uploadData();

		return model;
	}

	void ModelUtils::write(ResourceStream& stream, ResourceManager& manager, void* instance) {
		Model* model = dynamic_cast<Model*>((Model*) instance);

		if(!model)
			throw Exception("Resource is not a model");

		stream.pushGroup("model");

		stream.writeString("type", model->getType().getName());
		stream.writeString("name", model->name);

		stream.writeInt("vertexCount", model->modelData->position.size());
		stream.writeArray("positionData", (float*)model->modelData->position.data(), model->modelData->position.size() * 3);

		stream.writeInt("vertexCount", model->modelData->normal.size());
		stream.writeArray("normalData", (float*)model->modelData->normal.data(), model->modelData->normal.size() * 3);

		stream.writeInt("vertexCount", model->modelData->sTangent.size());
		stream.writeArray("sTangetData", (float*)model->modelData->sTangent.data(), model->modelData->sTangent.size() * 3);

		stream.writeInt("vertexCount", model->modelData->tTangent.size());
		stream.writeArray("tTangetData", (float*)model->modelData->tTangent.data(), model->modelData->tTangent.size() * 3);

		stream.writeInt("vertexCount", model->modelData->color.size());
		stream.writeArray("colorData", (float*)model->modelData->color.data(), model->modelData->color.size() * 3);

		stream.writeInt("vertexCount", model->modelData->texCoord.size());
		stream.writeArray("texCoordData", (float*)model->modelData->texCoord.data(), model->modelData->texCoord.size() * 2);

		stream.writeInt("vertexCount", model->modelData->boneIds.size());
		stream.writeArray("boneIdsData", (float*)model->modelData->boneIds.data(), model->modelData->boneIds.size() * 4);

		stream.writeInt("vertexCount", model->modelData->weights.size());
		stream.writeArray("weightsData", (float*)model->modelData->weights.data(), model->modelData->weights.size() * 4);

		stream.writeInt("indexCount", model->modelData->indices.size());
		stream.writeArray("indexData", (short*)model->modelData->indices.data(), model->modelData->indices.size());

		stream.writeInt("indexMeshCount", model->meshes.size());
		for(size_t i = 0; i < model->meshes.size(); i++) {
			Mesh* indexMesh = model->meshes[i];

			stream.pushGroup("indexMesh");

			stream.writeShort("offset", indexMesh->offset);
			stream.writeShort("count", indexMesh->count);
			stream.writeShort("start", indexMesh->start);
			stream.writeShort("end", indexMesh->end);
			stream.writeString("material", indexMesh->material->getName());

			stream.popGroup();
		}

		stream.writeInt("bonesCount", model->animation.bones.size());
		for(size_t i = 0; i < model->animation.bones.size(); i++) {
			const Bone& bone = model->animation.bones[i];

			stream.pushGroup("bone");

			stream.writeString("name", bone.name);
			stream.writeInt("parentIndex", bone.parentIndex);
			stream.writeArray("localSkeleton", bone.localSkeleton.matrix, 16);

			stream.writeInt("positionsCount", bone.positions.size());
			for(size_t p = 0; p < bone.positions.size(); p++) {
				const KeyFramePosition& position = bone.positions[p];

				stream.pushGroup("keyframePosition");

				stream.writeFloat("time", position.time);
				stream.writeArray("position", position.position.vector, 3);

				stream.popGroup();
			}

			stream.writeInt("rotationsCount", bone.rotations.size());
			for(size_t r = 0; r < bone.rotations.size(); r++) {
				const KeyFrameRotation& rotation = bone.rotations[r];

				stream.pushGroup("keyframeRotations");

				stream.writeFloat("time", rotation.time);
				stream.writeArray("rotation", rotation.rotation.quat, 4);

				stream.popGroup();
			}

			stream.popGroup();
		}

		stream.writeFloat("animationFps", model->animation.animationFps);
		stream.writeFloat("currentTime", model->animation.currentTime);
		stream.writeInt("totalFrames", model->animation.totalFrames);

		stream.popGroup();
	}

	Resource* ModelKey::loadResource(class ResourceManager& manager) const {
		std::string modelName = getName();

		FileStream fileStream("resources/scene/" + modelName + ".model");
		ResourceBinStream resourceStream(fileStream);
		Model* model = (Model*) ModelUtils::read(resourceStream, manager, 0);

		ModelEvent event;
		event.type = "model";
		event.model = model;

		manager.dispatchLoadedEvent(event);

		return model;
	}

} /* namespace engine */
