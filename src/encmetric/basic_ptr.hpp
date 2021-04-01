#pragma once
/*
    This file is part of Encmetric.
    Copyright (C) 2021 Paolo De Donato.

    Encmetric is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Encmetric is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Encmetric. If not, see <http://www.gnu.org/licenses/>.
*/
#include <new>
#include <memory>

namespace adv{

/*
    A basic unique_ptr that uses Allocator
*/
template<typename T, typename U>
class basic_ptr{
	private:
		void reset() noexcept{
			memory = nullptr;
			dimension = 0;
		}
		U alloc;
	public:
		T *memory;
		std::size_t dimension;
		basic_ptr(const U &all = U{}) : alloc{all}, memory{nullptr}, dimension{0} {}
		explicit basic_ptr(std::size_t dim, const U &all = U{}) : alloc{all}, memory{nullptr}, dimension{0} {
			if(dim>0){
				dimension = dim;
				memory = std::allocator_traits<U>::allocate(alloc, dim);
			}
		}
		explicit basic_ptr(const T *pt, std::size_t dim, const U &all = U{}) : basic_ptr{dim, all} {
			if(dim > 0 && pt != nullptr)
				std::memcpy(memory, pt, dim * sizeof(T));
		}
		basic_ptr(const basic_ptr<T, U> &) = delete;
		basic_ptr(basic_ptr<T, U> &&from) noexcept : memory{std::move(from.memory)}, dimension{from.dimension}, alloc{std::move(from.alloc)}
		{
			from.reset();
		}
		void free(){
			if(memory != nullptr){
				std::allocator_traits<U>::deallocate(alloc, memory, dimension);
				memory = nullptr;
			}
			dimension = 0;
		}
		~basic_ptr(){
			try{
				free();
			}
			catch(...){
				memory = nullptr;
			}
		}
		void swap(basic_ptr<T, U> &sw) noexcept{
			std::swap(memory, sw.memory);
			std::swap(dimension, sw.dimension);
			std::swap(alloc, sw.alloc);
		}
		basic_ptr<T, U> &operator=(basic_ptr<T, U> &&ref) noexcept{
			free();
			swap(ref);
			return *this;
		}
		basic_ptr<T, U> &operator=(const basic_ptr<T, U> &)=delete;

		void reallocate(std::size_t dim){
			T *newm = std::allocator_traits<U>::allocate(alloc, dim);
			int mindim = dim > dimension ? dimension : dim;
			if(memory != nullptr)
				std::memcpy(newm, memory, mindim * sizeof(T));
			free();
			dimension = dim;
			memory = newm;
		}
		void exp_fit(std::size_t fit){
			if(fit == 0)
				return;
			std::size_t grown = dimension == 0 ? 1 : dimension;
			while(grown < fit)
				grown *= 2;
			if(grown > dimension)
				reallocate(grown);
		}
		/*
			return ptr so that
			ptr - newmem = oldptr - oldmem
		*/
		T *exp_fit_and_transfer(std::size_t fit, const T *oldptr){
			std::ptrdiff_t ptdif = oldptr - memory;
			exp_fit(fit);
			return memory + ptdif;
		}
		T* leave() noexcept{
			T *ret = nullptr;
			std::swap(ret, memory);
			dimension = 0;
			return ret;
		}

		U get_allocator() const noexcept{
			return alloc;
		}
		basic_ptr<T, U> copy() const{
			return basic_ptr<T, U>{memory, dimension, alloc};
		}
};

/*
template<typename T, typename U>
basic_ptr<T, U> reallocate(const basic_ptr<T, U> &ptr, std::size_t newdim){
	basic_ptr<T, U> newinc{newdim, ptr.get_allocator()};
	std::size_t min = (newdim >= ptr.dimension) ? ptr.dimension : newdim;

	if(ptr.memory != nullptr)
		std::memcpy(newinc.memory, ptr.memory, min * sizeof(T));
	return newinc;
}

template<typename T, typename U>
basic_ptr<T, U> reallocate_expon(const basic_ptr<T, U> &ptr, std::size_t nd){
	std::size_t newdim = ptr.dimension > 0 ? ptr.dimension : 1;
	while(newdim < nd)
		newdim *= 2;
	basic_ptr<T, U> newinc{newdim, ptr.get_allocator()};
	
	if(ptr.memory != nullptr)
		std::memcpy(newinc.memory, ptr.memory, ptr.dimension * sizeof(T));
	return newinc;
}

template<typename T, typename U>
void append(basic_ptr<T, U> &mem, std::size_t pos, const T *from, std::size_t siz){
	if(mem.memory == nullptr)
		mem.init_a_copy(nullptr, 1);

	if(mem.dimension < pos + siz){
		mem = reallocate_expon(mem, pos+siz);
	}
	std::memcpy(mem.memory + pos, from, siz * sizeof(T));
}
*/

}
