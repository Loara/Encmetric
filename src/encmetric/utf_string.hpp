#ifndef ADV_MTYPES_T
#define	ADV_MTYPES_T

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
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <string>
#include <encmetric/chite.hpp>
#include <encmetric/basic_ptr.hpp>

namespace adv{
template<typename T>
void deduce_lens(const_tchar_pt<T>, size_t &len, size_t &siz);
template<typename T>
void deduce_lens(const_tchar_pt<T>, size_t rsiz, bool zero, size_t &len, size_t &siz);

template<typename T, typename U>
class adv_string; //forward declaration

/*
    Select only the second template parameter.
    Needed for SFINAE constructors

template<typename A, typename B>
struct second{
	using type = B;
};
template<typename A, typename B>
using second_t = typename second<A, B>::type;
*/
template<typename T>
class adv_string_view{
	private:
		size_t len;//character number
		size_t siz;//bytes number
		const_tchar_pt<T> ptr;
	protected:
		explicit adv_string_view(size_t length, size_t size, const_tchar_pt<T> bin) noexcept : ptr{bin}, len{length}, siz{size} {}
	public:
		explicit adv_string_view(const_tchar_pt<T>);
		explicit adv_string_view(const_tchar_pt<T>, size_t dim, bool dim_is_size);//true=size, false=length
		/*
		    read exactly len characters and siz bytes. If these values doesn't match trow error
		*/
		explicit adv_string_view(const_tchar_pt<T>, size_t siz, size_t len);
		/*
		    not-WIDENC costructors
		*/
		template<typename U, enable_not_widenc_t<T, int, U> = 0>
		explicit adv_string_view(const U *b) : adv_string_view{const_tchar_pt<T>{b}} {}
		template<typename U, enable_not_widenc_t<T, int, U> = 0>
		explicit adv_string_view(const U *b, size_t dim, bool dim_is_size) : adv_string_view{const_tchar_pt<T>{b}, dim, dim_is_size} {}
		template<typename U, enable_not_widenc_t<T, int, U> = 0>
		explicit adv_string_view(const U *b, size_t siz, size_t len) : adv_string_view{const_tchar_pt<T>{b}, siz, len} {}
		/*
		    WIDENC costructors
		*/
		template<typename U, enable_widenc_t<T, int, U> = 0>
		explicit adv_string_view(const U *b, const EncMetric &f) : adv_string_view{const_tchar_pt<T>{b, f}} {}
		template<typename U, enable_widenc_t<T, int, U> = 0>
		explicit adv_string_view(const U *b, size_t dim, bool dim_is_size, const EncMetric &f) : adv_string_view{const_tchar_pt<T>{b, f}, dim, dim_is_size} {}
		template<typename U, enable_widenc_t<T, int, U> = 0>
		explicit adv_string_view(const U *b, size_t siz, size_t len, const EncMetric &f) : adv_string_view{const_tchar_pt<T>{b, f}, siz, len} {}

		virtual ~adv_string_view() {}
		/*
		    Verify the string is correctly encoded
		*/
		void verify() const;
		bool verify_safe() const noexcept;
		
		adv_string_view<T> substring(size_t b, size_t e, bool endstr) const;
		adv_string_view<T> substring(size_t b, size_t e) const {return substring(b, e, false);}
		adv_string_view<T> substring(size_t b) const {return substring(b, 0, true);}
		size_t length() const noexcept {return len;}
		size_t size() const noexcept {return siz;}
		size_t size(size_t a, size_t n) const;//bytes of first n character starting from the (a+1)-st character
		size_t size(size_t n) const {return size(0, n);}

		template<typename S>
		bool compstr(const adv_string_view<S> &, size_t n) const;//compare only the first n character

		template<typename S>
		bool operator==(const adv_string_view<S> &) const;

		template<typename S>
		bool operator==(const_tchar_pt<S>) const;

		template<typename S>
		bool operator!=(const adv_string_view<S> &bin) const {return !(*this == bin);}

		template<typename S>
		bool operator!=(const_tchar_pt<S> bin) const {return !(*this == bin);}

		/*
			Note: id found is false then can return anything
		*/
		template<typename S>
		size_t bytesOf(const adv_string_view<S> &, bool &found) const;

		template<typename S>
		size_t indexOf(const adv_string_view<S> &, bool &found) const;

		template<typename S>
		bool containsChar(const_tchar_pt<S>) const;

		const byte *data() const noexcept {return ptr.data();}
		const char *raw() const noexcept {return (const char *)(ptr.data());}
		std::string toString() const noexcept {return std::string{(const char *)(ptr.data()), siz};}
		const_tchar_pt<T> begin() const noexcept {return ptr;}
		const_tchar_pt<T> end() const noexcept {return ptr.new_instance(ptr.data()+siz);}

		template<typename S>
		adv_string_view<S> basic_encoding_conversion(tchar_pt<S> buffer, size_t blen) const;

		template<typename S, typename U = std::allocator<byte>>
		adv_string<S, U> basic_encoding_conversion(const EncMetric &, const U & = U{}, enable_widenc_t<S, int> =0) const;
		template<typename S, typename U = std::allocator<byte>>
		adv_string<S, U> basic_encoding_conversion(const U & = U{}, enable_not_widenc_t<S, int> =0) const;

		template<typename S, typename U = std::allocator<byte>>
		adv_string<T, U> concatenate(const adv_string_view<S> &, const U & = U{}) const;

	template<typename W, typename S>
	friend adv_string_view<W> convert(const adv_string_view<S> &);
};

template<typename T, typename S>
bool sameEnc(const adv_string_view<T> &a, const adv_string_view<S> &b) noexcept{
	return sameEnc(a.begin(), b.begin());
}

template<typename S, typename T>
adv_string_view<S> convert(const adv_string_view<T> &ret){
	return adv_string_view<S>{convert<S, T>(ret.begin()), ret.length(), ret.size()};
}

template<typename T>
std::ostream &operator<<(std::ostream &stream, const adv_string_view<T> &string){
	return stream.write((const char *)(string.data()), string.size());
}

template<typename T, typename U = std::allocator<byte>>
class adv_string : public adv_string_view<T>{
	private:
		basic_ptr<byte, U> bind;

		adv_string(const_tchar_pt<T>, size_t, size_t, basic_ptr<byte, U>);
		adv_string(const_tchar_pt<T>, size_t, size_t, basic_ptr<byte, U>, int ignore);
	public:
		adv_string(const adv_string_view<T> &, const U & = U{});
		adv_string(const adv_string<T> &me) : adv_string{static_cast<const adv_string_view<T> &>(me), me.get_allocator()} {}
		adv_string(adv_string &&st) noexcept =default;

		U get_allocator() const noexcept{return bind.get_allocator();}
		std::size_t capacity() const noexcept{ return bind.dimension;}
		static adv_string<T, U> newinstance(const_tchar_pt<T>, const U & = U{});
	template<typename S>
	friend class adv_string_view;
};

template<typename S, typename T, typename U = std::allocator<byte>>
adv_string<S, U> operator+(const adv_string<S, U> &a, const adv_string<T, U> &b){
	return a.template concatenate<T, U>(b);
}

using wstr_view = adv_string_view<WIDENC>;

template<typename U = std::allocator<byte>>
using wstr_d = adv_string<WIDENC, U>;

using wstr = wstr_d<>;


#include <encmetric/utf_string.tpp>
}
#endif


