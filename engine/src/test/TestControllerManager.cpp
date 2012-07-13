#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ControllerManager.h"
#include "Component.h"
#include "Clock.h"

class MockController : public engine::ControllerComponent {
public:
	//virtual void update(float dt) = 0;
	MOCK_METHOD1(update, void(const engine::GameTime&));
};

class ControllerManagerTest : public testing::Test {
public:
	virtual void SetUp() {
		controller0 = new MockController;
		controller1 = new MockController;

		manager.addController(controller0);
		manager.addController(controller1);
	}

	engine::ControllerManager manager;
	MockController* controller0;
	MockController* controller1;
};

TEST_F(ControllerManagerTest, Update) {
	engine::GameTime gameTime(0.5, 60);

	EXPECT_CALL(*controller0, update(gameTime));
	EXPECT_CALL(*controller1, update(gameTime));

	manager.update(gameTime);
}
