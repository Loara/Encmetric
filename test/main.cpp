#include <encmetric.hpp>
#include <iostream>

using namespace adv;

template class const_tchar_pt<UTF8>;
template class const_tchar_pt<WIDEchr>;
template class tchar_pt<ISO_8859_1>;
template class tchar_pt<WIDE<byte>>;

int main(){
	adv_string_view<UTF16LE> ay{u"areèò"};
	adv_string_buf<UTF8> b{};
	b.append_string_c(ay);
	adv_string<UTF8> y = b.allocate<std::allocator<byte>>();
	const byte *p = y.data();
	const byte *q = ay.data();
	for(int i=0; i<10; i++){
		std::cout << (uint)(q[i]) << " ";
	}
	std::cout << std::endl;
	for(int i=0; i<7; i++){
		std::cout << (uint)(p[i]) << " ";
	}
	std::cout << std::endl;
}

