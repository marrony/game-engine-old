/*
 * ReplacementNew.h
 *
 *  Created on: 25/04/2012
 *      Author: objective
 */

#ifndef REPLACEMENTNEW_H_
#define REPLACEMENTNEW_H_

#include <new>
#include <cstddef>

extern "C++" {

#ifdef _GLIBCXX_THROW
#define THROW(x) _GLIBCXX_THROW(x)
#else
#define THROW(x) throw(x)
#endif

#ifdef _GLIBCXX_USE_NOEXCEPT
#define NO_THROW _GLIBCXX_USE_NOEXCEPT
#else
#define NO_THROW throw()
#endif

void* operator new(std::size_t) THROW(std::bad_alloc);
void* operator new[](std::size_t) THROW(std::bad_alloc);

void operator delete(void*) NO_THROW;
void operator delete[](void*) NO_THROW;

void* operator new(std::size_t, const std::nothrow_t&) NO_THROW;
void* operator new[](std::size_t, const std::nothrow_t&) NO_THROW;

void operator delete(void*, const std::nothrow_t&) NO_THROW;
void operator delete[](void*, const std::nothrow_t&) NO_THROW;

}

#endif /* REPLACEMENTNEW_H_ */
