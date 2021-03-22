
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

template<typename T, typename tt>
bool sameEnc(const const_tchar_pt<T> &, const const_tchar_pt<WIDE<tt>> &a){
	const EncMetric<tt> &f = a.format();
	return f.index() == index_traits<T>::index();
}

template<typename T, typename tt>
bool sameEnc(const const_tchar_pt<WIDE<tt>> &a, const const_tchar_pt<T> &){
	const EncMetric<tt> &f = a.format();
	return f.index() == index_traits<T>::index();
}

template<typename tt>
bool sameEnc(const const_tchar_pt<WIDE<tt>> &a, const const_tchar_pt<WIDE<tt>> &b){
	const EncMetric<tt> &f = a.format();
	const EncMetric<tt> &g = b.format();
	return f.index() == g.index();
}

template<typename S, typename T>
tchar_pt<S> convert(tchar_pt<T> p){
	if constexpr( std::is_same_v<S, T> ){
		return p;
	}
	else if constexpr( is_wide_v<S> ){
		return tchar_pt<S>{p.data(), T::instance()};
	}
	else if constexpr( is_wide_v<T> ){
		if(index_traits<S>::index() != p.format().index())
			throw encoding_error("Impossible to convert these strings");
		return tchar_pt<S>(p.data());
	}
	else{
		if(index_traits<S>::index() == index_traits<T>::index())
			return tchar_pt<S>{p.data()};
		throw encoding_error("Impossible to convert these strings");
	}
}

template<typename S, typename T>
const_tchar_pt<S> convert(const_tchar_pt<T> p){
	if constexpr( std::is_same_v<S, T> ){
		return p;
	}
	else if constexpr( is_wide_v<S> ){
		return const_tchar_pt<S>{p.data(), T::instance()};
	}
	else if constexpr( is_wide_v<T> ){
		if(index_traits<S>::index() != p.format().index())
			throw encoding_error("Impossible to convert these strings");
		return const_tchar_pt<S>(p.data());
	}
	else{
		if(index_traits<S>::index() == index_traits<T>::index())
			return const_tchar_pt<S>{p.data()};
		throw encoding_error("Impossible to convert these strings");
	}
}

template<typename S, typename T, enable_same_data_t<S, T, int> =0>
void basic_encoding_conversion(const_tchar_pt<T> in, int inlen, tchar_pt<S> out, int oulen){
	typename S::ctype bias;
	in.decode(&bias, inlen);
	out.encode(bias, oulen);
}

template<typename S, typename T, enable_same_data_t<S, T, int> =0>
void basic_encoding_conversion(const_tchar_pt<T> in, int inlen, tchar_pt<S> out, int oulen, int &inread, int &outread){
	typename S::ctype bias;
	inread = in.decode(&bias, inlen);
	outread = out.encode(bias, oulen);
}



