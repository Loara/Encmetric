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
#include <vector>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <string>
#include <encmetric/chite.hpp>
#include <encmetric/basic_ptr.hpp>

namespace adv{

template<typename T>
using terminate_func = std::function<bool(const byte *, const EncMetric_info<T> &)>;

template<typename T>
void deduce_lens(const_tchar_pt<T>, size_t &len, size_t &siz, const terminate_func<T> &);
template<typename T>
void deduce_lens(const_tchar_pt<T>, size_t rsiz, bool zero, size_t &len, size_t &siz);

enum class meas {size, length};

/*
 * Basic terminate function: string is terminated if and only if the encoded character is all 0 bytes
 */
template<typename T>
bool zero_terminating(const byte *data, const EncMetric_info<T> &format){
    uint size = format.unity();
	for(uint i=0; i<size; i++){
		if(data[i] != byte{0})
			return false;
	}
	return true;
}
/*
 * Another terminate function: terminate if and only if the encoded character is equivalent to 0
 *
 * WARNING: control up to st characters and may throw if the string is not correctly encoded
 */
template<typename T, size_t st = 100>
bool encoding_terminating(const byte *data, const EncMetric_info<T> &format){
    using ctype = typename T::ctype;
    ctype cha;
    try{
        format.decode(&cha, data, st);
    }
    catch(const buffer_small &){
        return false;
    }
    return cha == ctype{0};
}

template<typename T, typename U>
class adv_string; //forward declaration
template<typename T, typename V, typename U>
class adv_string_buf_0;


template<typename T>
class adv_string_view{
	private:
		const_tchar_pt<T> ptr;
		size_t len;//character number
		size_t siz;//bytes number
	protected:
		explicit adv_string_view(size_t length, size_t size, const_tchar_pt<T> bin) noexcept : ptr{bin}, len{length}, siz{size} {}
	public:
		explicit adv_string_view(const_tchar_pt<T>, const terminate_func<T> & = zero_terminating<T>);
		explicit adv_string_view(const_tchar_pt<T>, size_t dim, meas measure);
		/*
		    read exactly len characters and siz bytes. If these values doesn't match trow error
		*/
		explicit adv_string_view(const_tchar_pt<T>, size_t siz, size_t len);
		
		template<typename U, typename... Arg>
		explicit adv_string_view(const U *b, Arg... args) : adv_string_view{const_tchar_pt<T>{b, args...}} {}
		template<typename U, typename Integer, typename... Arg>
		explicit adv_string_view(const U *b, Integer dim, meas measure, Arg... args) : adv_string_view{const_tchar_pt<T>{b, args...}, dim, measure} {
            static_assert(std::is_integral_v<Integer> && std::is_unsigned_v<Integer> && !std::is_same_v<Integer, bool>, "Invalid dimension type");
        }
		template<typename U, typename Int1, typename Int2, typename... Arg>
		explicit adv_string_view(const U *b, Int1 siz, Int2 len, Arg... args) : adv_string_view{const_tchar_pt<T>{b, args...}, siz, len} {
            static_assert(std::is_integral_v<Int1> && std::is_unsigned_v<Int1> && !std::is_same_v<Int1, bool>, "Invalid dimension type");
            static_assert(std::is_integral_v<Int2> && std::is_unsigned_v<Int2> && !std::is_same_v<Int2, bool>, "Invalid dimension type");
        }
		/*
		    WIDENC costructors
		
		template<typename U, enable_wide_t<T, int, U> = 0>
		explicit adv_string_view(const U *b, const EncMetric<typename T::ctype> &f) : adv_string_view{const_tchar_pt<T>{b, f}} {}
		template<typename U, enable_wide_t<T, int, U> = 0>
		explicit adv_string_view(const U *b, size_t dim, meas measure, const EncMetric<typename T::ctype> &f) : adv_string_view{const_tchar_pt<T>{b, f}, dim, measure} {}
		template<typename U, enable_wide_t<T, int, U> = 0>
		explicit adv_string_view(const U *b, size_t siz, size_t len, const EncMetric<typename T::ctype> &f) : adv_string_view{const_tchar_pt<T>{b, f}, siz, len} {}
		*/

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

		template<typename S>
		bool startsWith(const adv_string_view<S> &) const;

		template<typename S>
		bool endsWith(const adv_string_view<S> &) const;

		const byte *data() const noexcept {return ptr.data();}
		const char *raw() const noexcept {return (const char *)(ptr.data());}
		std::string toString() const noexcept {return std::string{(const char *)(ptr.data()), siz};}
		/*
			Mustn't throw any exception if 0 <= a <= len
		*/
		const_tchar_pt<T> at(size_t chr) const;
		const_tchar_pt<T> begin() const noexcept {return at(0);}
		const_tchar_pt<T> end() const noexcept {return at(len);}

		/*
			These functions convert out string to another one with different encoding
		*/
		/*
			This one is unsafe, since blen is only the number of character to write, not the dimension of buffer
		*/
		template<typename S>
		adv_string_view<S> basic_encoding_conversion(tchar_pt<S> buffer, size_t blen) const;

		template<typename U = std::allocator<byte>>
		adv_string<WIDE<typename T::ctype>, U> basic_encoding_conversion(const EncMetric<typename T::ctype> *, const U & = U{}) const;

		template<typename S, typename U = std::allocator<byte>>
		adv_string<S, U> basic_encoding_conversion(const U & = U{}) const;

		template<typename S, typename U = std::allocator<byte>>
		adv_string<T, U> concatenate(const adv_string_view<S> &, const U & = U{}) const;

	template<typename W, typename S>
	friend adv_string_view<W> reassign(const adv_string_view<S> &);
	template<typename S, typename V, typename R>
	friend class adv_string_buf_0;
};

template<typename T, typename S>
bool sameEnc(const adv_string_view<T> &a, const adv_string_view<S> &b) noexcept{
	return sameEnc(a.begin(), b.begin());
}

template<typename S, typename T>
adv_string_view<S> reassign(const adv_string_view<T> &ret){
	if constexpr(is_raw_v<S>)
		return adv_string_view<S>{reassign<S, T>(ret.begin()), ret.size(), ret.size()};
	else
		return adv_string_view<S>{reassign<S, T>(ret.begin()), ret.length(), ret.size()};
}

template<typename T, typename V, typename U = std::allocator<byte>>
class adv_string_buf_0{
	private:
		basic_ptr<byte, U> buffer;
		EncMetric_info<T> ei;
		size_t siz, len;

		V *mycast() noexcept { return static_cast<V*>(this);}
		V &instance() noexcept { return *(mycast());}
		const V *mycast() const noexcept { return static_cast<V const*>(this);}
		const V &instance() const noexcept { return *(mycast());}
	protected:
		adv_string_buf_0(EncMetric_info<T> f, const U &alloc=U{}) : buffer{alloc}, ei{f}, siz{0}, len{0} {}
		adv_string_buf_0(EncMetric_info<T> f, size_t indim, const U &alloc=U{}) : buffer{indim, alloc}, ei{f}, siz{0}, len{0} {}
	public:
		size_t size() const noexcept { return siz;}
		size_t length() const noexcept {return len;}
		const byte *raw() {return buffer.memory;}

		uint append_chr(const_tchar_pt<T>);
		size_t append_chrs(const_tchar_pt<T>, size_t);
		size_t append_string(adv_string_view<T>);
		V &operator<<(const_tchar_pt<T> p){
			append_chr(p);
			return instance();
		}
		V &operator<<(adv_string_view<T> p){
			append_string(p);
			return instance();
		}

		/*
		    Validate the character(s) before adding to buffer
		*/
		bool append_chr_v(const_tchar_pt<T>, size_t siz);
		bool append_chrs_v(const_tchar_pt<T>, size_t siz, size_t nchr);

		/*
		    Convert the string before adding it
		*/
		template<typename S>
		size_t append_string_c(adv_string_view<S>);

		void clear() noexcept;
		adv_string_view<T> view() const noexcept;
		adv_string<T, U> move();
		template<typename Alloc>
		adv_string<T, Alloc> allocate(const Alloc & = Alloc{}) const;
};

template<typename T, typename U = std::allocator<byte>>
class adv_string_buf : public adv_string_buf_0<T, adv_string_buf<T, U>, U>{
	public:
		adv_string_buf(EncMetric_info<T> f, const U & alloc = U{}) : adv_string_buf_0<T, adv_string_buf<T, U>, U>{f, alloc} {}
		adv_string_buf(const U &alloc = U{}) : adv_string_buf{EncMetric_info<T>{}, alloc} {}
		adv_string_buf(size_t indim, const U &alloc = U{}) : adv_string_buf{EncMetric_info<T>{}, indim, alloc} {}
		adv_string_buf(adv_string_view<T> str, const U &alloc= U{}) : adv_string_buf{EncMetric_info<T>{}, alloc} {
			append_string(str);
		}
};

template<typename tt, typename U>
class adv_string_buf<WIDE<tt>, U> : public adv_string_buf_0<WIDE<tt>, adv_string_buf<WIDE<tt>, U>, U>{
	public:
		adv_string_buf(EncMetric_info<WIDE<tt>> f, const U & alloc = U{}) : adv_string_buf_0<WIDE<tt>, adv_string_buf<WIDE<tt>, U>, U>{f, alloc} {}

		adv_string_buf(const EncMetric<tt> *format, const U &alloc = U{}) : adv_string_buf{EncMetric_info<WIDE<tt>>{format}, alloc} {}
		adv_string_buf(const EncMetric<tt> *format, size_t indim, const U &alloc = U{}) : adv_string_buf{EncMetric_info<WIDE<tt>>{format}, indim, alloc} {}
		adv_string_buf(adv_string_view<WIDE<tt>> str, const U &alloc= U{}) : adv_string_buf{EncMetric_info<WIDE<tt>>{str.format()}, alloc} {
			append_string(str);
		}
};

template<typename tt>
class adv_string_buf<WIDE<tt>, std::allocator<byte>> : public adv_string_buf_0<WIDE<tt>, adv_string_buf<WIDE<tt>, std::allocator<byte>>, std::allocator<byte>>{
	public:
		adv_string_buf(EncMetric_info<WIDE<tt>> f, const std::allocator<byte> & alloc = std::allocator<byte>{}) : adv_string_buf_0<WIDE<tt>, adv_string_buf<WIDE<tt>, std::allocator<byte>>, std::allocator<byte>>{f, alloc} {}

		adv_string_buf(const EncMetric<tt> *format, const std::allocator<byte> &alloc = std::allocator<byte>{}) : adv_string_buf{EncMetric_info<WIDE<tt>>{format}, alloc} {}
};

template<typename T, typename U = std::allocator<byte>>
class adv_string : public adv_string_view<T>{
	private:
		basic_ptr<byte, U> bind;

		adv_string(const_tchar_pt<T>, size_t, size_t, basic_ptr<byte, U>);

		/*
			USE WITH EXTREME CARE
			init with memory pointed by data and ignore ptr, use it only to detect encoding
			ignore is ignored
		*/
		adv_string(const_tchar_pt<T> ptr, size_t len, size_t siz, basic_ptr<byte, U> data, int ignore);
	public:
		adv_string(const adv_string_view<T> &, const U & = U{});
		adv_string(const adv_string<T, U> &me) : adv_string{static_cast<const adv_string_view<T> &>(me), me.get_allocator()} {}
		adv_string(adv_string &&st) noexcept =default;

		U get_allocator() const noexcept{return bind.get_allocator();}
		std::size_t capacity() const noexcept{ return bind.dimension;}
		static adv_string<T, U> newinstance_ter(const_tchar_pt<T>, const terminate_func<T> &, const U & = U{});
		static adv_string<T, U> newinstance(const_tchar_pt<T> p, const U &alloc = U{}){return newinstance_ter(p, zero_terminating<T>, alloc);}
	template<typename S>
	friend class adv_string_view;
	template<typename S, typename V, typename R>
	friend class adv_string_buf_0;
};

template<typename S, typename T, typename U = std::allocator<byte>>
adv_string<S, U> operator+(const adv_string<S, U> &a, const adv_string<T, U> &b){
	return a.template concatenate<T, U>(b);
}

using wstr_view = adv_string_view<WIDE<unicode>>;

template<typename U = std::allocator<byte>>
using wstr_d = adv_string<WIDE<unicode>, U>;

using wstr = wstr_d<>;


#include <encmetric/enc_string.tpp>
}


