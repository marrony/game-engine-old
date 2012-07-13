/*
 * ResourcePlugin.h
 *
 *  Created on: 28/03/2012
 *      Author: marrony.neris
 */

#ifndef RESOURCEPLUGIN_H_
#define RESOURCEPLUGIN_H_

#include "Exports.h"
#include "Module.h"

struct Plugin {
	virtual ~Plugin() { }
	virtual void release() = 0;
};

extern "C" EXPIMP Plugin* initializePlugin(Module* dynLib);

#endif /* RESOURCEPLUGIN_H_ */
