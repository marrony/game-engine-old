#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "GameEntity.h"
#include "Component.h"

using namespace engine;

class EntityFixture : public testing::Test {
public:
	EntityFixture() :
		entity("entity name") {
	}

	GameEntity entity;
};

class ComponentMock0 : public Component {
public:
	MOCK_METHOD1(updateBindings, void(GameEntity*));
};

class ComponentMock1 : public Component {
public:
	virtual void updateBindings(GameEntity* owner) {
		mock0 = owner->getComponent<ComponentMock0>();
	}

	ComponentMock0* mock0;
};

TEST_F(EntityFixture, GetName) {
	ASSERT_EQ("entity name", entity.getName());
}

TEST_F(EntityFixture, GetComponent) {
	ComponentMock0* mock0 = new ComponentMock0;
	ComponentMock1* mock1 = new ComponentMock1;

	EXPECT_CALL(*mock0, updateBindings(&entity)).Times(2);

	entity.addComponent(mock0);
	entity.addComponent(mock1);

	ASSERT_EQ(mock0, entity.getComponent<ComponentMock0>());
	ASSERT_EQ(mock1, entity.getComponent<ComponentMock1>());
}

TEST_F(EntityFixture, UpdateBindings) {
	ComponentMock0* mock0 = new ComponentMock0;
	ComponentMock1* mock1 = new ComponentMock1;

	EXPECT_CALL(*mock0, updateBindings(&entity)).Times(2);

	entity.addComponent(mock0);
	entity.addComponent(mock1);

	ASSERT_EQ(mock0, mock1->mock0);
}

TEST_F(EntityFixture, RemoveComponents) {
	ComponentMock0* mock0 = new ComponentMock0;
	ComponentMock1* mock1 = new ComponentMock1;

	EXPECT_CALL(*mock0, updateBindings(&entity)).Times(2);

	entity.addComponent(mock0);
	entity.addComponent(mock1);

	entity.removeComponent<ComponentMock0>();

	ASSERT_TRUE(entity.getComponent<ComponentMock0>() == 0);
	ASSERT_TRUE(entity.getComponent<ComponentMock1>() != 0);
}
