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

    Any implementation of an Encoding Metric must have a typedef 'ctype' represents the type of data you want to encode/decode
    (for character encodings you can use Unicode). Also it must have these function member declared as 
    static:

     - constexpr unsigned int unity() noexcept  => minimum number of bytes needed to detect the length of a character
     - constexpr bool has_max() noexcept => the encoding fixes the maximum number of bytes per-character
     - constexpr unsigned int max_bytes() noexcept => maximum number of bytes needed to store an entire character, undefined
       if has_max is false
     - unsigned int chLen(const byte *)  => the length in bytes of the first character pointed by (can throw
        an encoding_error if the length can't be recognized). The first purpouse of this function
        is only to calculate le length of a character, not to verify it.
     - bool validChar(const byte *, unsigned int &) noexcept  => Test is the first character is valid with 
        respect to this encoding. This function sets also the character length in the second argument if
        it is valid (if the character is not valid the status of this argument is undefined). 
     - unsigned int decode(T *, const byte *, size_t)  => sets the Unicode code of the first encoded characters
        and returns the number of bytes read. If there aren't enough bytes it must throw buffer_small
     - unsigned int encode(const T &, byte *, size_t)  => encode the Unicode character and writes it in the memory pointed
        and returns the number of bytes written. If there isn't enough space it must throw buffer_small
*/
#include <encmetric/base.hpp>
#include <typeindex>
#include <type_traits>
#include <cstring>
#include <iostream>

namespace adv{

inline void copyN(const byte *src, byte *des, size_t l) {std::memcpy(des, src, l);}

inline bool is_all_zero(const byte *data, size_t len){
	for(size_t i=0; i<len; i++){
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
class buffer_small : public encoding_error{
	private:
		/*
			Minimum size required, 0 if not determined
		*/
		uint mins;
	public:
		buffer_small(const char *ms) : encoding_error{ms}, mins{0} {}
		buffer_small() : encoding_error{"Buffer is too small"}, mins{0} {}
		buffer_small(uint ms) : encoding_error{"Buffer is too small"}, mins{ms} {}
		uint get_required_size() const noexcept{
			return mins;
		}
};


template<typename T>
class EncMetric{
	public:
		using ctype=T;
		virtual ~EncMetric() {}
		virtual uint d_unity() const noexcept=0;
		virtual bool d_has_max() const noexcept=0;
		virtual uint d_max_bytes() const=0;
		virtual uint d_chLen(const byte *) const=0;
		virtual bool d_validChar(const byte *, uint &chlen) const noexcept =0;
		virtual uint d_decode(ctype *, const byte *, size_t) const =0;
		virtual uint d_encode(const ctype &, byte *, size_t) const =0;
		virtual bool d_fixed_size() const noexcept =0;
		virtual std::type_index index() const noexcept=0;
};

/*
    Placeholder: the encoding is determined at runtime
*/
template<typename T>
class WIDE{
	public:
		using ctype=T;
};

using WIDEchr=WIDE<unicode>;

/*
    No encoding provided
*/
template<typename T>
class RAW{
	public:
		using ctype=T;
		static constexpr int unity() noexcept {return 1;}
		static constexpr bool has_max() noexcept {return true;}
		static constexpr int max_bytes() {return 1;}
		using equivalent_enc=RAW<byte>;
		static uint chLen(const byte *) {return 1;}
		static bool validChar(const byte *, uint &i) noexcept{
			i=1;
			return true;
		}
		static uint decode(T *, const byte *, size_t) {throw encoding_error{"RAW encoding can't be converted"};}
		static uint encode(const T &, byte *, size_t) {throw encoding_error{"RAW encoding can't be converted"};}
};

using RAWchr=RAW<unicode>;

template<typename S, typename T>
struct same_data : public std::is_same<typename S::ctype, typename T::ctype> {};
template<typename S, typename T>
inline constexpr bool same_data_v = same_data<S, T>::value;

template<typename U>
struct is_wide : public std::false_type {};
template<typename tt>
struct is_wide<WIDE<tt>> : public std::true_type {};

template<typename T>
inline constexpr bool is_wide_v = is_wide<T>::value;

template<typename U>
struct is_raw : public std::false_type {};
template<typename tt>
struct is_raw<RAW<tt>> : public std::true_type {};

template<typename T>
inline constexpr bool is_raw_v = is_raw<T>::value;

/*
    Template packs are used when you need SFINAE
*/
template<typename S, typename T, typename U, typename...>
struct enable_same_data : public std::enable_if<same_data_v<S, T>, U>{};

template<typename S, typename T, typename U, typename... Args>
using enable_same_data_t = typename enable_same_data<S, T, U, Args...>::type;

template<typename T, typename U, typename...>
struct enable_wide : public std::enable_if<is_wide_v<T>, U> {};
template<typename T, typename U, typename...>
struct enable_not_wide : public std::enable_if<!is_wide_v<T>, U>{};

template<typename T, typename U, typename... Args>
using enable_wide_t = typename enable_wide<T, U, Args...>::type;
template<typename T, typename U, typename... Args>
using enable_not_wide_t = typename enable_not_wide<T, U, Args...>::type;

template<typename T>
inline constexpr bool safe_hasmax = T::has_max();
template<typename tt>
inline constexpr bool safe_hasmax<WIDE<tt>> = false;

template<typename T>
inline constexpr bool fixed_size = T::has_max() && (T::unity() == T::max_bytes());
template<typename tt>
inline constexpr bool fixed_size<WIDE<tt>> = false;

template<typename T>
constexpr int min_length(int nchr) noexcept{
	return T::unity() * nchr;
}
template<typename tt>
int min_length(int nchr, const EncMetric<tt> &format) noexcept{
	return format.d_unity() * nchr;
}

template<typename T>
constexpr int max_length(int nchr){
	static_assert(safe_hasmax<T>, "This encoding has no superior limit");
	return T::max_bytes() * nchr;
}
template<typename tt>
int max_length(int nchr, const EncMetric<tt> &format){
	if(format.d_has_max())
		return format.d_max_bytes() * nchr;
	else
		throw encoding_error{"This encoding has no superior limit"};
}

/*
    index_traits control if encoding class ovverides the index
*/
template<typename T, enable_not_wide_t<T, int>, typename>
struct index_traits_0{
	using type_enc=T;
};

template<typename T>
struct index_traits_0<T, 0, std::void_t<typename T::equivalent_enc>>{
	using type_enc=typename T::equivalent_enc;
};

template<typename T>
struct index_traits : public index_traits_0<T, 0, void> {
	static std::type_index index() noexcept {return std::type_index{typeid(typename index_traits_0<T, 0, void>::type_enc)};}
};

template<typename T>
constexpr void assert_raw(){static_assert(!is_raw_v<T>, "Using RAW format");}

template<typename tt>
void assert_raw(const EncMetric<tt> &f){
	if(f.index() == index_traits<RAW<tt>>::index())
		throw encoding_error("Using RAW format");
}

/*
    Wrapper of an encoding T in order to save it in a class field of WIDENC classes
*/
template<typename T, enable_not_wide_t<T, int> =0>
class DynEncoding : public EncMetric<typename T::ctype>{
	private:		
		DynEncoding() {}
	public:
		using static_enc = T;

		~DynEncoding() {}

		uint d_unity() const noexcept {return static_enc::unity();}
		bool d_has_max() const noexcept {return static_enc::has_max();}
		uint d_max_bytes() const {return static_enc::max_bytes();}
		uint d_chLen(const byte *b) const {return static_enc::chLen(b);}
		bool d_validChar(const byte *b, uint &chlen) const noexcept {return static_enc::validChar(b, chlen);}
		std::type_index index() const noexcept {return index_traits<T>::index();}

		uint d_decode(typename T::ctype *uni, const byte *by, size_t l) const {return static_enc::decode(uni, by, l);}
		uint d_encode(const typename T::ctype &uni, byte *by, size_t l) const {return static_enc::encode(uni, by, l);}

		bool d_fixed_size() const noexcept {return fixed_size<T>;}

		static const EncMetric<typename T::ctype> &instance() noexcept{
			static DynEncoding<T> t{};
			return t;
		}
};

/*
    Store information about used encoding
*/
template<typename T>
class EncMetric_info{
	public:
		using ctype=typename T::ctype;

		constexpr uint unity() const noexcept {return T::unity();}
		constexpr bool has_max() const noexcept {return T::has_max();}
		constexpr uint max_bytes() const noexcept {return T::max_bytes();}
		constexpr bool is_fixed() const noexcept {return fixed_size<T>;}
		uint chLen(const byte *b) const {return T::chLen(b);}
		uint encLen(const ctype &b) const {return T::encLen(b);}
		bool validChar(const byte *b, uint &l) const noexcept {return T::validChar(b, l);}
		uint decode(ctype *uni, const byte *by, size_t l) const {return T::decode(uni, by, l);}
		uint encode(const ctype &uni, byte *by, size_t l) const {return T::encode(uni, by, l);}
		std::type_index index() const noexcept {return index_traits<T>::index();}
};

template<typename tt>
class EncMetric_info<WIDE<tt>>{
	private:
		const EncMetric<tt> *f;
	public:
		using ctype=tt;
		EncMetric_info(const EncMetric<tt> &format) : f{&format} {}

		const EncMetric<tt> &format() const noexcept {return *f;}
		uint unity() const noexcept {return f->d_unity();}
		bool has_max() const noexcept {return f->d_has_max();}
		uint max_bytes() const noexcept {return f->d_max_bytes();}
		bool is_fixed() const noexcept {return f->d_fixed_size();}
		uint chLen(const byte *b) const {return f->d_chLen(b);}
		uint encLen(const ctype &b) const {return f->d_encLen(b);}
		bool validChar(const byte *b, uint &l) const noexcept {return f->d_validChar(b, l);}
		uint decode(ctype *uni, const byte *by, size_t l) const {return f->d_decode(uni, by, l);}
		uint encode(const ctype &uni, byte *by, size_t l) const {return f->d_encode(uni, by, l);}
		std::type_index index() const noexcept {return f->index();}
};

/*
    Some basic encodings
*/
class ASCII{
	public:
		using ctype=unicode;
		static constexpr uint unity() noexcept {return 1;}
		static constexpr bool has_max() noexcept {return true;}
		static constexpr uint max_bytes() noexcept {return 1;}
		static uint chLen(const byte *);
		static bool validChar(const byte *, uint &) noexcept;
		static uint decode(unicode *uni, const byte *by, size_t l);
		static uint encode(const unicode &uni, byte *by, size_t l);
};

class Latin1{
	public:
		using ctype=unicode;
		static constexpr uint unity() noexcept {return 1;}
		static constexpr bool has_max() noexcept {return true;}
		static constexpr uint max_bytes() noexcept {return 1;}
		static uint chLen(const byte *);
		static bool validChar(const byte *, uint &) noexcept;
		static uint decode(unicode *uni, const byte *by, size_t l);
		static uint encode(const unicode &uni, byte *by, size_t l);
};

}


