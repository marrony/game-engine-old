/*
 * Win32GraphiManager.h
 *
 *  Created on: 20/12/2011
 *      Author: marrony.neris
 */

#ifndef WIN32GRAPHIMANAGER_H_
#define WIN32GRAPHIMANAGER_H_

#include "GraphicManager.h"

namespace engine {

	class Win32Window;

	class Win32GraphicManager : public GraphicManager {
	public:
		Win32GraphicManager(Win32Window* window);
		virtual ~Win32GraphicManager();

		virtual void swapBuffers();
	private:
		void create();
		void destroy();

		Win32Window* window;
		HDC hdc;
		HGLRC hglrc;
	};

} /* namespace engine */

#endif /* WIN32GRAPHIMANAGER_H_ */
