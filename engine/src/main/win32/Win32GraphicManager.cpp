/*
 * WindowsGraphiManager.cpp
 *
 *  Created on: 20/12/2011
 *      Author: marrony.neris
 */

#include "win32/Win32GraphicManager.h"
#include "win32/Win32Window.h"

namespace engine {

	Win32GraphicManager::Win32GraphicManager(Win32Window* window) :
			GraphicManager(), window(window) {
		create();
	}

	Win32GraphicManager::~Win32GraphicManager() {
		destroy();
	}

	void Win32GraphicManager::create() {
		PIXELFORMATDESCRIPTOR pfd = {0};

		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.cDepthBits = 32;
		pfd.cAccumBits = 32;
		pfd.cStencilBits = 32;
		pfd.cAlphaBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;

		hdc = GetWindowDC(window->getHandle());

		int nPixelFormat = ChoosePixelFormat(hdc, &pfd);
		BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd);

		HGLRC tempContext = wglCreateContext(hdc);
		wglMakeCurrent(hdc, tempContext);

		GLenum err = glewInit();

		if(GLEW_OK != err) {
			//				return 0;
		}

//			if(wglewIsSupported("WGL_ARB_create_context")) {
//				int attribs[] = {WGL_CONTEXT_MAJOR_VERSION_ARB, 3, WGL_CONTEXT_MINOR_VERSION_ARB, 3, WGL_CONTEXT_FLAGS_ARB,
//				      WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB, 0};
//				hglrc = wglCreateContextAttribsARB(hdc, 0, attribs);
//				wglMakeCurrent(NULL, NULL);
//				wglDeleteContext(tempContext);
//				wglMakeCurrent(hdc, hglrc);
//
//				//	int OpenGLVersion[2];
//				//	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
//				//	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
//			} else {
			hglrc = tempContext;
//			}

//			engine->resize(width, height);

		wglSwapIntervalEXT(0);
	}

	void Win32GraphicManager::destroy() {
		wglDeleteContext(hglrc);
		ReleaseDC(window->getHandle(), hdc);
	}

	void Win32GraphicManager::swapBuffers() {
//			SwapBuffers(wglGetCurrentDC());
		wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
	}


} /* namespace engine */
