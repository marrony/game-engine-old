#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Frustum.h"
#include "SpatialLocation.h"
#include "Octree.h"
#include "OctreeBuilder.h"
#include "MockSceneTreeObject.h"

using engine::math::Vector3;

using testing::ReturnRef;

TEST(Octree, CreateOctreeWithoutObjects) {
	engine::Octree octree(Vector3(0, 0, 0), 10);

	engine::QueryList queryList;
	octree.queryObjects(queryList);

	ASSERT_TRUE(queryList.isEmpty());
}

TEST(Octree, CreateOctreeWithOneObject) {
	engine::Octree octree(Vector3(0, 0, 0), 10);

	MockSceneTreeObject* mock = new MockSceneTreeObject(Vector3(2, 2, 2), 1);
	octree.addObject(mock);

	engine::QueryList queryList;
	octree.queryObjects(queryList);

	ASSERT_EQ(1, queryList.size());
}

TEST(Octree, CreateOctreeWithObjects) {
	engine::Octree octree(Vector3(0, 0, 0), 20);

	MockSceneTreeObject* mock0 = new MockSceneTreeObject(Vector3(1, 1, 1), 1);
	octree.addObject(mock0);

	MockSceneTreeObject* mock1 = new MockSceneTreeObject(Vector3(2, 2, 2), 1);
	octree.addObject(mock1);

	MockSceneTreeObject* mock2 = new MockSceneTreeObject(Vector3(3, 3, 3), 1);
	octree.addObject(mock2);

	MockSceneTreeObject* mock3 = new MockSceneTreeObject(Vector3(4, 4, 4), 1);
	octree.addObject(mock3);

	engine::QueryList queryList;
	octree.queryObjects(queryList);

	ASSERT_EQ(4, queryList.size());
}

/**
 * octants
 *
 * 0 = [- - -]
 * 1 = [- - +]
 * 2 = [- + -]
 * 3 = [- + +]
 * 4 = [+ - -]
 * 5 = [+ - +]
 * 6 = [+ + -]
 * 7 = [+ + +]
 */

class QueryObjectFixture :  public testing::Test {
public:
	QueryObjectFixture() :
		octree(Vector3(0, 0, 0), 20),
		frustum() {
	}

	virtual void SetUp() {
		directions[0] = Vector3(-1, -1, -1).normalize();
		objectInOctant[0] = new MockSceneTreeObject(Vector3(-15, -10, -15), 2);
		octree.addObject(objectInOctant[0]);

		directions[1] = Vector3(-1, -1, +1).normalize();
		objectInOctant[1] = new MockSceneTreeObject(Vector3(-15, -10, +15), 2);
		octree.addObject(objectInOctant[1]);

		directions[2] = Vector3(-1, +1, -1).normalize();
		objectInOctant[2] = new MockSceneTreeObject(Vector3(-15, +10, -15), 2);
		octree.addObject(objectInOctant[2]);

		directions[3] = Vector3(-1, +1, +1).normalize();
		objectInOctant[3] = new MockSceneTreeObject(Vector3(-15, +10, +15), 2);
		octree.addObject(objectInOctant[3]);

		directions[4] = Vector3(+1, -1, -1).normalize();
		objectInOctant[4] = new MockSceneTreeObject(Vector3(+15, -10, -15), 2);
		octree.addObject(objectInOctant[4]);

		directions[5] = Vector3(+1, -1, +1).normalize();
		objectInOctant[5] = new MockSceneTreeObject(Vector3(+15, -10, +15), 2);
		octree.addObject(objectInOctant[5]);

		directions[6] = Vector3(+1, +1, -1).normalize();
		objectInOctant[6] = new MockSceneTreeObject(Vector3(+15, +10, -15), 2);
		octree.addObject(objectInOctant[6]);

		directions[7] = Vector3(+1, +1, +1).normalize();
		objectInOctant[7] = new MockSceneTreeObject(Vector3(+15, +10, +15), 2);
		octree.addObject(objectInOctant[7]);

		frustum.setPerspective(60.0, 1.0);
	}

	engine::Octree octree;
	MockSceneTreeObject* objectInOctant[8];
	Vector3 directions[8];
	engine::Frustum frustum;
};

TEST_F(QueryObjectFixture, QueryObjectInOctant0) {
	frustum.setDirection(directions[0]);

	engine::QueryList queryList;
	queryList.setFrustum(&frustum);

	octree.queryObjects(queryList);

	ASSERT_EQ(1, queryList.size());

	ASSERT_EQ(objectInOctant[0], queryList.getObject(0));
}

TEST_F(QueryObjectFixture, QueryObjectInOctant1) {
	frustum.setDirection(directions[1]);

	engine::QueryList queryList;
	queryList.setFrustum(&frustum);

	octree.queryObjects(queryList);

	ASSERT_EQ(1, queryList.size());

	ASSERT_EQ(objectInOctant[1], queryList.getObject(0));
}

TEST_F(QueryObjectFixture, QueryObjectInOctant2) {
	frustum.setDirection(directions[2]);

	engine::QueryList queryList;
	queryList.setFrustum(&frustum);

	octree.queryObjects(queryList);

	ASSERT_EQ(1, queryList.size());

	ASSERT_EQ(objectInOctant[2], queryList.getObject(0));
}

TEST_F(QueryObjectFixture, QueryObjectInOctant3) {
	frustum.setDirection(directions[3]);

	engine::QueryList queryList;
	queryList.setFrustum(&frustum);

	octree.queryObjects(queryList);

	ASSERT_EQ(1, queryList.size());

	ASSERT_EQ(objectInOctant[3], queryList.getObject(0));
}

TEST_F(QueryObjectFixture, QueryObjectInOctant4) {
	frustum.setDirection(directions[4]);

	engine::QueryList queryList;
	queryList.setFrustum(&frustum);

	octree.queryObjects(queryList);

	ASSERT_EQ(1, queryList.size());

	ASSERT_EQ(objectInOctant[4], queryList.getObject(0));
}

TEST_F(QueryObjectFixture, QueryObjectInOctant5) {
	frustum.setDirection(directions[5]);

	engine::QueryList queryList;
	queryList.setFrustum(&frustum);

	octree.queryObjects(queryList);

	ASSERT_EQ(1, queryList.size());

	ASSERT_EQ(objectInOctant[5], queryList.getObject(0));
}

TEST_F(QueryObjectFixture, QueryObjectInOctant6) {
	frustum.setDirection(directions[6]);

	engine::QueryList queryList;
	queryList.setFrustum(&frustum);

	octree.queryObjects(queryList);

	ASSERT_EQ(1, queryList.size());

	ASSERT_EQ(objectInOctant[6], queryList.getObject(0));
}

TEST_F(QueryObjectFixture, QueryObjectInOctant7) {
	frustum.setDirection(directions[7]);

	engine::QueryList queryList;
	queryList.setFrustum(&frustum);

	octree.queryObjects(queryList);

	ASSERT_EQ(1, queryList.size());

	ASSERT_EQ(objectInOctant[7], queryList.getObject(0));
}

TEST_F(QueryObjectFixture, QueryObjectOutsideFrustum) {
	frustum.setDirection(Vector3(0, 1, 0).normalize());

	engine::QueryList queryList;
	queryList.setFrustum(&frustum);

	octree.queryObjects(queryList);

	ASSERT_TRUE(queryList.isEmpty());
}

TEST_F(QueryObjectFixture, InsertObjectOutsideOfOctree) {
	MockSceneTreeObject* mock = new MockSceneTreeObject(Vector3(+25, +25, +25), 2);
	octree.addObject(mock);

	frustum.setPosition(Vector3(20, 20, 20));
	frustum.setDirection(Vector3(1, 1, 1).normalize());

	engine::QueryList queryList;
	queryList.setFrustum(&frustum);

	octree.queryObjects(queryList);

	ASSERT_EQ(1, queryList.size());

	ASSERT_EQ(mock, queryList.getObject(0));
}

//TEST_F(QueryObjectFixture, RemoveObject) {
//	octree.removeObject(objectInOctant[3]);
//
//	ASSERT_TRUE(false);
//}
