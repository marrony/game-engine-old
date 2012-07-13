/*
 * LinuxWindow.cpp
 *
 *  Created on: 22/02/2012
 *      Author: marrony
 */

#include "linux/LinuxWindow.h"

namespace engine {

	LinuxWindow::LinuxWindow(int width, int height) :
			dpy(0), win(0), vi(0), width(width), height(height) {
	}

	LinuxWindow::~LinuxWindow() {
		destroyWindow();
	}

	void LinuxWindow::setTitle(const std::string& title) {
//	if (!hwnd) {
//		return;
//	}
//
//	SetWindowText(hwnd, title.c_str());
	}

	void LinuxWindow::resize(int newWidth, int newHeight) {
//	if (!hwnd) {
//		return;
//	}
//
//	width = newWidth;
//	height = newHeight;
//	SetWindowPos(hwnd, 0, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
	}

	void LinuxWindow::show() {
		if(!win) {
			createWindow();
		}

		resize(width, height);
//
//		ShowWindow(hwnd, SW_SHOW);
//		UpdateWindow (hwnd);
	}

	void LinuxWindow::hide() {
//	if (!hwnd) {
//		return;
//	}
//
//	ShowWindow(hwnd, SW_HIDE);
//	UpdateWindow(hwnd);
	}

	bool LinuxWindow::processMessages() {
		XEvent xev;
		XNextEvent(dpy, &xev);
		return true;
	}

	void LinuxWindow::createWindow() {
		::Window root;
		int att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
		Colormap cmap;
		XSetWindowAttributes swa;
		GLXContext glc;
		XWindowAttributes gwa;
		XEvent xev;

		dpy = XOpenDisplay(NULL);

		if(dpy == NULL) {
			printf("\n\tcannot connect to X server\n\n");
			exit(0);
		}

		root = DefaultRootWindow(dpy) ;

		vi = glXChooseVisual(dpy, 0, att);

		if(vi == NULL) {
			printf("\n\tno appropriate visual found\n\n");
			exit(0);
		} else {
			printf("\n\tvisual %p selected\n", (void *) vi->visualid);
		}/* %p creates hexadecimal output like in glxinfo */

		cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

		swa.colormap = cmap;
		swa.event_mask = ExposureMask | KeyPressMask;

		win = XCreateWindow(dpy, root, 0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
				CWColormap | CWEventMask, &swa);

		XMapWindow(dpy, win);
		XStoreName(dpy, win, "VERY SIMPLE APPLICATION");
	}

	void LinuxWindow::destroyWindow() {
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}

} /* namespace engine */
