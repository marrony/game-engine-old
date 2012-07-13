#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Component.h"

using namespace engine;

class MockComponent : public Component {
public:
};

TEST(Component, Owner) {
	GameEntity* entity = (GameEntity*) 0x1234;

	MockComponent component;

//	component.setOwner(entity);
//
//	ASSERT_EQ(entity, component.getOwner());
}
