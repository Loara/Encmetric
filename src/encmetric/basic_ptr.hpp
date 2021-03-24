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
		//a can be nullptr: only allocates memory
		explicit basic_ptr(const T* a, std::size_t dim, const U &all = U{}) : memory{nullptr}, dimension{0}, alloc{all} {
			if(dim>0){
				dimension = dim;
				memory = std::allocator_traits<U>::allocate(alloc, dim);
				if(a != nullptr){
					for(std::size_t i=0; i<dim; i++){
						memory[i] = a[i];
					}
				}
			}
		}
		void init_a_copy(const T *a, std::size_t dim){
			if(memory == nullptr){
				dimension=dim;
				memory = std::allocator_traits<U>::allocate(alloc, dim);
				if(a != nullptr){
					for(std::size_t i=0; i<dim; i++){
						memory[i] = a[i];
					}
				}
			}
		}
		T* leave() noexcept{
			T *ret = nullptr;
			std::swap(ret, memory);
			dimension = 0;
			return ret;
		}
		basic_ptr(const basic_ptr<T, U> &) = delete;
		basic_ptr(basic_ptr<T, U> &&from) noexcept : memory{std::move(from.memory)}, dimension{from.dimension}, alloc{std::move(from.alloc)}
		{
			from.reset();
		}
		void swap(basic_ptr<T, U> &sw) noexcept{
			std::swap(memory, sw.memory);
			std::swap(dimension, sw.dimension);
			std::swap(alloc, sw.alloc);
		}
		basic_ptr<T, U> &operator=(basic_ptr<T, U> &&ref) noexcept{
			swap(ref);
			return *this;
		}
		basic_ptr<T, U> &operator=(const basic_ptr<T, U> &)=delete;

		U get_allocator() const noexcept{
			return alloc;
		}
		basic_ptr<T, U> copy() const{
			return basic_ptr<T, U>{memory, dimension, alloc};
		}
		void free(){
			if(memory != nullptr){
				std::allocator_traits<U>::deallocate(alloc, memory, dimension);
				memory = nullptr;
				dimension = 0;
			}
		}
		~basic_ptr(){
			try{
				free();
			}
			catch(...){
				memory = nullptr;
			}
		}
};

template<typename T, typename U = std::allocator<byte>>
basic_ptr<T, U> reallocate(const basic_ptr<T, U> &ptr, std::size_t newdim){
	basic_ptr<T, U> newinc{newdim, ptr.get_allocator()};
	std::size_t min = (newdim >= ptr.dimension) ? ptr.dimension : newdim;
	for(int i=0; i<min; i++)
		newinc.memory[i] = ptr.memory[i];
	return newinc;
}

}
