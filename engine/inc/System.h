/*
 * System.h
 *
 *  Created on: 19/12/2011
 *      Author: marrony.neris
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#define GLEW_STATIC

#include <GL/glew.h>

#ifdef _WIN32
#include "win32/win32.h"
#else
#include "linux/linux.h"
#endif // _WIN32

#include <stdio.h>
#include <stddef.h>

#ifdef ANDROID
#include <errno.h>

#include <EGL/egl.h>
//#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"native-activity", __VA_ARGS__))

#endif // ANDROID

#include "Exception.h"

#define CHECK_ERROR() //checkError(__FILE__, __LINE__)

static inline void checkError(const char* file, int line) {
	GLenum error = glGetError();
	if(error != GL_NO_ERROR) {
		char buffer[256];
		sprintf(buffer, "%s(%d): erro %d 0x%x", file, line, error, error);
		throw engine::Exception(buffer);
	}
}

#endif /* SYSTEM_H_ */
