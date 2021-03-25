
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
template<typename T>
void deduce_lens(const_tchar_pt<T> ptr, size_t &len, size_t &siz){
	len=0;
	siz=0;
	int add;

	while(!ptr.terminate()){
		add = ptr.next();
		siz += add;
		len++;
	}
}

template<typename T>
void deduce_lens(const_tchar_pt<T> ptr, size_t dim, meas measure, size_t &len, size_t &siz){
	len = 0;
	siz = 0;
	bool issiz = measure == meas::size;
	if constexpr(fixed_size<T>){
		if(issiz){
			len = dim / T::unity();
		}
		else{
			len = dim;
		}
		siz = len * T::unity();
	}
	else{
		int add;
		size_t oldim;

		if(issiz){
			try{
				add = ptr.next();
				oldim = dim;
				dim -= add;
				while(dim < oldim){//Overflow test for unsigned integer
					siz += add;
					len++;
					if(dim == 0)
						break;
					add = ptr.next();
					oldim = dim;
					dim -= add;
				}
			}
			catch(const encoding_error &){}
		}
		else{
			try{
				for(size_t i=0; i<dim; i++){
					add = ptr.next();
					siz += add;
					len++;
				}
			}
			catch(const encoding_error &){}
		}
	}
}
//-----------------------
template<typename T>
adv_string_view<T>::adv_string_view(const_tchar_pt<T> cu) : ptr{cu}, len{0}, siz{0}{
	deduce_lens(cu, len, siz);
}

template<typename T>
adv_string_view<T>::adv_string_view(const_tchar_pt<T> cu, size_t dim, meas isdim) : ptr{cu}, len{0}, siz{0}{
	deduce_lens(cu, dim, isdim, len, siz);
}

template<typename T>
adv_string_view<T>::adv_string_view(const_tchar_pt<T> cu, size_t size, size_t lent) : ptr{cu}, len{0}, siz{0}{
	if constexpr(fixed_size<T>){
		if(size / T::unity() < lent)
			throw encoding_error("Too small string");//prevent integer overflow due to multiplication
		len = lent;
		siz = lent * T::unity(); //may be siz < size
	}
	else{
		int add;
		size_t olsiz;
		for(size_t i=0; i<lent; i++){
			add = cu.next();
			olsiz = size;
			size -= add;
			siz += add;
			if(size >= olsiz)
				throw encoding_error("Too small string");
		}
		len = lent;
	}
}

template<typename T>
void adv_string_view<T>::verify() const{
	size_t remlen = siz;
	const_tchar_pt<T> mem{ptr};
	for(size_t i=0; i<len; i++){
		if(!mem.valid_next(remlen))
			throw encoding_error("Invalid string encoding");
	}
	//La lunghezza deve essere esatta
	if(remlen != 0)
		throw encoding_error("Invalid string encoding");
}

template<typename T>
bool adv_string_view<T>::verify_safe() const noexcept{
	try{
		verify();
		return true;
	}
	catch(...){
		return false;
	}
}

template<typename T>
const_tchar_pt<T> adv_string_view<T>::at(size_t chr) const{
	if(chr > len)
		throw std::out_of_range{"Out of range"};
	if(chr == 0)
		return ptr;
	if constexpr(fixed_size<T>){
		return ptr + (chr * T::unity());
	}
	else{
		const_tchar_pt<T> ret = ptr;
		if(chr == len)
			return ret + siz;
		for(size_t i=0; i< chr; i++)
			ret.next();
		return ret;
	}
};

template<typename T>
size_t adv_string_view<T>::size(size_t a, size_t n) const{
	if(a+n < n || a+n > len)
		throw std::out_of_range{"Out of range"};
	if(n == 0)
		return 0;
	if constexpr (fixed_size<T>){
		return n * T::unity();
	}
	else{
		const_tchar_pt<T> mem = ptr;
		for(size_t i=0; i<a; i++)
			mem.next();
		size_t ret = 0;
		for(size_t i=0; i<n; i++){
			ret += mem.next();
		}
		return ret;
	}
}

template<typename T>
adv_string_view<T> adv_string_view<T>::substring(size_t b, size_t e, bool ign) const{
	if(ign)
		e = len;
	else if(e > len)
		e = len;
	if(b > e)
		b = e;
	if constexpr(fixed_size<T>){
		const_tchar_pt<T> nei = ptr + (b * T::unity());
		return adv_string_view<T>{e-b, (e-b) * T::unity(), nei};
	}
	else{
		const_tchar_pt<T> nei = ptr;
		for(size_t i=0; i<b; i++)
			nei.next();
		size_t nlen = 0;
		const_tchar_pt<T> temp = nei;
		for(size_t i=0; i<(e-b); i++)
			nlen += temp.next();
		return adv_string_view<T>{e - b, nlen, nei};
	}
}

template<typename T> template<typename S>
bool adv_string_view<T>::compstr(const adv_string_view<S> &t, size_t ch) const{
	if(!sameEnc(ptr, t.begin()))
		return false;
	size_t l1 = size(ch);
	size_t l2 = t.size(ch);
	if(l1 != l2)
		return false;
	return compare(data(), t.data(), l1);
}

template<typename T> template<typename S>
bool adv_string_view<T>::operator==(const adv_string_view<S> &t) const{
	if(!sameEnc(ptr, t.begin()))
		return false;
	if(siz != t.size())
		return false;
	return compare(data(), t.data(), siz);
}

template<typename T> template<typename S>
bool adv_string_view<T>::operator==(const_tchar_pt<S> t) const{
	if(!sameEnc(ptr, t))
		return false;
	size_t tsiz, tch;
	deduce_lens(t, tch, tsiz);
	if(siz != tsiz)
		return false;
	return compare(data(), t.data(), siz);
}

template<typename T> template<typename S>
size_t adv_string_view<T>::bytesOf(const adv_string_view<S> &sq, bool &found) const{
	if(!sameEnc(ptr, sq.begin())){
		found = false;
		return 0;
	}

	if(sq.size() == 0){
		found = true;
		return 0;
	}
	if(siz < sq.size()){
		found = false;
		return 0;
	}
	size_t rem = siz - sq.size();
	size_t byt = 0;
	const_tchar_pt<T> newi = ptr;
	while(byt <= rem){
		if(compare(newi.data(), sq.begin().data(), sq.size())){
			found = true;
			return byt;
		}
		byt += newi.next();
	}
	found = false;
	return 0;
}

template<typename T> template<typename S>
size_t adv_string_view<T>::indexOf(const adv_string_view<S> &sq, bool &found) const{
	if(!sameEnc(ptr, sq.begin())){
		found = false;
		return 0;
	}

	if(sq.size() == 0){
		found = true;
		return 0;
	}
	if(siz < sq.size()){
		found = false;
		return 0;
	}
	size_t rem = siz - sq.size();
	size_t byt = 0;
	size_t chr = 0;
	const_tchar_pt<T> newi = ptr;
	while(byt <= rem){
		if(compare(newi.data(), sq.begin().data(), sq.size())){
			found = true;
			return chr;
		}
		byt += newi.next();
		chr++;
	}
	found = false;
	return 0;
}

template<typename T> template<typename S>
bool adv_string_view<T>::containsChar(const_tchar_pt<S> cu) const{
	if(!sameEnc(ptr, cu))
		return false;

	size_t chl = cu.chLen();
	if(siz < chl)
		return false;
	size_t rem = siz - chl;
	size_t byt = 0;
	const_tchar_pt<T> newi = ptr;
	while(byt <= rem){
		if(compare(newi.data(), cu.data(), chl)){
			return true;
		}
		byt += newi.next();
	}
	return false;
}

template<typename T>
template<typename S>
adv_string_view<S> adv_string_view<T>::basic_encoding_conversion(tchar_pt<S> buffer, size_t blen) const{
	size_t remby = siz;
	size_t sadby = blen;
	size_t byread;
	size_t bywrite;
	const_tchar_pt<T> read = ptr;
	tchar_pt<S> write = buffer;
	for(size_t i=0; i<len; i++){
		basic_encoding_conversion(read, remby, write, sadby, byread, bywrite);
		++read;
		++write;
		remby -= byread;
		sadby -= bywrite;
	}
	return adv_string_view<S>(buffer, blen-sadby);
}

template<typename T>
template<typename S, typename U>
adv_string<S, U> adv_string_view<T>::basic_encoding_conversion(const EncMetric<typename S::ctype> &format, const U &alloc, enable_wide_t<S, int>) const{
	basic_ptr<byte, U> temp{len, alloc};
	const_tchar_pt<T> from = ptr;
	tchar_pt<S> to{temp.memory, format};
	const_tchar_pt<S> new_to = to.new_instance(temp.memory).cast();
	if(len == 0)
		return adv_string<S, U>{new_to, 0, 0, std::move(temp)};
	size_t remsiz = temp.dimension;
	size_t newsiz = 0;
	size_t input_len = siz;
	for(size_t i=0; i<len; i++){
		if(input_len <= 0)
			throw encoding_error();
		unicode uni;
		int read = from.to_unicode(uni, input_len);
		int write = to.from_unicode(uni, remsiz);
		while(write == 0){
			size_t tdim = temp.dimension;
			byte *old_pr = temp.memory;
			remsiz += 3*tdim;
			temp = reallocate(std::move(temp), 4*tdim);
			new_to = to.new_instance(temp.memory).cast();
			to = to.transfer_to(old_pr, temp.memory);
			write = to.from_unicode(uni, remsiz);
		}	
		input_len -= read;
		remsiz -= write;
		newsiz += write;
		++from;
		++to;
	}
	return adv_string<S, U>{new_to, len, newsiz, std::move(temp)};
}

template<typename T>
template<typename S, typename U>
adv_string<S, U> adv_string_view<T>::basic_encoding_conversion(const U &alloc, enable_not_wide_t<S, int>) const{
	basic_ptr<byte, U> temp{len, alloc};
	const_tchar_pt<T> from = ptr;
	tchar_pt<S> to{temp.memory};
	const_tchar_pt<S> new_to = to.new_instance(temp.memory).cast();
	if(len == 0)
		return adv_string<S, U>{new_to, 0, 0, std::move(temp)};
	size_t remsiz = temp.dimension;
	size_t newsiz = 0;
	size_t input_len = siz;
	for(size_t i=0; i<len; i++){
		if(input_len <= 0)
			throw encoding_error();
		unicode uni;
		int read = from.to_unicode(uni, input_len);
		int write = to.from_unicode(uni, remsiz);
		while(write == 0){
			size_t tdim = temp.dimension;
			byte *old_pr = temp.memory;
			remsiz += 3*tdim;
			temp = reallocate(std::move(temp), 4*tdim);
			new_to = to.new_instance(temp.memory).cast();
			to = to.transfer_to(old_pr, temp.memory);
			write = to.from_unicode(uni, remsiz);
		}	
		input_len -= read;
		remsiz -= write;
		newsiz += write;
		++from;
		++to;
	}
	return adv_string<S, U>{new_to, len, newsiz, std::move(temp)};
}

template<typename T>
template<typename S, typename U>
adv_string<T, U> adv_string_view<T>::concatenate(const adv_string_view<S> &err, const U &alloc) const{
	if(!sameEnc(*this, err))
		throw encoding_error("Not same encoding");
	size_t esiz = err.size();
	size_t elen = err.length();
	const byte *buf1 = data();
	const byte *buf2 = err.data();
	basic_ptr<byte, U> allocater{(std::size_t)(siz+esiz), alloc};
	byte *buf = allocater.memory;
	for(size_t i=0; i<siz; i++)
		buf[i] = buf1[i];
	for(size_t j=0; j<esiz; j++)
		buf[j + siz] = buf2[j];
	return adv_string<T, U>{std::move(allocater), ptr, len+elen, siz+esiz};
}
//------------

template<typename T, typename U>
adv_string<T, U>::adv_string(const_tchar_pt<T> ptr, size_t len, size_t siz, basic_ptr<byte, U> by) : adv_string_view<T>{len, siz, ptr}, bind{std::move(by)} {}

//ignore the memory pointed bu ptr, use the memory pointed by by
template<typename T, typename U>
adv_string<T, U>::adv_string(const_tchar_pt<T> ptr, size_t len, size_t siz, basic_ptr<byte, U> by, [[maybe_unused]] int ignore) : adv_string_view<T>{len, siz, ptr.new_instance(by.memory)}, bind{std::move(by)} {}


template<typename T, typename U>
adv_string<T, U>::adv_string(const adv_string_view<T> &st, const U &alloc)
	 : adv_string{st.begin(), st.length(), st.size(), basic_ptr<byte, U>{st.begin().data(), (std::size_t)st.size(), alloc}, 0} {}

template<typename T, typename U>
adv_string<T, U> adv_string<T, U>::newinstance(const_tchar_pt<T> pt, const U &alloc){
	size_t len=0, siz=0;
	deduce_lens(pt, len, siz);
	return adv_string<T, U>{pt, len, siz, basic_ptr<byte, U>{pt.data(), (std::size_t)siz, alloc}, 0};
}



