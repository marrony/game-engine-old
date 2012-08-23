/*
 * ModelBuilder.cpp
 *
 *  Created on: 08/08/2012
 *      Author: marrony.neris
 */

#include "GeometryData.h"
#include "Model.h"
#include "Buffer.h"
#include "Material.h"
#include "ResourceManager.h"

namespace engine {

	Mesh::Mesh() {
		memset(attributeOffsets, 0, sizeof(attributeOffsets));
	}

	Mesh::~Mesh() {
	}

	void Mesh::addVertexData(const std::vector<MeshVertex>& vertexArray, const std::vector<unsigned short>& newIndices, Material* material, int flags) {
		size_t lastVertexCount = position.size();

		Batch indexMesh;
		indexMesh.offset = indices.size();
		indexMesh.count = newIndices.size();
		indexMesh.start = lastVertexCount + *std::min_element(newIndices.begin(), newIndices.end());
		indexMesh.end = lastVertexCount + *std::max_element(newIndices.begin(), newIndices.end());
		batches.push_back(indexMesh);
		materials.push_back(material);

		for(size_t i = 0; i < newIndices.size(); i++) {
			indices.push_back(lastVertexCount + newIndices[i]);
		}

		size_t vertexCount = lastVertexCount + vertexArray.size();

		position.resize(vertexCount);
		for(size_t i = 0; i < newIndices.size(); i++) {
			size_t index = newIndices[i];

			position[lastVertexCount + index] = vertexArray[index].position;
		}

		if(flags & MeshVertex::NORMAL) {
			normal.resize(vertexCount);
			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				normal[lastVertexCount + index] = vertexArray[index].normal;
			}
		}

		if(flags & MeshVertex::TEXTURE) {
			texCoord.resize(vertexCount);
			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				texCoord[lastVertexCount + index] = vertexArray[index].texCoord;
			}
		}

		if(flags & MeshVertex::sTANGENT) {
			sTangent.resize(vertexCount);
			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				sTangent[lastVertexCount + index] = vertexArray[index].sTangent;
			}
		}

		if(flags & MeshVertex::tTANGENT) {
			tTangent.resize(vertexCount);
			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				tTangent[lastVertexCount + index] = vertexArray[index].tTangent;
			}
		}

		if(flags & MeshVertex::COLOR) {
			color.resize(vertexCount);
			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				color[lastVertexCount + index] = vertexArray[index].color;
			}
		}

		if(flags & MeshVertex::BONES) {
			boneIds.resize(vertexCount);
			weights.resize(vertexCount);

			for(size_t i = 0; i < newIndices.size(); i++) {
				size_t index = newIndices[i];

				boneIds[lastVertexCount + index] = vertexArray[index].boneIds;
				weights[lastVertexCount + index] = vertexArray[index].weights;
			}
		}
	}

	void Mesh::calculateAttributeOffsetsAndElementsPerVertex() {
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

	void Mesh::calculateTangent() {
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

	void Mesh::preCalculateTangent() {
		for(size_t j = 0; j < indices.size(); j += 3) {
			unsigned int i1 = indices[j + 0];
			unsigned int i2 = indices[j + 1];
			unsigned int i3 = indices[j + 2];

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

	void Mesh::calculateBoundingBox() {
		boundingBox.reset();

		for(const math::Vector3& v : position)
			boundingBox.merge(v);
	}

	void Mesh::calculateNormal() {
		if(!normal.empty()) {
			return;
		}
	}

	void Mesh::readFromStream(ResourceManager& manager, ResourceStream& stream) {
		stream.pushGroup("model");

		std::string type = stream.readString("type");
		name = stream.readString("name");

		position.resize(stream.readInt("vertexCount"));
		stream.readArray("positionData", (float*) position.data(), position.size() * 3);

		normal.resize(stream.readInt("vertexCount"));
		stream.readArray("normalData", (float*) normal.data(), normal.size() * 3);

		sTangent.resize(stream.readInt("vertexCount"));
		stream.readArray("sTangetData", (float*) sTangent.data(), sTangent.size() * 3);

		tTangent.resize(stream.readInt("vertexCount"));
		stream.readArray("tTangetData", (float*) tTangent.data(), tTangent.size() * 3);

		color.resize(stream.readInt("vertexCount"));
		stream.readArray("colorData", (float*) color.data(), color.size() * 3);

		texCoord.resize(stream.readInt("vertexCount"));
		stream.readArray("texCoordData", (float*) texCoord.data(), texCoord.size() * 2);

		boneIds.resize(stream.readInt("vertexCount"));
		stream.readArray("boneIdsData", (float*) boneIds.data(), boneIds.size() * 4);

		weights.resize(stream.readInt("vertexCount"));
		stream.readArray("weightsData", (float*) weights.data(), weights.size() * 4);

		indices.resize(stream.readInt("indexCount"));
		stream.readArray("indexData", (short*) indices.data(), indices.size());

		batches.resize(stream.readInt("indexMeshCount"));
		materials.resize(batches.size());
		for(size_t i = 0; i < batches.size(); i++) {
			stream.pushGroup("indexMesh");

			batches[i].offset = stream.readShort("offset");
			batches[i].count = stream.readShort("count");
			batches[i].start = stream.readShort("start");
			batches[i].end = stream.readShort("end");
			materials[i] = (Material*) manager.loadResource(MaterialKey(stream.readString("material")));

			stream.popGroup();
		}

#if 0
		animation.bones.resize(stream.readInt("bonesCount"));
		for(size_t i = 0; i < animation.bones.size(); i++) {
			Bone& bone = animation.bones[i];

			stream.pushGroup("bone");

			bone.name = stream.readString("name");
			bone.parentIndex = stream.readInt("parentIndex");
			stream.readArray("localSkeleton", bone.localSkeleton.matrix, 16);

			bone.keyframes.resize(stream.readInt("keyframesCount"));
			for(size_t p = 0; p < bone.keyframes.size(); p++) {
				KeyFrame& keyframe = bone.keyframes[p];

				stream.pushGroup("keyframe");

				keyframe.time = stream.readFloat("time");
				stream.readArray("position", keyframe.position.vector, 3);
				stream.readArray("rotation", keyframe.rotation.quat, 4);

				stream.popGroup();
			}

			stream.popGroup();
		}

		animation.animationFps = stream.readFloat("animationFps");
		animation.currentTime = stream.readFloat("currentTime");
		animation.totalFrames = stream.readInt("totalFrames");

		stream.popGroup();

		bindPose.resize(animation.bones.size());
		for(size_t i = 0; i < animation.bones.size(); i++) {
			Bone& bone = animation.bones[i];

			bindPose[i] = bone.globalSkeleton.inverse();
			bindPose[i] = math::Matrix4::IDENTITY;
		}

		animation.updateBones();
#endif

		calculateTangent();
		calculateBoundingBox();
		calculateAttributeOffsetsAndElementsPerVertex();
	}

	void Mesh::writeToStream(ResourceStream& stream) {
		stream.pushGroup("model");

		stream.writeString("type", "model");
		stream.writeString("name", name);

		stream.writeInt("vertexCount", position.size());
		stream.writeArray("positionData", (float*)position.data(), position.size() * 3);

		stream.writeInt("vertexCount", normal.size());
		stream.writeArray("normalData", (float*)normal.data(), normal.size() * 3);

		stream.writeInt("vertexCount", sTangent.size());
		stream.writeArray("sTangetData", (float*)sTangent.data(), sTangent.size() * 3);

		stream.writeInt("vertexCount", tTangent.size());
		stream.writeArray("tTangetData", (float*)tTangent.data(), tTangent.size() * 3);

		stream.writeInt("vertexCount", color.size());
		stream.writeArray("colorData", (float*)color.data(), color.size() * 3);

		stream.writeInt("vertexCount", texCoord.size());
		stream.writeArray("texCoordData", (float*)texCoord.data(), texCoord.size() * 2);

		stream.writeInt("vertexCount", boneIds.size());
		stream.writeArray("boneIdsData", (float*)boneIds.data(), boneIds.size() * 4);

		stream.writeInt("vertexCount", weights.size());
		stream.writeArray("weightsData", (float*)weights.data(), weights.size() * 4);

		stream.writeInt("indexCount", indices.size());
		stream.writeArray("indexData", (short*)indices.data(), indices.size());

		stream.writeInt("indexMeshCount", batches.size());
		for(size_t i = 0; i < batches.size(); i++) {
			stream.pushGroup("indexMesh");

			stream.writeShort("offset", batches[i].offset);
			stream.writeShort("count", batches[i].count);
			stream.writeShort("start", batches[i].start);
			stream.writeShort("end", batches[i].end);
			stream.writeString("material", materials[i]->getName());

			stream.popGroup();
		}

#if 0
		stream.writeInt("bonesCount", animation.bones.size());
		for(size_t i = 0; i < animation.bones.size(); i++) {
			const Bone& bone = animation.bones[i];

			stream.pushGroup("bone");

			stream.writeString("name", bone.name);
			stream.writeInt("parentIndex", bone.parentIndex);
			stream.writeArray("localSkeleton", bone.localSkeleton.matrix, 16);

			stream.writeInt("keyframesCount", bone.keyframes.size());
			for(size_t p = 0; p < bone.keyframes.size(); p++) {
				const KeyFrame& keyframe = bone.keyframes[p];

				stream.pushGroup("keyframePosition");

				stream.writeFloat("time", keyframe.time);
				stream.writeArray("position", keyframe.position.vector, 3);
				stream.writeArray("rotation", keyframe.rotation.quat, 4);

				stream.popGroup();
			}

			stream.popGroup();
		}

		stream.writeFloat("animationFps", animation.animationFps);
		stream.writeFloat("currentTime", animation.currentTime);
		stream.writeInt("totalFrames", animation.totalFrames);
#endif

		stream.popGroup();
	}

} /* namespace engine */
