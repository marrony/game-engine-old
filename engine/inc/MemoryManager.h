/*
 * MemoryManager.h
 *
 *  Created on: 11/03/2012
 *      Author: Marrony
 */

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

#ifndef NDEBUG

#include "MemoryManagerInternal.h"

#define new new(__FILE__, __LINE__)

#endif /* NDEBUG */

#endif /* MEMORYMANAGER_H_ */
