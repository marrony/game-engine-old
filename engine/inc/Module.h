/*
 * Module.h
 *
 *  Created on: 20/04/2012
 *      Author: marrony.neris
 */

#ifndef MODULE_H_
#define MODULE_H_

struct Module {
	virtual ~Module() { }
	virtual bool getName(char* name, int maxLenght) const = 0;
	virtual void* getProcAddress(const char* procName) = 0;
};

extern "C" Module* getModule();

#endif /* MODULE_H_ */
