#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "OctreeNode.h"
#include "MockSceneTreeObject.h"

using engine::OctreeNode;
using engine::OctreeConfiguration;
using engine::math::Vector3;

TEST(OctreeNode, TestSplitNodeWithMaxObjects) {
	OctreeConfiguration configuration;

	configuration.maxLevel(1);
	configuration.maxObjects(2);
	configuration.minObjects(1);

	OctreeNode node(configuration, 0, Vector3(0, 0, 0), 100);

	MockSceneTreeObject* mock0 = new MockSceneTreeObject(Vector3(-10, -10, -10), 5);
	MockSceneTreeObject* mock1 = new MockSceneTreeObject(Vector3(-20, -20, -20), 5);

	ASSERT_TRUE(node.getChildAt(engine::OCTANT0) == 0);

	node.addObject(mock0);
	node.addObject(mock1);

	ASSERT_TRUE(node.getChildAt(engine::OCTANT0) != 0);
}

TEST(OctreeNode, TestThatSplitCarryObjectToChilds) {
	OctreeConfiguration configuration;

	configuration.maxLevel(1);
	configuration.maxObjects(2);
	configuration.minObjects(2);

	OctreeNode node(configuration, 0, Vector3(0, 0, 0), 100);

	MockSceneTreeObject* mock0 = new MockSceneTreeObject(Vector3(-10, -10, -10), 5);
	MockSceneTreeObject* mock1 = new MockSceneTreeObject(Vector3(-20, -20, -20), 5);
	MockSceneTreeObject* mock2 = new MockSceneTreeObject(Vector3(-30, -30, -30), 5);

	node.addObject(mock0);
	node.addObject(mock1);
	node.addObject(mock2);

	ASSERT_TRUE(node.getObjectList().isEmpty());

	const OctreeNode* octant0 = node.getChildAt(engine::OCTANT0);

	ASSERT_EQ(3, octant0->getObjectList().size());
}

TEST(OctreeNode, TestSplitNodeInManyChilds) {
	OctreeConfiguration configuration;

	configuration.maxLevel(2);
	configuration.maxObjects(4);
	configuration.minObjects(1);

	OctreeNode node(configuration, 0, Vector3(0, 0, 0), 20);

	MockSceneTreeObject* mock0 = new MockSceneTreeObject(Vector3(-10, -10, -10), 5);
	MockSceneTreeObject* mock1 = new MockSceneTreeObject(Vector3(-10, -10, +10), 5);
	MockSceneTreeObject* mock2 = new MockSceneTreeObject(Vector3(-10, +10, -10), 5);
	MockSceneTreeObject* mock3 = new MockSceneTreeObject(Vector3(-10, +10, +10), 5);

	node.addObject(mock0);
	node.addObject(mock1);
	node.addObject(mock2);
	node.addObject(mock3);

	ASSERT_TRUE(node.getChildAt(engine::OCTANT0) != 0);
	ASSERT_TRUE(node.getChildAt(engine::OCTANT1) != 0);
	ASSERT_TRUE(node.getChildAt(engine::OCTANT2) != 0);
	ASSERT_TRUE(node.getChildAt(engine::OCTANT3) != 0);
}

TEST(OctreeNode, TestMerge) {
	OctreeConfiguration configuration;

	configuration.maxLevel(2);
	configuration.maxObjects(2);
	configuration.minObjects(2);

	OctreeNode node(configuration, 0, Vector3(0, 0, 0), 100);

	MockSceneTreeObject* mock0 = new MockSceneTreeObject(Vector3(0, 0, 0), 5);
	MockSceneTreeObject* mock1 = new MockSceneTreeObject(Vector3(-50, -50, -50), 5);

	node.addObject(mock0);
	node.addObject(mock1);

	node.removeObject(mock1);

	ASSERT_EQ(1, node.getObjectList().size());
}

TEST(OctreeNode, TestMergeRemovingObjectFromIntermadiateNode) {
	OctreeConfiguration configuration;

	configuration.maxLevel(2);
	configuration.maxObjects(3);
	configuration.minObjects(2);

	OctreeNode node(configuration, 0, Vector3(0, 0, 0), 100);

	MockSceneTreeObject* mock0 = new MockSceneTreeObject(Vector3(-10, -10, -10), 5);
	MockSceneTreeObject* mock1 = new MockSceneTreeObject(Vector3(-50, -50, -50), 5);
	MockSceneTreeObject* mock2 = new MockSceneTreeObject(Vector3(-90, -90, -90), 5);

	node.addObject(mock0);
	node.addObject(mock1);
	node.addObject(mock2);

	ASSERT_TRUE(node.getChildAt(engine::OCTANT0) != 0);
	ASSERT_TRUE(node.getChildAt(engine::OCTANT0)->getChildAt(engine::OCTANT0) != 0);
	ASSERT_TRUE(node.getChildAt(engine::OCTANT0)->getChildAt(engine::OCTANT7) != 0);

	node.getChildAt(engine::OCTANT0)->removeObject(mock1);

	ASSERT_TRUE(node.getChildAt(engine::OCTANT0) == 0);

	ASSERT_EQ(2, node.getObjectList().size());
}

