/*
 * android_main.cpp
 *
 *  Created on: 29/04/2012
 *      Author: Marrony
 */
#include "android/AndroidApplication.h"
#include "android/AndroidGraphicManager.h"
#include "MyGame.h"

#include "MemoryManager.h"

extern "C" Module* getModule() {
	return 0;
}

using namespace engine;

void android_main(struct android_app* state) {
	AndroidApplication app(state);
	MyGame game;

	app_dummy();

	app.run(game);
}
