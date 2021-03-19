#include <encmetric.hpp>
#include <encmetric/tokens.hpp>
#include <iostream>

using namespace adv;

using atok = Token<CENC>;

int main(){
	UTF16BE rout{};
	UTF32LE rin{};

	astr_view res{"Ciao ciao   bambino"};
	wstr_view rey{"Hi", DynEncoding<UTF8>::instance()};
	astr_view del = getstring(" ");
	astr copy = res;
	atok t{res};
	while(!t.eof())
		std::cout << t.proceed(del) << std::endl;
	byte b;
	ISO_8859_2::from_unicode(0x13a, &b, 1);
	std::cout << std::hex << to_integer<int>(b) << std::endl;
	ISO_8859_2::from_unicode(0x1a, &b, 1);
	std::cout << std::hex << to_integer<int>(b) << std::endl;
	ISO_8859_2::from_unicode(0x163, &b, 1);
	std::cout << std::hex << to_integer<int>(b) << std::endl;
	b = byte{0xec};
	unicode u;
	ISO_8859_2::to_unicode(u, &b, 1);
	std::cout << std::hex << u << std::endl;
}

