/*
 * LinuxApplication.h
 *
 *  Created on: 27/04/2012
 *      Author: objective
 */

#ifndef LINUXAPPLICATION_H_
#define LINUXAPPLICATION_H_

#include "Application.h"

namespace engine {

	class LinuxApplication : public Application {
	public:
		LinuxApplication();
		virtual ~LinuxApplication();

		virtual void createApplication();
		virtual void destroyApplication();

	protected:
		virtual bool processMessages();

		class LinuxWindow* window;
	};

} /* namespace engine */

#endif /* LINUXAPPLICATION_H_ */
