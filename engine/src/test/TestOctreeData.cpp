#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>

#include "Frustum.h"
#include "SpatialLocation.h"
#include "Octree.h"
#include "MockSceneTreeObject.h"

using namespace engine;
using namespace engine::math;

class OctreeDataFixture : public testing::Test {
private:

	Vector3 parseVector(std::istream& stream) {
		char ignore;
		Vector3 vector;

		stream >> ignore;
		stream >> vector.x;
		stream >> vector.y;
		stream >> vector.z;
		stream >> ignore;

		return vector;
	}

	void parseComment(std::istream& stream) {
		char buffer[1024];

		stream.getline(buffer, sizeof(buffer));
	}

	void parseObject(std::istream& stream) {
		char parenthesis;
		Vector3 center;
		float size;

		center = parseVector(stream);
		stream >> size;

		octree.addObject(new MockSceneTreeObject(center, size));
	}

	void parsePerspective(std::istream& stream) {
		float fov;
		float aspect;
		float znear;
		float zfar;

		stream >> fov;
		stream >> aspect;
		stream >> znear;
		stream >> zfar;

		frustum.setPerspective(fov, aspect, znear, zfar);
	}

	void parseFrustum(std::istream& stream) {
		Vector3 position;
		Vector3 direction;
		int count;

		position = parseVector(stream);
		direction = parseVector(stream);
		stream >> count;

		frustum.setPosition(position);
		frustum.setDirection(direction);

		engine::QueryList queryList;
		queryList.setFrustum(&frustum);
		octree.queryObjects(queryList);

		ASSERT_EQ(count, queryList.size());
	}

public:
	OctreeDataFixture() :
			octree(Vector3(0, 0, 0), 20),
			frustum() {
	}

	virtual void SetUp() {
		frustum.setPerspective(60.0, 1.0);
	}

	void testFile(const char* file) {
		std::ifstream stream(file);

		if(!stream.is_open()) {
			throw "erro";
		}

		char command;
		while(!stream.eof() && (stream >> command)) {

			switch(command) {
			case '#':
				parseComment(stream);
				break;

			case 'o':
				parseObject(stream);
				break;

			case 'f':
				parseFrustum(stream);
				break;

			case 'p':
				parsePerspective(stream);
				break;
			}
		}
	}

public:
	Octree octree;
	SpatialLocation location;
	Frustum frustum;
};

TEST_F(OctreeDataFixture, SmallData) {

	testFile("engine/src/test/resources/octree-data.txt");

}

TEST_F(OctreeDataFixture, HugeData) {

}
