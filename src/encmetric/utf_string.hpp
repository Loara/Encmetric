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

/*
si ferma al primo carattere (non byte) nullo
Può lanciare eccezioni
*/
template<typename T>
void deduce_lens(const_tchar_pt<T>, int &len, int &siz);

/*
rsiz è la lunghezza massima del buffer
zero dice se fermarsi al primo carattere nullo (senza includerlo)
*/
template<typename T>
void deduce_lens(const_tchar_pt<T>, int rsiz, bool zero, int &len, int &siz);

template<typename T, typename U>
class adv_string; //forward declaration

template<typename T>
class adv_string_view{
	private:
		int len;
		int siz;//bytes
		const_tchar_pt<T> ptr;
	protected:
		explicit adv_string_view(int length, int size, const_tchar_pt<T> bin) noexcept : ptr{bin}, len{length}, siz{size} {}
	public:
		explicit adv_string_view(const_tchar_pt<T>);
		explicit adv_string_view(const_tchar_pt<T>, int dim, bool dim_is_size);//true=size, false=length
		explicit adv_string_view(const_tchar_pt<T>, int siz, int len);//legge esattamente len caratteri, se supera siz da errore
		virtual ~adv_string_view() {}
		/*
		FORTEMENTE CONSIGLIATO chiamare almeno una delle due funzioni seguenti prima di processare i dati di un utente.

		l'algoritmo per determinare la lunghezza della stringa è improntato all'efficienza, non alla correttezza della stessa, e quindi potrebbero essere nascosti errori potenzialmente fatali e vulnerabilità.
		*/
		void verify() const;
		bool verify_safe() const noexcept;
		
		adv_string_view<T> substring(int b, int e, bool endstr) const;
		adv_string_view<T> substring(int b, int e) const {return substring(b, e, false);}
		adv_string_view<T> substring(int b) const {return substring(b, 0, true);}
		int length() const noexcept {return len;}
		int size() const noexcept {return siz;}
		int size(int a, int n) const;//byte dei primi n caratteri a partire da a
		int size(int n) const {return size(0, n);}

		template<typename S>
		bool compstr(const adv_string_view<S> &, int) const;//compara solamente i primi n caratteri

		template<typename S>
		bool operator==(const adv_string_view<S> &) const;

		template<typename S>
		bool operator==(const_tchar_pt<S>) const;

		template<typename S>
		bool operator!=(const adv_string_view<S> &bin) const {return !(*this == bin);}

		template<typename S>
		bool operator!=(const_tchar_pt<S> bin) const {return !(*this == bin);}

		template<typename S>
		int bytesOf(const adv_string_view<S> &) const;//ritorna quanti byte dall'inizio ci sono prima della prima occorrenza, -1 se non compare

		template<typename S>
		int indexOf(const adv_string_view<S> &) const;//stessa cosa ma ritorna il numero di lettere

		template<typename S>
		bool containsChar(const_tchar_pt<S>) const;

		const byte *data() const noexcept {return ptr.data();}
		const char *raw() const noexcept {return (const char *)(ptr.data());}
		std::string toString() const noexcept {return std::string{(const char *)(ptr.data()), (long unsigned)siz};}
		const_tchar_pt<T> begin() const noexcept {return ptr;}
		const_tchar_pt<T> end() const noexcept {return ptr.new_instance(ptr.data()+siz);}

		template<typename S>
		adv_string_view<S> basic_encoding_conversion(tchar_pt<S> buffer, int blen) const;

		template<typename S, typename U = std::allocator<byte>>
		adv_string<S, U> basic_encoding_conversion(const EncMetric &, const U & = U{}, enable_widenc_t<S, int> =0) const;
		template<typename S, typename U = std::allocator<byte>>
		adv_string<S, U> basic_encoding_conversion(const U & = U{}, enable_not_widenc_t<S, int> =0) const;

		template<typename S, typename U = std::allocator<byte>>
		adv_string<T, U> concatenate(const adv_string_view<S> &, const U & = U{}) const;

	//friend std::ostream &operator<<(std::ostream &, const utfstring &);
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

		adv_string(const_tchar_pt<T>, int, int, basic_ptr<byte, U>);
		/*
		il const_str_pointer qui non serve tanto per la memoria che punta, quanto per il formato. La memoria viene data dal basic_ptr
		*/
		adv_string(basic_ptr<byte, U>, const_tchar_pt<T>, int, int);
		//adv_string(basic_ptr<byte, U>, const_tchar_pt<T>, bool);
	public:
		//adv_string(const_tchar_pt<T> b, int dim, bool dim_is_siz, const U & = U{});
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

using w_string_view = adv_string_view<WIDENC>;

template<typename U = std::allocator<byte>>
using w_string = adv_string<WIDENC, U>;


#include <encmetric/utf_string.tpp>
}
#endif


