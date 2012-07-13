#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "KeyboardManager.h"

class KeyboardManagerTest : public testing::Test {
public:
	MOCK_METHOD2(onKeyDown, void(void*, engine::KeyEvent));
	MOCK_METHOD2(onKeyUp, void(void*, engine::KeyEvent));

	virtual void SetUp() {
		manager.onKeyDown += [&](void* sender, engine::KeyEvent event) {
			onKeyDown(sender, event);
		};

		manager.onKeyUp += [&](void* sender, engine::KeyEvent event) {
			onKeyUp(sender, event);
		};
	}

	engine::KeyboardManager manager;
};

TEST_F(KeyboardManagerTest, OnKeyDown) {
	EXPECT_CALL(*this, onKeyDown(&manager, engine::KeyEvent('a')));

	manager.keyDown('a');

	ASSERT_TRUE(manager.isKeyPressed('a'));
}

TEST_F(KeyboardManagerTest, OnKeyUp) {
	EXPECT_CALL(*this, onKeyUp(&manager, engine::KeyEvent('a')));

	manager.keyUp('a');

	ASSERT_FALSE(manager.isKeyPressed('a'));
}

TEST_F(KeyboardManagerTest, TwoKeysDown) {
	EXPECT_CALL(*this, onKeyDown(&manager, engine::KeyEvent('w')));
	EXPECT_CALL(*this, onKeyDown(&manager, engine::KeyEvent('s')));

	manager.keyDown('w');
	manager.keyDown('s');

	ASSERT_TRUE(manager.isKeyPressed('w'));
	ASSERT_TRUE(manager.isKeyPressed('s'));
}

TEST_F(KeyboardManagerTest, PressTwoKeysAndReleaseOne) {
	EXPECT_CALL(*this, onKeyDown(&manager, engine::KeyEvent('w')));
	EXPECT_CALL(*this, onKeyDown(&manager, engine::KeyEvent('s')));

	manager.keyDown('w');
	manager.keyDown('s');

	EXPECT_CALL(*this, onKeyUp(&manager, engine::KeyEvent('w')));

	manager.keyUp('w');

	ASSERT_FALSE(manager.isKeyPressed('w'));
	ASSERT_TRUE(manager.isKeyPressed('s'));
}
