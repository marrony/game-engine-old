/*
 * Win32Window.h
 *
 *  Created on: 22/02/2012
 *      Author: marrony
 */

#ifndef WIN32WINDOW_H_
#define WIN32WINDOW_H_

#include "Window.h"
#include "System.h"

namespace engine {

	class Win32Window : public Window {
	public:
		Win32Window(int width, int height);
		virtual ~Win32Window();

		virtual void setTitle(const std::string& title);
		virtual void resize(int newWidth, int newHeight);
		virtual void show();
		virtual void hide();

		virtual bool processMessages();

		HWND getHandle();

		LRESULT wndProc(UINT msg, WPARAM wParam, LPARAM lParam);
	protected:
		void registerClass();
		void createWindow();
		void destroyWindow();
	private:
		HWND hwnd;
		int width;
		int height;
	};

} /* namespace engine */
#endif /* WIN32WINDOW_H_ */
