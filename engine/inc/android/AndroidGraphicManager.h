/*
 * AndroidGraphicManager.h
 *
 *  Created on: 19/12/2011
 *      Author: marrony.neris
 */

#ifndef ANDROIDGRAPHICMANAGER_H_
#define ANDROIDGRAPHICMANAGER_H_

#include "GraphicManager.h"

namespace engine {

	class Window;

	class AndroidGraphicManager : public GraphicManager {
	public:
		AndroidGraphicManager();
		virtual ~AndroidGraphicManager();

		virtual void swapBuffers();
	};

} /* namespace engine */
#endif /* ANDROIDGRAPHICMANAGER_H_ */
