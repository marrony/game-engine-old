/*
 * main.cpp
 *
 *  Created on: 28/07/2010
 *      Author: marrony
 */
#ifdef WIN32
#include "win32/Win32Application.h"
typedef engine::Win32Application MyApplication;
#else
#include "linux/LinuxApplication.h"
typedef engine::LinuxApplication MyApplication;
#endif
#include "Exception.h"
#include "MyGame.h"

#include <iostream>

#include "MemoryManager.h"

extern "C" Module* getModule() {
	return 0;
}

using namespace engine;

int main(int argv, char* argc[]) {
	try {
		MyApplication app;
		MyGame game;

		app.run(game);
	} catch(const Exception& ex) {
		std::cout << ex.getMessage() << std::endl;
	} catch(...) {
		std::cout << "execeção desconhecida" << std::endl;
	}

	return 0;
}
