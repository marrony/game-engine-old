#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "MouseManager.h"

using namespace engine;

class EngineMouseListener : public testing::Test {
public:
	MOCK_METHOD1(onMouseMove, void(MouseEvent));
	MOCK_METHOD1(onMouseUp, void(MouseEvent));
	MOCK_METHOD1(onMouseDown, void(MouseEvent));
	MOCK_METHOD1(onDoubleClick, void(MouseEvent));

	virtual void SetUp() {
		manager.onMouseMove += [&](MouseEvent event) {
			onMouseMove(event);
		};

		manager.onMouseUp += [&](MouseEvent event) {
			onMouseUp(event);
		};

		manager.onMouseDown += [&](MouseEvent event) {
			onMouseDown(event);
		};

		manager.onDoubleClick += [&](MouseEvent event) {
			onDoubleClick(event);
		};
	}

	MouseManager manager;
};

TEST_F(EngineMouseListener, OnMove) {
	MouseState mouseState(10, 10);

	EXPECT_CALL(*this, onMouseMove(MouseEvent(mouseState)));
	manager.setMousePosition(10, 10);
}

TEST_F(EngineMouseListener, OnLeftDown) {
	MouseState mouseState(20, 20, MouseState::PRESSED);

	EXPECT_CALL(*this, onMouseDown(MouseEvent(mouseState)));
	manager.mouseLeftDown(20, 20);
}

TEST_F(EngineMouseListener, OnLeftUp) {
	MouseState mouseState(30, 30, MouseState::RELESEAD);

	EXPECT_CALL(*this, onMouseUp(MouseEvent(mouseState)));
	manager.mouseLeftUp(30, 30);
}

TEST_F(EngineMouseListener, OnLeftDoubleClick) {
	MouseState mouseState(40, 40, MouseState::DOUBLE_PRESSED);

	EXPECT_CALL(*this, onDoubleClick(MouseEvent(mouseState)));
	manager.mouseLeftDoubleClick(40, 40);
}

TEST_F(EngineMouseListener, onRightDown) {
	MouseState mouseState(50, 50, MouseState::RELESEAD, MouseState::PRESSED);

	EXPECT_CALL(*this, onMouseDown(MouseEvent(mouseState)));
	manager.mouseRightDown(50, 50);
}

TEST_F(EngineMouseListener, onRightUp) {
	MouseState mouseState(60, 60, MouseState::RELESEAD, MouseState::RELESEAD);

	EXPECT_CALL(*this, onMouseUp(MouseEvent(mouseState)));
	manager.mouseRightUp(60, 60);
}

TEST_F(EngineMouseListener, OnRightDoubleClick) {
	MouseState mouseState(70, 70, MouseState::RELESEAD, MouseState::DOUBLE_PRESSED);

	EXPECT_CALL(*this, onDoubleClick(MouseEvent(mouseState)));
	manager.mouseRightDoubleClick(70, 70);
}
