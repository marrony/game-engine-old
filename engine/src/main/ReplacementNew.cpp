/*
 * ReplacementNew.cpp
 *
 *  Created on: 25/04/2012
 *      Author: objective
 */

#include "ReplacementNew.h"
#include "Module.h"
#include "MemoryManagerInternal.h"

void * operator new(std::size_t nbytes) THROW(std::bad_alloc) {
	void* ptr = allocMemory(getModule(), nbytes, "<<unknow>>", 0, false);

	if(!ptr) throw std::bad_alloc();

	return ptr;
}

void* operator new[](std::size_t nbytes) THROW(std::bad_alloc) {
	void* ptr = allocMemory(getModule(), nbytes, "<<unknow>>", 0, true);

	if(!ptr) throw std::bad_alloc();

	return ptr;
}

void operator delete(void* ptr) NO_THROW {
	freeMemory(getModule(), ptr, false);
}

void operator delete[](void* ptr) NO_THROW {
	freeMemory(getModule(), ptr, true);
}

void* operator new(std::size_t nbytes, const std::nothrow_t&) NO_THROW {
	return allocMemory(getModule(), nbytes, "<<unknow>>", 0, false);
}

void* operator new[](std::size_t nbytes, const std::nothrow_t&) NO_THROW {
	return allocMemory(getModule(), nbytes, "<<unknow>>", 0, true);
}

void operator delete(void* ptr, const std::nothrow_t&) NO_THROW {
	freeMemory(getModule(), ptr, false);
}

void operator delete[](void* ptr, const std::nothrow_t&) NO_THROW {
	freeMemory(getModule(), ptr, true);
}
