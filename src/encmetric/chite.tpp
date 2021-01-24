
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
template<typename S, typename T>
bool sameEnc(const const_tchar_pt<S> &, const const_tchar_pt<T> &) {
	return index_traits<S>::index() == index_traits<T>::index();
}

template<typename T>
bool sameEnc(const const_tchar_pt<T> &, const const_tchar_pt<WIDENC> &a){
	const EncMetric &f = a.format();
	return f.index() == index_traits<T>::index();
}

template<typename T>
bool sameEnc(const const_tchar_pt<WIDENC> &a, const const_tchar_pt<T> &){
	const EncMetric &f = a.format();
	return f.index() == index_traits<T>::index();
}

inline bool sameEnc(const const_tchar_pt<WIDENC> &a, const const_tchar_pt<WIDENC> &b){
	const EncMetric &f = a.format();
	const EncMetric &g = b.format();
	return f.index() == g.index();
}

template<typename S, typename T>
tchar_pt<S> convert(tchar_pt<T> p){
	if constexpr( std::is_same_v<S, T> ){
		return p;
	}
	else if constexpr( std::is_same_v<S, WIDENC> ){
		return tchar_pt<WIDENC>{p.data(), T::instance()};
	}
	else if constexpr( std::is_same_v<T, WIDENC> ){
		if(std::type_index{typeid(S)} != p.format().index())
			throw encoding_error("Impossible to convert these strings");
		return tchar_pt<S>(p.data());
	}
	else
		throw encoding_error("Impossible to convert these strings");
}

template<typename S, typename T>
const_tchar_pt<S> convert(const_tchar_pt<T> p){
	if constexpr( std::is_same_v<S, T> ){
		return p;
	}
	else if constexpr( std::is_same_v<S, WIDENC> ){
		return const_tchar_pt<WIDENC>{p.data(), T::instance()};
	}
	else if constexpr( std::is_same_v<T, WIDENC> ){
		if(std::type_index{typeid(S)} != p.format().index())
			throw encoding_error("Impossible to convert these strings");
		return const_tchar_pt<S>(p.data());
	}
	else
		throw encoding_error("Impossible to convert these strings");
}

template<typename S, typename T>
void basic_encoding_conversion(const_tchar_pt<T> in, int inlen, tchar_pt<S> out, int oulen){
	unicode bias;
	in.to_unicode(bias, inlen);
	out.from_unicode(bias, oulen);
}

template<typename S, typename T>
void basic_encoding_conversion(const_tchar_pt<T> in, int inlen, tchar_pt<S> out, int oulen, int &inread, int &outread){
	unicode bias;
	inread = in.to_unicode(bias, inlen);
	outread = out.from_unicode(bias, oulen);
}



