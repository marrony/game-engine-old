/*
 * ModelBuilder.h
 *
 *  Created on: 08/08/2012
 *      Author: marrony.neris
 */

#ifndef MODELBUILDER_H_
#define MODELBUILDER_H_

#include <vector>
#include <string>

#include "math/Vector.h"
#include "math/Matrix.h"

#include "Model.h"
#include "Bounding.h"
#include "Bone.h"
#include "Effect.h"

namespace engine {

	class ResourceStream;
	class ResourceManager;
	class Model;
	class Material;

	struct MeshVertex {
		math::Vector3 position;
		math::Vector3 normal;
		math::Vector3 sTangent;
		math::Vector3 tTangent;
		math::Vector3 color;
		math::Vector2 texCoord;
		math::Vector4 boneIds;
		math::Vector4 weights;

		enum VertexFlags {
			POSITION = 0x01,
			NORMAL = 0x02,
			TEXTURE = 0x04,
			sTANGENT = 0x08,
			tTANGENT = 0x10,
			COLOR = 0x20,
			BONES = 0x40
		};
	};

	class GeometryData {
	public:
		std::vector<math::Vector3> position;
		std::vector<math::Vector3> normal;
		std::vector<math::Vector3> sTangent;
		std::vector<math::Vector3> tTangent;
		std::vector<math::Vector3> color;
		std::vector<math::Vector2> texCoord;
		std::vector<math::Vector4> boneIds;
		std::vector<math::Vector4> weights;
		std::vector<unsigned short> indices;
		std::vector<math::Matrix4> bindPose;

		int elementsPerVertex;
		int attributeOffsets[MaxAttributeOffset];

		AABoundingBox boundingBox;
		std::vector<Batch> batches;
		std::vector<Material*> materials;
		std::string name;

		void calculateAttributeOffsetsAndElementsPerVertex();
		void calculateTangent();
		void calculateNormal();
		void preCalculateTangent();
		void calculateBoundingBox();
		//void createBuffers(Model* model);
	public:
		GeometryData();
		~GeometryData();

		std::string getName() const { return name; }
		void setName(const std::string& name) { this->name = name; }

		void addVertexData(const std::vector<MeshVertex>& vertexArray, const std::vector<unsigned short>& newIndices, Material* material, int flags);

		//Model* createModel(Model* model = 0);
		void readFromStream(ResourceManager& manager, ResourceStream& stream);
		void writeToStream(ResourceStream& stream);
	};

} /* namespace engine */

#endif /* MODELBUILDER_H_ */
