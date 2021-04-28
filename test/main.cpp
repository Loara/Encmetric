#include <encmetric.hpp>
#include <iostream>

using namespace adv;

template class const_tchar_pt<UTF8>;
template class const_tchar_pt<WIDEchr>;
template class tchar_pt<ISO_8859_1>;
template class tchar_pt<WIDE<byte>>;
template class adv_string_buf<UTF8>;

int main(){
    adv_string_view<UTF8> i = "Hello"_asv;
	adv_string_view<UTF16LE> ay = getstring_16(u"areèò主");
	adv_string_buf<UTF8> b = new_str_buf<UTF8>();
	b.append_string_c(ay);
    b << " "_asv << i;
	adv_string<UTF8> y = b.allocate<std::pmr::polymorphic_allocator<byte>>();

    stdout_putStr(y);
    stdout_putStr(i);
	std::cout << std::endl;
}

