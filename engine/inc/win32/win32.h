/*
 * opengl.h
 *
 *  Created on: 04/02/2011
 *      Author: marrony
 */

#ifndef WIN32_H_
#define WIN32_H_

#ifdef _WIN32

#ifndef WINDOWS_LEAN_AND_MEAN
#define WINDOWS_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <GL/wglew.h>
#include <windows.h>
#include <mmsystem.h>

#endif /* _WIN32 */

#endif /* WIN32_H_ */
