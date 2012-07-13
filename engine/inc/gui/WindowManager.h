/*
 * WindowManager.h
 *
 *  Created on: 04/02/2011
 *      Author: marrony.neris
 */

#ifndef WINDOWMANAGER_H_
#define WINDOWMANAGER_H_

#include <vector>

namespace engine {

	class GraphicManager;

	namespace gui {

		class Window;

		typedef std::vector<Window*> WindowList;

		class WindowManager {
			WindowList windows;
		public:
			void addWindow(Window* window);
			void removeWindow(Window* window);

			void render(GraphicManager* graphicManager);

			void keyDown(int ch);
			void keyUp(int ch);

			void mouseMove(int x, int y);

			void mouseRightDown();
			void mouseRightUp();
			void mouseRightDoubleClick();

			void mouseLeftDown();
			void mouseLefttUp();
			void mouseLeftDoubleClick();

			void mouseMiddleDown();
			void mouseMiddleUp();
			void mouseMiddleDoubleClick();
		};

	}  // namespace gui

}  // namespace engine

#endif /* WINDOWMANAGER_H_ */
