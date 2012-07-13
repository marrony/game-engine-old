/*
 * LinuxGraphiManager.h
 *
 *  Created on: 20/12/2011
 *      Author: marrony.neris
 */

#ifndef LINUXGRAPHIMANAGER_H_
#define LINUXGRAPHIMANAGER_H_

#include "GraphicManager.h"
#include "System.h"

namespace engine {

	class LinuxWindow;

	class LinuxGraphicManager : public GraphicManager {
	public:
		LinuxGraphicManager(LinuxWindow* window);
		virtual ~LinuxGraphicManager();

		virtual void swapBuffers();
	private:
		void create();
		void destroy();

		LinuxWindow* window;
		GLXContext context;
	};

} /* namespace engine */

#endif /* LINUXGRAPHIMANAGER_H_ */
