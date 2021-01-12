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
    Encoding Metrics classes, with some useful base classes.

    Any implementation of an Encoding Metric must have these function member declared as 
    static:

     - int unity() noexcept  => minimum number of bytes needed to detect the length of a character
     - int chLen(const byte *)  => the length in bytes of the first character pointed by (can throw
        an encoding_error if the length can't be recognized). The first purpouse of this function
        is only to calculate le length of a character, not to verify it.
     - bool validChar(const byte *, int &) noexcept  => Test is the first character is valid with 
        respect to this encoding. This function sets also the character length in the second argument if
        it is valid (if the character is not valid the status of this argument is undefined).

    Optional functions, also defined as static:
 
     - int to_unicode(unicode &, const byte *, int)  => sets the Unicode code of the first encoded characters
     - int from_unicode(unicode , byte *, int)  => encode the Unicode character and writes it in the memory pointed

     Both have a integer parameter that represents the size of the string/buffer, and returns the nunber of bytes
     read/written. If the string parameter in to_unicode is too small return 0 without throw an exception 
*/
#include <encmetric/base.hpp>
#include <typeindex>
#include <type_traits>

namespace adv{

void copyN(const byte *, byte *, int) noexcept;

inline bool is_all_zero(const byte *data, int len){
	for(int i=0; i<len; i++){
		if(data[i] != byte{0})
			return false;
	}
	return true;
}

class encoding_error : public std::exception{
	private:
		const char *c;
	public:
		encoding_error(const char *ms) : c{ms} {}
		encoding_error() : encoding_error{"EncMetric error"} {}
		const char *what() const noexcept override {return c;}
};
class EncMetric{
	public:
		virtual ~EncMetric() {}
		virtual int d_unity() const noexcept=0;
		virtual int d_chLen(const byte *) const=0;
		virtual bool d_validChar(const byte *, int &chlen) const noexcept =0;
		virtual int d_to_unicode(unicode &, const byte *, int) const =0;
		virtual int d_from_unicode(unicode, byte *, int) const =0;
		virtual std::type_index index() const noexcept=0;
};

/*
    Placeholder: the encoding is determined at runtime
*/
class WIDENC{};

class RAW{
	public:
	static constexpr int unity() noexcept {return 1;}
	static int chLen(const byte *) {return 1;}
	static bool validChar(const byte *, int &i) noexcept{
		i=1;
		return true;
	}
};

template<typename T>
using is_widenc = std::is_same<T, WIDENC>;
template<typename T>
inline constexpr bool is_widenc_v = is_widenc<T>::value;
template<typename T>
using is_raw = std::is_same<T, RAW>;
template<typename T>
inline constexpr bool is_raw_v = is_raw<T>::value;

template<typename T, typename U>
using enable_widenc = std::enable_if<is_widenc_v<T>, U>;
template<typename T, typename U>
using enable_not_widenc = std::enable_if<!is_widenc_v<T>, U>;
template<typename T, typename U>
using enable_widenc_t = typename enable_widenc<T, U>::type;
template<typename T, typename U>
using enable_not_widenc_t = typename enable_not_widenc<T, U>::type;

/*
    Struct to test if the encoding class has conversion functions to/from Unicode (for example RAW haven't them)

    You should use this wrap struct instead of calling directly, otherwise your functions won't be compiled 
    with the RAW encoding or another encoding incompatible with Unicode..
*/
template<typename T, typename = void>
struct has_unicode_tf{
	static int go(unicode &, const byte *, int) {throw encoding_error("Unsupported Unicode conversion");}
};

template<typename T>
struct has_unicode_tf<T, std::void_t<decltype(static_cast<int(*)(unicode &, const byte *, int)>(T::to_unicode))>>{
	static int go(unicode &a, const byte *b, int c) {return T::to_unicode(a, b, c);}
};

template<typename T, typename = void>
struct has_unicode_ff{
	static int go(unicode , byte *, int) {throw encoding_error("Unsupported Unicode conversion");}
};

template<typename T>
struct has_unicode_ff<T, std::void_t<decltype(static_cast<int(*)(unicode, byte *, int)>(T::from_unicode))>>{
	static int go(unicode a, byte *b, int c) {return T::from_unicode(a, b, c);}
};

template<typename T, enable_not_widenc_t<T, int> =0>
struct em_traits{
	static int to_unicode(unicode &a, const byte *b, int c)  {return has_unicode_tf<T>::go(a, b, c);}
	static int from_unicode(unicode a, byte *b, int c)  {return has_unicode_ff<T>::go(a, b, c);}
	static std::type_index index() noexcept {return std::type_index{typeid(T)};}
};
//-------------------
/*
    Wrapper of an encoding T in order to save it in a class field of WIDENC classes
*/
template<typename T, enable_not_widenc_t<T, int> =0>
class dyn_encoding : public EncMetric{
	public:
		using static_enc = T;

		dyn_encoding() {}
		~dyn_encoding() {}

		int d_unity() const noexcept {return static_enc::unity();}
		int d_chLen(const byte *b) const {return static_enc::chLen(b);}
		bool d_validChar(const byte *b, int &chlen) const noexcept {return static_enc::validChar(b, chlen);}
		std::type_index index() const noexcept {return em_traits<T>::index();}

		//dato che le funzioni unicode sono opzionali servono le strutture di supporto
		int d_to_unicode(unicode &uni, const byte *by, int l) const {return em_traits<T>::to_unicode(uni, by, l);}
		int d_from_unicode(unicode uni, byte *by, int l) const {return em_traits<T>::from_unicode(uni, by, l);}

		static const EncMetric &instance() noexcept{
			static dyn_encoding<T> t{};
			return t;
		}
};

template<typename T>
constexpr void assert_raw(){static_assert(!is_raw_v<T>, "Using RAW format");}

inline void assert_raw(const EncMetric &f){
	if(f.index() == em_traits<RAW>::index())
		throw encoding_error("Using RAW format");
}

/*
    Some basic encodings
*/
class ASCII{
	public:
		static constexpr int unity() noexcept {return 1;}
		static int chLen(const byte *);
		static bool validChar(const byte *, int &) noexcept;
		static int to_unicode(unicode &uni, const byte *by, int l);
		static int from_unicode(unicode uni, byte *by, int l);
};

class Latin1{
	public:
		static constexpr int unity() noexcept {return 1;}
		static int chLen(const byte *);
		static bool validChar(const byte *, int &) noexcept;
		static int to_unicode(unicode &uni, const byte *by, int l);
		static int from_unicode(unicode uni, byte *by, int l);
};

}


