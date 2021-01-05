
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
void deduce_lens(const_tchar_pt<T> ptr, int &len, int &siz){
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
void deduce_lens(const_tchar_pt<T> ptr, int dim, bool issiz, int &len, int &siz){
	len = 0;
	siz = 0;
	int add;

	if(issiz){
		try{
			add = ptr.next();
			dim -= add;
			while(dim >= 0){
				siz += add;
				len++;
				if(dim == 0)
					break;
				add = ptr.next();
				dim -= add;
			}
		}
		catch(const encoding_error &){}
	}
	else{
		try{
			for(int i=0; i<dim; i++){
				add = ptr.next();
				siz += add;
				len++;
			}
		}
		catch(const encoding_error &){}
	}
}
/*
template<typename S, typename T>
int bytesOf(const_tchar_pt<S> c, int size, const_tchar_pt<T> sq, int nsiz){
	if(!sameEnc(c, sq))
		return -1;
	if(nsiz == 0)
		return 0;
	if(size < nsiz)
		return -1;
	int rem = size - nsiz +1;
	int byt = 0;
	while(byt < rem){
		if(compare(c.data(), sq.data(), nsiz)){
			return byt;
		}
		byt += c.next();
	}
	return -1;
}

template<typename S, typename T>
int indexOf(const_tchar_pt<S> c, int size, const_tchar_pt<T> sq, int nsiz){
	if(!sameEnc(c, sq))
		return -1;
	if(nsiz == 0)
		return 0;
	if(size < nsiz)
		return -1;
	int rem = size - nsiz +1;
	int byt = 0;
	int chr = 0;
	while(byt < rem){
		if(compare(c.data(), sq.data(), nsiz)){
			return chr;
		}
		byt += c.next();
		chr++;
	}
	return -1;
}
*/
//-----------------------
template<typename T>
adv_string_view<T>::adv_string_view(const_tchar_pt<T> cu) : ptr{cu}, len{0}, siz{0}{
	deduce_lens(cu, len, siz);
}

template<typename T>
adv_string_view<T>::adv_string_view(const_tchar_pt<T> cu, int dim, bool isdim) : ptr{cu}, len{0}, siz{0}{
	deduce_lens(cu, dim, isdim, len, siz);
}

template<typename T>
adv_string_view<T>::adv_string_view(const_tchar_pt<T> cu, int size, int lent) : ptr{cu}, len{0}, siz{0}{
	int add;
	if(size < 0 || lent < 0)
		throw encoding_error("Incorrect parameters");
	for(int i=0; i<lent; i++){
		add = cu.next();
		size -= add;
		siz += add;
		if(size < 0)
			throw encoding_error("Too small string");
	}
	len = lent;
}

template<typename T>
void adv_string_view<T>::verify() const{
	int remlen = siz;
	const_tchar_pt<T> mem{ptr};
	for(int i=0; i<len; i++){
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
int adv_string_view<T>::size(int a, int n) const{
	if(a < 0 || n < 0)
		throw std::out_of_range{"Out of range"};
	if(a+n > len)
		throw std::out_of_range{"Out of range"};
	if(n == 0)
		return 0;
	const_tchar_pt<T> mem = ptr+a;
	int ret = 0;
	for(int i=0; i<n; i++){
		ret += mem.next();
	}
	return ret;
}

template<typename T>
adv_string_view<T> adv_string_view<T>::substring(int b, int e, bool ign) const{
	if(ign)
		e = len;
	else if(e > len)
		e = len;
	if(b > e)
		b = e;
	const_tchar_pt<T> nei = ptr+b;
	int nlen = size(b, e-b);
	return adv_string_view<T>{nei, e - b, nlen};
}

template<typename T> template<typename S>
bool adv_string_view<T>::compstr(const adv_string_view<S> &t, int ch) const{
	if(!sameEnc(ptr, t.begin()))
		return false;
	int l1 = size(ch);
	int l2 = t.size(ch);
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
	int tsiz, tch;
	deduce_lens(t, tch, tsiz);
	if(siz != tsiz)
		return false;
	return compare(data(), t.data(), siz);
}

template<typename T> template<typename S>
int adv_string_view<T>::bytesOf(const adv_string_view<S> &sq) const{
	if(!sameEnc(ptr, sq.begin()))
		return -1;

	if(sq.size() == 0)
		return 0;
	if(siz < sq.size())
		return -1;
	int rem = siz - sq.size();
	int byt = 0;
	const_tchar_pt<T> newi = ptr;
	while(byt <= rem){
		if(compare(newi.data(), sq.begin().data(), sq.size())){
			return byt;
		}
		byt += newi.next();
	}
	return -1;
}

template<typename T> template<typename S>
int adv_string_view<T>::indexOf(const adv_string_view<S> &sq) const{
	if(!sameEnc(ptr, sq.begin()))
		return -1;

	if(sq.size() == 0)
		return 0;
	if(siz < sq.size())
		return -1;
	int rem = siz - sq.size();
	int byt = 0;
	int chr = 0;
	const_tchar_pt<T> newi = ptr;
	while(byt <= rem){
		if(compare(newi.data(), sq.begin().data(), sq.size())){
			return chr;
		}
		byt += newi.next();
		chr++;
	}
	return -1;
}

template<typename T> template<typename S>
bool adv_string_view<T>::containsChar(const_tchar_pt<S> cu) const{
	if(!sameEnc(ptr, cu))
		return false;

	int chl = cu.chLen();
	if(siz < chl)
		return -1;
	int rem = siz - chl;
	int byt = 0;
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
adv_string_view<S> adv_string_view<T>::basic_encoding_conversion(tchar_pt<S> buffer, int blen) const{
	int remby = siz;
	int sadby = blen;
	int byread;
	int bywrite;
	const_tchar_pt<T> read = ptr;
	tchar_pt<S> write = buffer;
	for(int i=0; i<len; i++){
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
adv_string<S, U> adv_string_view<T>::basic_encoding_conversion(const EncMetric &format, const U &alloc, enable_widenc_t<S, int>) const{
	basic_ptr<byte, U> temp{(std::size_t)len, alloc};
	const_tchar_pt<T> from = ptr;
	tchar_pt<S> to{temp.memory, format};
	const_tchar_pt<S> new_to = to.new_instance(temp.memory).cast();
	if(len == 0)
		return adv_string<S, U>{new_to, 0, 0, std::move(temp)};
	int remsiz = temp.dimension;
	int newsiz = 0;
	int input_len = siz;
	for(int i=0; i<len; i++){
		if(input_len <= 0)
			throw encoding_error();
		unicode uni;
		int read = from.to_unicode(uni, input_len);
		int write = to.from_unicode(uni, remsiz);
		while(write == 0){
			int tdim = temp.dimension;
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
adv_string<S, U> adv_string_view<T>::basic_encoding_conversion(const U &alloc, enable_not_widenc_t<S, int>) const{
	basic_ptr<byte, U> temp{(std::size_t)len, alloc};
	const_tchar_pt<T> from = ptr;
	tchar_pt<S> to{temp.memory};
	const_tchar_pt<S> new_to = to.new_instance(temp.memory).cast();
	if(len == 0)
		return adv_string<S, U>{new_to, 0, 0, std::move(temp)};
	int remsiz = temp.dimension;
	int newsiz = 0;
	int input_len = siz;
	for(int i=0; i<len; i++){
		if(input_len <= 0)
			throw encoding_error();
		unicode uni;
		int read = from.to_unicode(uni, input_len);
		int write = to.from_unicode(uni, remsiz);
		while(write == 0){
			int tdim = temp.dimension;
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
	int esiz = err.size();
	int elen = err.length();
	const byte *buf1 = data();
	const byte *buf2 = err.data();
	basic_ptr<byte, U> allocater{(std::size_t)(siz+esiz), alloc};
	byte *buf = allocater.memory;
	for(int i=0; i<siz; i++)
		buf[i] = buf1[i];
	for(int j=0; j<esiz; j++)
		buf[j + siz] = buf2[j];
	return adv_string<T, U>{std::move(allocater), ptr, len+elen, siz+esiz};
}
//------------

template<typename T, typename U>
adv_string<T, U>::adv_string(const_tchar_pt<T> ptr, int len, int siz, basic_ptr<byte, U> by) : adv_string_view<T>{len, siz, ptr}, bind{std::move(by)} {}

//stavolta ignora la memoria puntata da ptr
template<typename T, typename U>
adv_string<T, U>::adv_string(basic_ptr<byte, U> by, const_tchar_pt<T> ptr, int len, int siz) : adv_string_view<T>{len, siz, ptr.new_instance(by.memory)}, bind{std::move(by)} {}
/*
template<typename T, typename U>
adv_string<T, U>::adv_string(basic_ptr<byte, U> by, const_tchar_pt<T> frm, bool stopzero) : 
	adv_string_view<T>{frm.new_instance(by.memory),
	 static_cast<int>(by.dimension), stopzero},
	 bind{std::move(by)} {}


template<typename T, typename U>
adv_string<T, U>::adv_string(const_tchar_pt<T> b, int dim, bool stopzero, const U &alloc) : adv_string{basic_ptr<byte, U>{b.data(), static_cast<std::size_t>(dim), alloc}, b, stopzero} {}
*/
template<typename T, typename U>
adv_string<T, U>::adv_string(const adv_string_view<T> &st, const U &alloc)
	 : adv_string{basic_ptr<byte, U>{st.begin().data(), (std::size_t)st.size(), alloc}, st.begin(), st.length(), st.size()} {}

template<typename T, typename U>
adv_string<T, U> adv_string<T, U>::newinstance(const_tchar_pt<T> pt, const U &alloc){
	int len=0, siz=0;
	deduce_lens(pt, len, siz);
	return adv_string<T, U>{basic_ptr<byte, U>{pt.data(), (std::size_t)siz, alloc}, pt, len, siz};
}



