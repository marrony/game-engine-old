/*
 * LinuxWindow.h
 *
 *  Created on: 22/02/2012
 *      Author: marrony
 */

#ifndef LINUXWINDOW_H_
#define LINUXWINDOW_H_

#include "Window.h"
#include "System.h"

namespace engine {

	class LinuxWindow : public Window {
	public:
		LinuxWindow(int width, int height);
		virtual ~LinuxWindow();

		virtual void setTitle(const std::string& title);
		virtual void resize(int newWidth, int newHeight);
		virtual void show();
		virtual void hide();

		virtual bool processMessages();

		Display* getDisplay() {
			return dpy;
		}

		::Window getHandle() {
			return win;
		}

		XVisualInfo* getVisualInfo() {
			return vi;
		}
	protected:
		void createWindow();
		void destroyWindow();
	private:
		Display *dpy;
		::Window win;
		XVisualInfo *vi;
		int width;
		int height;
	};

} /* namespace engine */

#endif /* LINUXWINDOW_H_ */
