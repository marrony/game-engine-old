/*
 * WindowsGraphiManager.cpp
 *
 *  Created on: 20/12/2011
 *      Author: marrony.neris
 */

#include "linux/LinuxGraphicManager.h"
#include "linux/LinuxWindow.h"

namespace engine {

	LinuxGraphicManager::LinuxGraphicManager(LinuxWindow* window) :
			GraphicManager(), window(window) {
		create();
	}

	LinuxGraphicManager::~LinuxGraphicManager() {
		destroy();
	}

	void LinuxGraphicManager::create() {
		GLXContext tempContext = glXCreateContext(window->getDisplay(), window->getVisualInfo(), NULL, GL_TRUE);
		glXMakeCurrent(window->getDisplay(), window->getHandle(), tempContext);

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
		context = tempContext;
//			}

//			engine->resize(width, height);
	}

	void LinuxGraphicManager::destroy() {
		glXMakeCurrent(window->getDisplay(), window->getHandle(), context);
		glXDestroyContext(window->getDisplay(), context);
	}

	void LinuxGraphicManager::swapBuffers() {
		glXSwapBuffers(window->getDisplay(), window->getHandle());
	}

} /* namespace engine */
