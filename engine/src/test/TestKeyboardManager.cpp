#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "KeyboardManager.h"

class KeyboardManagerTest : public testing::Test {
public:
	MOCK_METHOD1(onKeyDown, void(engine::KeyEvent));
	MOCK_METHOD1(onKeyUp, void(engine::KeyEvent));

	virtual void SetUp() {
		manager.onKeyDown += [&](engine::KeyEvent event) {
			onKeyDown(event);
		};

		manager.onKeyUp += [&](engine::KeyEvent event) {
			onKeyUp(event);
		};
	}

	engine::KeyboardManager manager;
};

TEST_F(KeyboardManagerTest, OnKeyDown) {
	EXPECT_CALL(*this, onKeyDown(engine::KeyEvent('a')));

	manager.keyDown('a');

	ASSERT_TRUE(manager.isKeyPressed('a'));
}

TEST_F(KeyboardManagerTest, OnKeyUp) {
	EXPECT_CALL(*this, onKeyUp(engine::KeyEvent('a')));

	manager.keyUp('a');

	ASSERT_FALSE(manager.isKeyPressed('a'));
}

TEST_F(KeyboardManagerTest, TwoKeysDown) {
	EXPECT_CALL(*this, onKeyDown(engine::KeyEvent('w')));
	EXPECT_CALL(*this, onKeyDown(engine::KeyEvent('s')));

	manager.keyDown('w');
	manager.keyDown('s');

	ASSERT_TRUE(manager.isKeyPressed('w'));
	ASSERT_TRUE(manager.isKeyPressed('s'));
}

TEST_F(KeyboardManagerTest, PressTwoKeysAndReleaseOne) {
	EXPECT_CALL(*this, onKeyDown(engine::KeyEvent('w')));
	EXPECT_CALL(*this, onKeyDown(engine::KeyEvent('s')));

	manager.keyDown('w');
	manager.keyDown('s');

	EXPECT_CALL(*this, onKeyUp(engine::KeyEvent('w')));

	manager.keyUp('w');

	ASSERT_FALSE(manager.isKeyPressed('w'));
	ASSERT_TRUE(manager.isKeyPressed('s'));
}
