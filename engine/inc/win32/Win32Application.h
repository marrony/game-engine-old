/*
 * Win32Application.h
 *
 *  Created on: 27/04/2012
 *      Author: objective
 */

#ifndef WIN32APPLICATION_H_
#define WIN32APPLICATION_H_

#include "Application.h"

namespace engine {

	class Win32Application : public Application {
	public:
		Win32Application();
		virtual ~Win32Application();

		virtual void createApplication();
		virtual void destroyApplication();

	protected:
		virtual bool processMessages();

		class Win32Window* window;
	};

} /* namespace engine */

#endif /* WIN32APPLICATION_H_ */
