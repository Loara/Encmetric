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
/*
    Pointers to encoded string types.

    We don't use a generic const byte * to point the string, instead we use an
    encoding-dependent class in order to simplify the char-length operations
*/
/*
    Remember that
    - length = number of characters
    - size = number of bytes
*/
#include <type_traits>
#include <typeindex>
#include <cstddef>
#include <encmetric/encoding.hpp>

namespace adv{

/*
    Base class of all pointer-type operations
*/
template<typename T, typename U, typename B>
class base_tchar_pt{
	private:
		using data_type = B;
		/*
		    Static cast operations
		*/
		U* mycast() noexcept {return static_cast<U*>(this);}
		U& instance() noexcept {return *(mycast());}
		const U* mycast() const noexcept {return static_cast<const U*>(this);}
		const U& instance() const noexcept {return *(mycast());}
		U p_new_instance(data_type *bin) const noexcept {return mycast()->new_instance(bin);}

	protected:
		data_type *ptr;
		explicit constexpr base_tchar_pt(data_type *b) : ptr{b} {}
	public:
		/*
		    Raw pointer
		*/
		data_type *data() const {return ptr;}
		/*
		    Informations about relative EncMetric
		*/
		uint b_unity() const noexcept {return mycast()->unity();}
		bool b_has_max() const noexcept {return mycast()->has_max();}
		uint b_max_bytes() const {return mycast()->max_bytes();}
		uint b_chLen() const {return mycast()->chLen();}
		bool b_validChar(uint &chsiz) const noexcept {return mycast()->validChar(chsiz);}
		uint b_decode(typename T::ctype *uni, size_t l) const {return mycast()->decode(uni, l);}
		bool b_terminate() const{return mycast()->terminate();}
		/*
		    Step the pointer by 1 character, returns the number og bytes skipped
		*/
		uint next(){
			if constexpr(fixed_size<T>){
				ptr += T::unity();
				return T::unity();
			}
			else{
				uint add = b_chLen();
				ptr += add;
				return add;
			}
		}
		/*
		    Validate the first character, then skip it

		    rsiz is the number of bytes of ptr, it will be updated
		*/
		bool valid_next(size_t &rsiz) noexcept{
			if(b_unity() > rsiz)
				return false;
			uint dec;
			if(!b_validChar(dec))
				return false;
			if(dec > rsiz)
				return false;
			rsiz -= dec;
			ptr += dec;
			return true;
		}
		/*
		    Test if is a null string
		*/
		bool isNull() const {return ptr == nullptr;}
		/*
		    Access ptr as a byte array
		*/
		data_type &operator[](uint i) const {return ptr[i];}
		/*
		    Increase ptr by i bytes (not characters)
		*/
		U operator+(std::ptrdiff_t i) const{
			if(i <= 0)
				return instance();
			else
				return p_new_instance(ptr + i);
		}
		U &operator+=(std::ptrdiff_t i){
			if(i > 0)
				ptr += i;
			return instance();
		}
		/*
		    Difference of pointers in bytes
		*/
		std::ptrdiff_t operator-(const U &oth) const noexcept{
			return ptr - oth.data();
		}
		/*
		    Steps this by times character
		    
		    Use this function instead of operator+
		
		U& add(size_t times){
			if(times > 0){
				int add = b_chLen(times);
				ptr += add;
			}
			return instance();
		}
			Useless
		*/
		/*
		    Compairson
		*/
		bool operator==(const U &it) const {return ptr == it.data();}
		bool operator!=(const U &oth) const {return ptr != oth.data();}

		/*
		    Let two pointers base e newbase returns ret so that

		    ret.ptr-newbase = *this.ptr - base
		*/
		U transfer_to(data_type *base, data_type *newbase) const noexcept{
			data_type *newww = newbase + (ptr - base);
			return p_new_instance(newww);
		}		
};

/*
    Adds the funstion wrapper from_unicode for rw pointers
*/
template<typename T, typename U>
class wbase_tchar_pt : public base_tchar_pt<T, U, byte>{
	public:
		explicit constexpr wbase_tchar_pt(byte *b) : base_tchar_pt<T, U, byte>{b} {}
		
		uint b_encode(const typename T::ctype &uni, size_t l) const {return this->mycast()->encode(uni, l);}
};

/*
    Standard implementations

    WIDENC implementation have also a EncMetric *f field in order to retrieve encoding dynamically
*/
template<typename T>
class const_tchar_pt : public base_tchar_pt<T, const_tchar_pt<T>, byte const>{
	public:
		using static_enc = T;

		explicit constexpr const_tchar_pt() : base_tchar_pt<T, const_tchar_pt<T>, byte const>{nullptr} {}
		explicit constexpr const_tchar_pt(const byte *c) : base_tchar_pt<T, const_tchar_pt<T>, byte const>{c} {}
		explicit constexpr const_tchar_pt(const char *c) : const_tchar_pt{(const byte *)c} {}

		const EncMetric<typename T::ctype> &format() const noexcept {return DynEncoding<T>::instance();}
		uint unity() const noexcept {return T::unity();}
		uint max_bytes() const {return T::max_bytes();}
		uint chLen() const {return T::chLen(this->ptr);}
		bool validChar(uint &chsiz) const noexcept {return T::validChar(this->ptr, chsiz);}
		uint decode(typename static_enc::ctype *uni, size_t l) const {return T::decode(uni, this->ptr, l);}
		bool terminate() const {return is_all_zero(this->ptr, T::unity());}

		const_tchar_pt new_instance(const byte *c) const{return const_tchar_pt<T>{c};}
		const_tchar_pt new_instance(const char *c) const{return const_tchar_pt<T>{c};}
};

template<typename tt>
class const_tchar_pt<WIDE<tt>> : public base_tchar_pt<WIDE<tt>, const_tchar_pt<WIDE<tt>>, byte const>{
	private:
		const EncMetric<tt> *f;
	public:
		using static_enc = WIDE<tt>;

		explicit const_tchar_pt(const EncMetric<tt> &rf) : base_tchar_pt<WIDE<tt>, const_tchar_pt<WIDE<tt>>, byte const>{nullptr}, f{&rf} {}
		explicit const_tchar_pt(const byte *c, const EncMetric<tt> &rf) : base_tchar_pt<WIDE<tt>, const_tchar_pt<WIDE<tt>>, byte const>{c}, f{&rf} {}
		explicit const_tchar_pt(const char *c, const EncMetric<tt> &rf) : const_tchar_pt{(const byte *)c, rf} {}
		explicit const_tchar_pt(const EncMetric<tt> &&rf)=delete;
		explicit const_tchar_pt(const byte *c, const EncMetric<tt> &&rf)=delete;
		explicit const_tchar_pt(const char *c, const EncMetric<tt> &&rf)=delete;

		const EncMetric<tt> &format() const noexcept {return *f;}
		uint unity() const noexcept {return f->d_unity();}
		uint max_bytes() const {return f->d_max_bytes();}
		uint chLen() const {return f->d_chLen(this->ptr);}
		bool validChar(uint &chsiz) const noexcept {return f->d_validChar(this->ptr, chsiz);}
		uint decode(tt &uni, size_t l) const {return f->d_decode(uni, this->ptr, l);}
		bool terminate() const {return is_all_zero(this->ptr, f->d_unity());}

		const_tchar_pt<WIDE<tt>> new_instance(const byte *c) const{return const_tchar_pt<WIDE<tt>>{c, *f};}
		const_tchar_pt<WIDE<tt>> new_instance(const char *c) const{return const_tchar_pt<WIDE<tt>>{c, *f};}
};

//-----------

template<typename T>
class tchar_pt : public wbase_tchar_pt<T, tchar_pt<T>>{
	public:
		using static_enc = T;

		explicit tchar_pt() : wbase_tchar_pt<T, tchar_pt<T>>{nullptr} {}
		explicit tchar_pt(byte *c) : wbase_tchar_pt<T, tchar_pt<T>>{c} {}
		explicit tchar_pt(char *c) : tchar_pt{(byte *)c} {}

		const_tchar_pt<T> cast() noexcept{ return const_tchar_pt<T>{this->ptr};}

		const EncMetric<typename T::ctype> &format() const noexcept {return DynEncoding<T>::instance();}
		uint unity() const noexcept {return T::unity();}
		uint max_bytes() const {return T::max_bytes();}
		uint chLen() const {return T::chLen(this->ptr);}
		bool validChar(uint &chsiz) const noexcept {return T::validChar(this->ptr, chsiz);}
		uint decode(typename T::ctype *uni, size_t l) const {return T::decode(uni, this->ptr, l);}
		uint encode(const typename T::ctype &uni, size_t l) const {return T::encode(uni, this->ptr, l);}
		bool terminate() const {return is_all_zero(this->ptr, T::unity());}

		tchar_pt new_instance(byte *c) const{return tchar_pt<T>{c};}
		tchar_pt new_instance(char *c) const{return tchar_pt<T>{c};}
};

template<typename tt>
class tchar_pt<WIDE<tt>> : public wbase_tchar_pt<WIDE<tt>, tchar_pt<WIDE<tt>>>{
	private:
		const EncMetric<tt> *f;
	public:
		using static_enc = WIDE<tt>;

		explicit tchar_pt(const EncMetric<tt> &rf) : wbase_tchar_pt<WIDE<tt>, tchar_pt<WIDE<tt>>>{nullptr}, f{&rf} {}
		explicit tchar_pt(byte *c, const EncMetric<tt> &rf) : wbase_tchar_pt<WIDE<tt>, tchar_pt<WIDE<tt>>>{c}, f{&rf} {}
		explicit tchar_pt(char *c, const EncMetric<tt> &rf) : tchar_pt{(byte *)c, rf} {}
		explicit tchar_pt(const EncMetric<tt> &&rf)=delete;
		explicit tchar_pt(byte *c, const EncMetric<tt> &&rf)=delete;
		explicit tchar_pt(char *c, const EncMetric<tt> &&rf)=delete;
		const EncMetric<tt> &format() const noexcept {return *f;}

		const_tchar_pt<WIDE<tt>> cast() noexcept{ return const_tchar_pt<WIDE<tt>>{this->ptr, *f};}

		uint unity() const noexcept {return f->d_unity();}
		uint max_bytes() const {return f->d_max_bytes();}
		uint chLen() const {return f->d_chLen(this->ptr);}
		bool validChar(uint &chsiz) const noexcept {return f->d_validChar(this->ptr, chsiz);}
		uint decode(tt *uni, size_t l) const {return f->d_decode(uni, this->ptr, l);}
		uint encode(const tt &uni, size_t l) const {return f->d_encode(uni, this->ptr, l);}
		bool terminate() const {return is_all_zero(this->ptr, f->d_unity());}

		tchar_pt<WIDE<tt>> new_instance(byte *c) const{return tchar_pt<WIDE<tt>>{c, *f};}
		tchar_pt<WIDE<tt>> new_instance(char *c) const{return tchar_pt<WIDE<tt>>{c, *f};}
};

//---------------------------------------------

/*
    Test if the pointers have the same encoding (if one of them has WIDENC encoding then control the f field)
*/
template<typename S, typename T>
inline constexpr bool sameEnc_static = !is_wide_v<S> && !is_wide_v<T> && index_traits<S>::index() == index_traits<T>::index();

template<typename S, typename T>
bool sameEnc(const const_tchar_pt<S> &, const const_tchar_pt<T> &) noexcept;
template<typename T1, typename T2>
bool sameEnc(const T1 &arg1, const T2 &arg2) noexcept{
	return sameEnc(const_tchar_pt<typename T1::static_enc>{arg1}, const_tchar_pt<typename T2::static_enc>{arg2});
}
template<typename S, typename T, typename... Rarg>
bool sameEnc(const S &f1, const T &f2, const Rarg&... tre) noexcept{
	return sameEnc(f2, tre...) && sameEnc(f1, f2);
}
/*
T not wide
*/
template<typename S, typename T>
bool sameEnc(const const_tchar_pt<S> &) noexcept;

/*
    Return a new pointer pointing to the same array and with the same encoding, but with possible different template parameter.
*/
template<typename S, typename T>
tchar_pt<S> convert(tchar_pt<T> p);
template<typename S, typename T>
const_tchar_pt<S> convert(const_tchar_pt<T> p);

/*
    Assign an encoding to a RAW character pointer
*/
template<typename tt>
inline tchar_pt<WIDE<tt>> assign(tchar_pt<RAW<tt>> r, const EncMetric<tt> &f) noexcept {return tchar_pt<WIDE<tt>>{r.data(), f};}
template<typename tt>
inline const_tchar_pt<WIDE<tt>> assign(const_tchar_pt<RAW<tt>> r, const EncMetric<tt> &f) noexcept {return const_tchar_pt<WIDE<tt>>{r.data(), f};}

/*
    Make an encoding conversion between Unicode-compatible encodings using from_unicode and to_unicode functions.
    Note: convert only the first character
*/
template<typename S, typename T, enable_same_data_t<S, T, int> =0>
void basic_encoding_conversion(const_tchar_pt<T> in, uint inlen, tchar_pt<S> out, uint oulen);
template<typename S, typename T, enable_same_data_t<S, T, int> =0>
void basic_encoding_conversion(const_tchar_pt<T> in, uint inlen, tchar_pt<S> out, uint oulen, uint &inread, uint &outwrite);

/*
    Estimate the size of a possible string with n characters
*/
template<typename T>
uint min_size_estimate(const_tchar_pt<T>, uint) noexcept;
template<typename T>
uint max_size_estimate(const_tchar_pt<T>, uint);

template<typename T>
bool dynamic_fixed_size(const_tchar_pt<T>) noexcept;

using c_wchar_pt = const_tchar_pt<WIDE<unicode>>;
using wchar_pt = tchar_pt<WIDE<unicode>>;

#include <encmetric/chite.tpp>
}

