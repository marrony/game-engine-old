/*
 * Allocator.h
 *
 *  Created on: 09/04/2012
 *      Author: marrony.neris
 */

#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

namespace engine {

	template<typename T>
	class Allocator {
	public:
		typedef T * pointer;
		typedef const T * const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		int x;

		Allocator(int x) : x(x) { }

		Allocator(const Allocator<T>& other) : x(other.x) { }

		template<typename U>
		Allocator(const Allocator<U>& other) : x(other.x) { }

		~Allocator() { }

		void construct(pointer ptr, const value_type& value, const char* file = __FILE__, int line = __LINE__) {
			new(ptr) T(value);
		}

		void destroy(pointer ptr, const char* file = __FILE__, int line = __LINE__) {
			ptr->~T();
		}

		pointer allocate(size_t n, const void* ptr = 0, const char* file = __FILE__, int line = __LINE__) {
			return new T();
		}

		void deallocate(pointer ptr, size_t n = 0, const char* file = __FILE__, int line = __LINE__) {
			delete ptr;
		}

		size_t max_size() const {
			return size_t(-1) / sizeof(T);
		}

		template<typename U>
		struct rebind {
			typedef Allocator<U> other;
		};

	private:
		void operator=(Allocator& other);
	};

}  // namespace engine


#endif /* ALLOCATOR_H_ */
