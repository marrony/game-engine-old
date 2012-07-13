/*
 * MemoryManagerInternal.h
 *
 *  Created on: 11/03/2012
 *      Author: Marrony
 */

#ifndef MEMORYMANAGERINTERNAL_H_
#define MEMORYMANAGERINTERNAL_H_

#ifndef NDEBUG

#include <new>
#include <cstddef>

#include "Exports.h"
#include "Module.h"

EXPIMP void* allocMemory(Module* dynLib, std::size_t nbytes, const char* fileName, int line, bool isArray);
EXPIMP void freeMemory(Module* dynLib, void* ptr, bool isArray);

inline void* operator new(std::size_t nbytes, const char* fileName, int line) {
	return allocMemory(getModule(), nbytes, fileName, line, false);
}

inline  void* operator new[](std::size_t nbytes, const char* fileName, int line) {
	return allocMemory(getModule(), nbytes, fileName, line, true);
}

inline  void operator delete(void* ptr, const char* fileName, int line) {
	freeMemory(getModule(), ptr, false);
}

inline  void operator delete[](void* ptr, const char* fileName, int line) {
	freeMemory(getModule(), ptr, true);
}


#endif /* NDEBUG */

#endif /* MEMORYMANAGERINTERNAL_H_ */
