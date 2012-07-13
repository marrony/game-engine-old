#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "MouseManager.h"

using namespace engine;

class EngineMouseListener : public testing::Test {
public:
	MOCK_METHOD2(onMouseMove, void(void*, MouseEvent));
	MOCK_METHOD2(onMouseUp, void(void*, MouseEvent));
	MOCK_METHOD2(onMouseDown, void(void*, MouseEvent));
	MOCK_METHOD2(onDoubleClick, void(void*, MouseEvent));

	virtual void SetUp() {
		manager.onMouseMove += [&](void* sender, MouseEvent event) {
			onMouseMove(sender, event);
		};

		manager.onMouseUp += [&](void* sender, MouseEvent event) {
			onMouseUp(sender, event);
		};

		manager.onMouseDown += [&](void* sender, MouseEvent event) {
			onMouseDown(sender, event);
		};

		manager.onDoubleClick += [&](void* sender, MouseEvent event) {
			onDoubleClick(sender, event);
		};
	}

	MouseManager manager;
};

TEST_F(EngineMouseListener, OnMove) {
	MouseState mouseState(10, 10);

	EXPECT_CALL(*this, onMouseMove(&manager, MouseEvent(mouseState)));
	manager.setMousePosition(10, 10);
}

TEST_F(EngineMouseListener, OnLeftDown) {
	MouseState mouseState(20, 20, MouseState::PRESSED);

	EXPECT_CALL(*this, onMouseDown(&manager, MouseEvent(mouseState)));
	manager.mouseLeftDown(20, 20);
}

TEST_F(EngineMouseListener, OnLeftUp) {
	MouseState mouseState(30, 30, MouseState::RELESEAD);

	EXPECT_CALL(*this, onMouseUp(&manager, MouseEvent(mouseState)));
	manager.mouseLeftUp(30, 30);
}

TEST_F(EngineMouseListener, OnLeftDoubleClick) {
	MouseState mouseState(40, 40, MouseState::DOUBLE_PRESSED);

	EXPECT_CALL(*this, onDoubleClick(&manager, MouseEvent(mouseState)));
	manager.mouseLeftDoubleClick(40, 40);
}

TEST_F(EngineMouseListener, onRightDown) {
	MouseState mouseState(50, 50, MouseState::RELESEAD, MouseState::PRESSED);

	EXPECT_CALL(*this, onMouseDown(&manager, MouseEvent(mouseState)));
	manager.mouseRightDown(50, 50);
}

TEST_F(EngineMouseListener, onRightUp) {
	MouseState mouseState(60, 60, MouseState::RELESEAD, MouseState::RELESEAD);

	EXPECT_CALL(*this, onMouseUp(&manager, MouseEvent(mouseState)));
	manager.mouseRightUp(60, 60);
}

TEST_F(EngineMouseListener, OnRightDoubleClick) {
	MouseState mouseState(70, 70, MouseState::RELESEAD, MouseState::DOUBLE_PRESSED);

	EXPECT_CALL(*this, onDoubleClick(&manager, MouseEvent(mouseState)));
	manager.mouseRightDoubleClick(70, 70);
}
