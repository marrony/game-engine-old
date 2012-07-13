/*
 * TestResourceManager.cpp
 *
 *  Created on: 19/03/2012
 *      Author: marrony.neris
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ResourceManager.h"
#include "GraphicManager.h"
#include "GeneralHashFunction.h"
#include "Exception.h"

using namespace engine;
using namespace testing;

#if 0
class MockResource : public Resource {
public:
	MockResource(ResourceId id) : Resource(id) { }
	virtual ~MockResource() { dtor(); }

	MOCK_METHOD0(dtor, void(void));

	//virtual Type getType() const = 0;
	MOCK_CONST_METHOD0(getType, Type(void));

	//virtual void serialize(Serializer&)
	MOCK_METHOD1(serialize, void(Serializer&));

	//virtual void deserialize(Deserializer&)
	MOCK_METHOD1(deserialize, void(Deserializer&));
};

class MockResourceLoader : public ResourceLoader {
public:
	//virtual Resource* load(ResourceId id, std::istream& stream) = 0;
	MOCK_METHOD2(load, Resource*(const std::string&, std::istream&));
};

TEST(ResourceTest, CouldDelete) {
	ResourceId id = JSHash("teste", 5);
	MockResource resource(id);

	EXPECT_CALL(resource, dtor());

	ASSERT_TRUE(resource.couldDelete());
}

TEST(ResourceTest, IncrementReferenceCouldNotDelete) {
	ResourceId id = JSHash("teste", 5);
	MockResource resource(id);

	resource.incrementReference();

	EXPECT_CALL(resource, dtor());

	ASSERT_FALSE(resource.couldDelete());
}

TEST(ResourceTest, IncrementAndDecrementReference) {
	ResourceId id = JSHash("teste", 5);
	MockResource resource(id);

	resource.incrementReference();
	resource.decrementReference();

	EXPECT_CALL(resource, dtor());

	ASSERT_TRUE(resource.couldDelete());
}

TEST(ResourceManagerTest, RegisterLoader) {
	ResourceManager resourceManager;

	MockResourceLoader* resourceLoader = new MockResourceLoader;

	resourceManager.registerLoader("mock", "ext1;ext2", resourceLoader);

	ASSERT_EQ(resourceLoader, resourceManager.findLoader("ext1"));
	ASSERT_EQ(resourceLoader, resourceManager.findLoader("ext2"));
}

TEST(ResourceManagerTest, LoadResource) {
	ResourceId id = JSHash("teste.mock", 10);
	MockResourceLoader* resourceLoader = new MockResourceLoader;

	ResourceManager resourceManager;

	resourceManager.registerLoader("mock", "mock", resourceLoader);

	MockResource* resource = new MockResource(id);

	ON_CALL(*resourceLoader, load(id, _)).WillByDefault(Return(resource));

	EXPECT_CALL(*resourceLoader, load(id, _));
	EXPECT_CALL(*resource, dtor());

	//ASSERT_EQ(id, resourceManager.registerResource("teste", Type("mock")));
}

TEST(ResourceManagerTest, GetResource) {
	ResourceId id = JSHash("teste", 5);

	ResourceManager resourceManager;

	MockResource* resource = new MockResource(id);
	//resourceManager.addResource(resource);

	EXPECT_CALL(*resource, dtor());

	//ASSERT_EQ(resource, resourceManager.getResource(id));
}

TEST(ResourceManagerTest, FindResource) {
	ResourceId id = JSHash("teste", 5);

	ResourceManager resourceManager;

	MockResource* resource = new MockResource(id);
	//resourceManager.addResource(resource);

	Type type("teste");

	ON_CALL(*resource, getType()).WillByDefault(Return(type));
	EXPECT_CALL(*resource, getType());
	EXPECT_CALL(*resource, dtor());

	//ASSERT_EQ(id, resourceManager.findResource(type, "teste"));
}

TEST(ResourceManagerTest, DontFindFindResource) {
	ResourceManager resourceManager;

	Type type("teste");

	//ASSERT_EQ(0, resourceManager.findResource(type, "teste"));
}

TEST(ResourceManagerTest, DeleteUnusedResources) {
	ResourceId id = JSHash("teste", 5);
	MockResource* resource = new MockResource(id);

	ResourceManager resourceManager;

	//resourceManager.addResource(resource);

	EXPECT_CALL(*resource, dtor());

	//resourceManager.deleteUnusedResources();
}

class MockResource1 : public Resource {
public:
	MockResource1(ResourceId id, Resource* resource) : Resource(id), resource(resource) {
		resource->incrementReference();
	}

	virtual ~MockResource1() {
		resource->decrementReference();
		dtor();
	}

	MOCK_METHOD0(dtor, void(void));

	//virtual Type getType() const = 0;
	MOCK_CONST_METHOD0(getType, Type(void));

	//virtual void serialize(Serializer&)
	MOCK_METHOD1(serialize, void(Serializer&));

	//virtual void deserialize(Deserializer&)
	MOCK_METHOD1(deserialize, void(Deserializer&));

	Resource* resource;
};

TEST(ResourceManagerTest, DeleteResourcesWithReferencesToAnotherResources) {
	ResourceManager resourceManager;

	ResourceId id0 = JSHash("teste", 5);
	MockResource* resourceWithZeroReferences = new MockResource(id0);
	//resourceManager.addResource(resourceWithZeroReferences);

	ResourceId id1 = JSHash("teste1", 6);
	MockResource1* resourceWithOneReferences = new MockResource1(id1, resourceWithZeroReferences);
	//resourceManager.addResource(resourceWithOneReferences);

	EXPECT_CALL(*resourceWithZeroReferences, dtor());
	EXPECT_CALL(*resourceWithOneReferences, dtor());

	//resourceManager.deleteUnusedResources();

	//ASSERT_EQ(0, resourceManager.getResource(id0));
	//ASSERT_EQ(0, resourceManager.getResource(id1));
}

TEST(ResourceManagerTest, TryDestroyResourceManagerWithResouceReferenced) {
	ResourceManager* resourceManager = new ResourceManager;

	ResourceId id0 = JSHash("teste", 5);
	MockResource* resource = new MockResource(id0);
	//resourceManager->addResource(resource);

	resource->incrementReference();

	ASSERT_THROW(delete resourceManager, Exception);
}

TEST(ResourceManagerTest, GenerateId) {
	ResourceManager resourceManager;

	char resourceName[] = "Resource-0";
	ResourceId id = JSHash(resourceName, sizeof(resourceName) - 1);

	ASSERT_EQ(id, resourceManager.generateId());
}
#endif
