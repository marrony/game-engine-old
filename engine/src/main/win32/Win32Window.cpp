/*
 * Win32Window.cpp
 *
 *  Created on: 22/02/2012
 *      Author: marrony
 */

#include "win32/Win32Window.h"

namespace engine {

	Win32Window::Win32Window(int width, int height) :
			hwnd(0), width(width), height(height) {
	}

	Win32Window::~Win32Window() {
		destroyWindow();
	}

	void Win32Window::setTitle(const std::string& title) {
		if(!hwnd) {
			return;
		}

		SetWindowText(hwnd, title.c_str());
	}

	void Win32Window::resize(int newWidth, int newHeight) {
		if(!hwnd) {
			return;
		}

		width = newWidth;
		height = newHeight;
		SetWindowPos(hwnd, 0, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
	}

	void Win32Window::show() {
		if(!hwnd) {
			createWindow();
		}

		resize(width, height);

		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
	}

	void Win32Window::hide() {
		if(!hwnd) {
			return;
		}

		ShowWindow(hwnd, SW_HIDE);
		UpdateWindow(hwnd);
	}

	bool Win32Window::processMessages() {
		MSG msg;

		while(PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE) != 0) {
			if(msg.message == WM_QUIT) {
				return false;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return true;
	}

	HWND Win32Window::getHandle() {
		if(!hwnd) {
			createWindow();
		}

		return hwnd;
	}

	void Win32Window::registerClass() {
		WNDCLASSEX wcx;
		wcx.cbSize = sizeof(wcx);
		wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcx.lpfnWndProc = DefWindowProc;
		wcx.cbClsExtra = 0;
		wcx.cbWndExtra = sizeof(Win32Window*);
		wcx.hInstance = GetModuleHandle(NULL);
		wcx.hIcon = NULL;
		wcx.hCursor = NULL;
		wcx.hbrBackground = NULL;
		wcx.lpszMenuName = NULL;
		wcx.lpszClassName = "WindowClass";
		wcx.hIconSm = NULL;

		RegisterClassEx(&wcx);
	}

	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		Win32Window* window = (Win32Window*) GetWindowLongPtr(hwnd, 0);
		return window->wndProc(msg, wParam, lParam);
	}

	LRESULT Win32Window::wndProc(UINT msg, WPARAM wParam, LPARAM lParam) {
		if(msg == WM_CLOSE) {
			PostQuitMessage(0);
		}

		switch(msg) {
		case WM_SIZE:
			onResize(this, ResizeEvent(LOWORD(lParam), HIWORD(lParam)));
			break;

		case WM_KEYDOWN:
			onKeyDown(this, KeyEvent(wParam));
			break;

		case WM_KEYUP:
			onKeyUp(this, KeyEvent(wParam));
			break;

		case WM_MOUSEMOVE:
			POINTS pt = MAKEPOINTS(lParam);
			break;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	void Win32Window::createWindow() {
		registerClass();

		hwnd = CreateWindowEx(0,
				"WindowClass",
		      "",
		      WS_OVERLAPPEDWINDOW,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      HWND_DESKTOP,
		      (HMENU) NULL,
		      GetModuleHandle(NULL),
		      (LPVOID)NULL);

		SetWindowLongPtr(hwnd, 0, (LONG_PTR)this);
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)MainWndProc);
	}

	void Win32Window::destroyWindow() {
		DestroyWindow(hwnd);
	}

} /* namespace engine */
