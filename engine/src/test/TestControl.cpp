/*
 * TestControl.cpp
 *
 *  Created on: 06/03/2012
 *      Author: marrony.neris
 */


#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "gui/Control.h"
#include "TypedEvent.h"

using namespace testing;

using namespace engine;
using namespace engine::gui;

class MockListener {
public:
	MOCK_METHOD1(onResize, void(ResizeEvent));
	MOCK_METHOD1(onPressed, void(MouseEvent));
	MOCK_METHOD1(onReleased, void(MouseEvent));
};

class MockKeyboardManager : public KeyboardManager {
};

class MockMouseManager : public MouseManager {
public:
	MOCK_METHOD0(getMouseState, MouseState(void));
};

class ControlTest : public Test {
protected:

	virtual void SetUp() {
		Rect rect(0, 0, 100, 100);

		ctrl = new Control(rect, keyboardManager, mouseManager);
	}

	virtual void TearDown() {
		delete ctrl;
	}

	MockKeyboardManager keyboardManager;
	MockMouseManager mouseManager;
	Control* ctrl;
};

TEST_F(ControlTest, OnResize) {
	MockListener mockListener;

	ctrl->onResize += [&](ResizeEvent event) {
		mockListener.onResize(event);
	};

	EXPECT_CALL(mockListener, onResize(ResizeEvent()));

	ctrl->resize(Size(10, 10));
}

TEST_F(ControlTest, OnPressed) {
	MockListener mockListener;

	ctrl->onPressed += [&](MouseEvent event) {
		mockListener.onPressed(event);
	};

	ON_CALL(mouseManager, getMouseState()).WillByDefault(Return(MouseState(10, 10, MouseState::PRESSED)));
	EXPECT_CALL(mouseManager, getMouseState());
	EXPECT_CALL(mockListener, onPressed(MouseEvent(MouseState(10, 10, MouseState::PRESSED))));

	ctrl->update();
}

TEST_F(ControlTest, TwoUpdadesPressedDontGenerateMoreThanOnePressedEvent) {
	MockListener mockListener;

	ctrl->onPressed += [&](MouseEvent event) {
		mockListener.onPressed(event);
	};

	ON_CALL(mouseManager, getMouseState()).WillByDefault(Return(MouseState(10, 10, MouseState::PRESSED)));
	EXPECT_CALL(mouseManager, getMouseState());
	EXPECT_CALL(mockListener, onPressed(MouseEvent(MouseState(10, 10, MouseState::PRESSED))));

	ctrl->update();

	ON_CALL(mouseManager, getMouseState()).WillByDefault(Return(MouseState(10, 10, MouseState::PRESSED)));
	EXPECT_CALL(mouseManager, getMouseState());

	ctrl->update();
}

TEST_F(ControlTest, PressAndRelease) {
	MockListener mockListener;

	ctrl->onReleased += [&](MouseEvent event) {
		mockListener.onReleased(event);
	};

	ON_CALL(mouseManager, getMouseState()).WillByDefault(Return(MouseState(10, 10, MouseState::PRESSED)));
	EXPECT_CALL(mouseManager, getMouseState());

	ctrl->update();

	ON_CALL(mouseManager, getMouseState()).WillByDefault(Return(MouseState(10, 10, MouseState::RELESEAD)));
	EXPECT_CALL(mouseManager, getMouseState());
	EXPECT_CALL(mockListener, onReleased(MouseEvent(MouseState(10, 10, MouseState::RELESEAD))));

	ctrl->update();
}

TEST_F(ControlTest, Position) {
	ASSERT_EQ(Position(0, 0), ctrl->getPosition());
}

TEST_F(ControlTest, AbsolutePosition) {
	ASSERT_EQ(Position(0, 0), ctrl->getAbsolutePosition());
}

TEST_F(ControlTest, AbsolutePositionWithParent) {
	Rect rect(10, 10, 100, 100);
	Control parent(rect, keyboardManager, mouseManager);

	ctrl->setParent(&parent);

	ASSERT_EQ(Position(10, 10), ctrl->getAbsolutePosition());
}

