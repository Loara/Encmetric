
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
bool sameEnc(const const_tchar_pt<S> &a) noexcept{
	static_assert(!is_wide_v<T>, "Template parameter cannot be wide");
	if constexpr(!is_wide_v<S>)
		return sameEnc_static<S, T>;
	else{
		const EncMetric<typename S::ctype> &f = a.format();
		return f.index() == index_traits<T>::index();
	}
}

template<typename S, typename T>
bool sameEnc(const const_tchar_pt<S> &, const const_tchar_pt<T> &) noexcept{
	static_assert(!is_wide_v<S>, "encodings on different ctypes");
	static_assert(!is_wide_v<T>, "encodings on different ctypes");
	return sameEnc_static<S, T>;
}

template<typename T>
bool sameEnc(const const_tchar_pt<T> &, const const_tchar_pt<WIDE<typename T::ctype>> &a) noexcept{
	return sameEnc<T>(a);
}

template<typename T>
bool sameEnc(const const_tchar_pt<WIDE<typename T::ctype>> &a, const const_tchar_pt<T> &) noexcept{
	return sameEnc<T>(a);
}

template<typename tt>
bool sameEnc(const const_tchar_pt<WIDE<tt>> &a, const const_tchar_pt<WIDE<tt>> &b) noexcept{
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
void basic_encoding_conversion(const_tchar_pt<T> in, uint inlen, tchar_pt<S> out, uint oulen){
	typename S::ctype bias;
	in.decode(&bias, inlen);
	out.encode(bias, oulen);
}

template<typename S, typename T, enable_same_data_t<S, T, int> =0>
void basic_encoding_conversion(const_tchar_pt<T> in, uint inlen, tchar_pt<S> out, uint oulen, uint &inread, uint &outread){
	typename S::ctype bias;
	inread = in.decode(&bias, inlen);
	outread = out.encode(bias, oulen);
}

template<typename T>
uint min_size_estimate(const_tchar_pt<T> ptr, uint nchr) noexcept{
	if constexpr(!is_wide_v<T>)
		return min_length<T>(nchr);
	else
		return min_length(nchr, ptr.format());
}
template<typename T>
uint max_size_estimate(const_tchar_pt<T> ptr, uint nchr){
	if constexpr(!is_wide_v<T>)
		return max_length<T>(nchr);
	else
		return max_length(nchr, ptr.format());
}

template<typename T>
bool dynamic_fixed_size(const_tchar_pt<T> ptr) noexcept{
	if constexpr(is_wide_v<T>)
		return ptr.format().d_fixed_size();
	else
		return fixed_size<T>;
}



