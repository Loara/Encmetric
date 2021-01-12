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
template<typename U, typename B>
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
		explicit base_tchar_pt(data_type *b) : ptr{b} {}
	public:
		/*
		    Raw pointer
		*/
		data_type *data() const {return ptr;}
		/*
		    Informations about relative EncMetric
		*/
		int b_unity() const noexcept {return mycast()->unity();}
		int b_chLen() const {return mycast()->chLen();}
		bool b_validChar(int &chsiz) const noexcept {return mycast()->validChar(chsiz);}
		int b_to_unicode(unicode &uni, int l) const {return mycast()->to_unicode(uni, l);}
		bool b_terminate() const{return mycast()->terminate();}
		/*
		    Step the pointer by 1 character, returns the number og bytes skipped
		*/
		int next(){
			int add = b_chLen();
			ptr += add;
			return add;
		}
		/*
		    Validate the first character, then skip it

		    rsiz is the number of bytes of ptr, it will be updated
		*/
		bool valid_next(int &rsiz) noexcept{
			if(b_unity() > rsiz)
				return false;
			int dec;
			if(!b_validChar(dec))
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
		data_type &operator[](int i) const {return ptr[i];}
		/*
		    Increase ptr by i bytes (not characters)
		*/
		U operator+(std::ptrdiff_t i) const{
			if(i <= 0)
				return instance();
			else
				return p_new_instance(ptr + i);
		}
		/*
		    Steps this by 1 character
		*/
		U& operator++(){
			next();
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
		*/
		U& add(int times){
			if(times > 0){
				int add = b_chLen(times);
				ptr += add;
			}
			return instance();
		}
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
template<typename U>
class wbase_tchar_pt : public base_tchar_pt<U, byte>{
	public:
		explicit wbase_tchar_pt(byte *b) : base_tchar_pt<U, byte>{b} {}
		
		int b_from_unicode(unicode uni, int l) const {return this->mycast()->from_unicode(uni, l);}
};

/*
    Standard implementations

    WIDENC implementation have also a EncMetric *f field (usually a dyn_encoding instance)
    in order to retrieve encoding dynamically
*/
template<typename T>
class const_tchar_pt : public base_tchar_pt<const_tchar_pt<T>, byte const>{
	public:
		using static_enc = T;

		explicit const_tchar_pt() : base_tchar_pt<const_tchar_pt<T>, byte const>{nullptr} {}
		explicit const_tchar_pt(const byte *c) : base_tchar_pt<const_tchar_pt<T>, byte const>{c} {}
		explicit const_tchar_pt(const char *c) : const_tchar_pt{(const byte *)c} {}

		const EncMetric &format() const noexcept {return dyn_encoding<T>::instance();}
		int unity() const noexcept {return T::unity();}
		int chLen() const {return T::chLen(this->ptr);}
		bool validChar(int &chsiz) const noexcept {return T::validChar(this->ptr, chsiz);}
		int to_unicode(unicode &uni, int l) const {return em_traits<T>::to_unicode(uni, this->ptr, l);}
		bool terminate() const {return is_all_zero(this->ptr, T::unity());}

		const_tchar_pt new_instance(const byte *c) const{return const_tchar_pt<T>{c};}
		const_tchar_pt new_instance(const char *c) const{return const_tchar_pt<T>{c};}
};

template<>
class const_tchar_pt<WIDENC> : public base_tchar_pt<const_tchar_pt<WIDENC>, byte const>{
	private:
		const EncMetric *f;
	public:
		using static_enc = WIDENC;

		explicit const_tchar_pt(const EncMetric &rf) : base_tchar_pt<const_tchar_pt<WIDENC>, byte const>{nullptr}, f{&rf} {}
		explicit const_tchar_pt(const byte *c, const EncMetric &rf) : base_tchar_pt<const_tchar_pt<WIDENC>, byte const>{c}, f{&rf} {}
		explicit const_tchar_pt(const char *c, const EncMetric &rf) : const_tchar_pt{(const byte *)c, rf} {}
		explicit const_tchar_pt(const EncMetric &&rf)=delete;
		explicit const_tchar_pt(const byte *c, const EncMetric &&rf)=delete;
		explicit const_tchar_pt(const char *c, const EncMetric &&rf)=delete;

		const EncMetric &format() const noexcept {return *f;}
		int unity() const noexcept {return f->d_unity();}
		int chLen() const {return f->d_chLen(this->ptr);}
		bool validChar(int &chsiz) const noexcept {return f->d_validChar(this->ptr, chsiz);}
		int to_unicode(unicode &uni, int l) const {return f->d_to_unicode(uni, this->ptr, l);}
		bool terminate() const {return is_all_zero(this->ptr, f->d_unity());}

		const_tchar_pt<WIDENC> new_instance(const byte *c) const{return const_tchar_pt<WIDENC>{c, *f};}
		const_tchar_pt<WIDENC> new_instance(const char *c) const{return const_tchar_pt<WIDENC>{c, *f};}
};

//-----------

template<typename T>
class tchar_pt : public wbase_tchar_pt<tchar_pt<T>>{
	public:
		using static_enc = T;

		explicit tchar_pt() : wbase_tchar_pt<tchar_pt<T>>{nullptr} {}
		explicit tchar_pt(byte *c) : wbase_tchar_pt<tchar_pt<T>>{c} {}
		explicit tchar_pt(char *c) : tchar_pt{(byte *)c} {}

		const_tchar_pt<T> cast() noexcept{ return const_tchar_pt<T>{this->ptr};}

		const EncMetric &format() const noexcept {return dyn_encoding<T>::instance();}
		int unity() const noexcept {return T::unity();}
		int chLen() const {return T::chLen(this->ptr);}
		bool validChar(int &chsiz) const noexcept {return T::validChar(this->ptr, chsiz);}
		int to_unicode(unicode &uni, int l) const {return em_traits<T>::to_unicode(uni, this->ptr, l);}
		int from_unicode(unicode uni, int l) const {return em_traits<T>::from_unicode(uni, this->ptr, l);}
		bool terminate() const {return is_all_zero(this->ptr, T::unity());}

		tchar_pt new_instance(byte *c) const{return tchar_pt<T>{c};}
		tchar_pt new_instance(char *c) const{return tchar_pt<T>{c};}
};

template<>
class tchar_pt<WIDENC> : public wbase_tchar_pt<tchar_pt<WIDENC>>{
	private:
		const EncMetric *f;
	public:
		using static_enc = WIDENC;

		explicit tchar_pt(const EncMetric &rf) : wbase_tchar_pt<tchar_pt<WIDENC>>{nullptr}, f{&rf} {}
		explicit tchar_pt(byte *c, const EncMetric &rf) : wbase_tchar_pt<tchar_pt<WIDENC>>{c}, f{&rf} {}
		explicit tchar_pt(char *c, const EncMetric &rf) : tchar_pt{(byte *)c, rf} {}
		explicit tchar_pt(const EncMetric &&rf)=delete;
		explicit tchar_pt(byte *c, const EncMetric &&rf)=delete;
		explicit tchar_pt(char *c, const EncMetric &&rf)=delete;
		const EncMetric &format() const noexcept {return *f;}

		const_tchar_pt<WIDENC> cast() noexcept{ return const_tchar_pt<WIDENC>{this->ptr, *f};}

		int unity() const noexcept {return f->d_unity();}
		int chLen() const {return f->d_chLen(this->ptr);}
		bool validChar(int &chsiz) const noexcept {return f->d_validChar(this->ptr, chsiz);}
		int to_unicode(unicode &uni, int l) const {return f->d_to_unicode(uni, this->ptr, l);}
		int from_unicode(unicode uni, int l) const {return f->d_from_unicode(uni, this->ptr, l);}
		bool terminate() const {return is_all_zero(this->ptr, f->d_unity());}

		tchar_pt<WIDENC> new_instance(byte *c) const{return tchar_pt<WIDENC>{c, *f};}
		tchar_pt<WIDENC> new_instance(char *c) const{return tchar_pt<WIDENC>{c, *f};}
};

//---------------------------------------------

/*
    Test if the pointers have the same encoding (if one of them has WIDENC encoding then control the f field)
*/
template<typename S, typename T>
bool sameEnc(const const_tchar_pt<S> &, const const_tchar_pt<T> &);
template<typename T1, typename T2>
bool sameEnc(const T1 &arg1, const T2 &arg2){
	return sameEnc(const_tchar_pt<typename T1::static_enc>{arg1}, const_tchar_pt<typename T2::static_enc>{arg2});
}
template<typename S, typename T, typename... Rarg>
bool sameEnc(const S &f1, const T &f2, const Rarg &... far){
	return sameEnc(f2, far...) && sameEnc(f1, f2);
}
/*
template<typename S, typename T, typename... Rarg>
bool sameEnc(const const_tchar_pt<S> &f1, const const_tchar_pt<T> &f2, const const_tchar_pt<Rarg> &... far){
	return sameEnc(f2, far...) && sameEnc(f1, f2);
}
*/

/*
    Return a new pointer pointing to the same array and with the same encoding, but with possible different template parameter.
*/
template<typename S, typename T>
tchar_pt<S> convert(tchar_pt<T> p);
template<typename S, typename T>
const_tchar_pt<S> convert(const_tchar_pt<T> p);

/*
    Make an encoding conversion between Unicode-compatible encodings using from_unicode and to_unicode functions.
*/
template<typename S, typename T>
void basic_encoding_conversion(const_tchar_pt<T> in, int inlen, tchar_pt<S> out, int oulen);
template<typename S, typename T>
void basic_encoding_conversion(const_tchar_pt<T> in, int inlen, tchar_pt<S> out, int oulen, int &inread, int &outwrite);

using c_wchar_pt = const_tchar_pt<WIDENC>;
using wchar_pt = tchar_pt<WIDENC>;

#include <encmetric/chite.tpp>
}

