#include <encmetric.hpp>
#include <iostream>

using namespace adv;

int main(){
	Win_1252 a;
	byte buf[10];
	astr_view res{"Còao ciao   bambino"};
	c_achar_pt in = res.begin();
	iochar_pt day{buf}, dayo{buf};
	basic_encoding_conversion(in, 5, day, 5);
	in.next();
	day.next();
	basic_encoding_conversion(in, 5, day, 5);
	in.next();
	day.next();
	basic_encoding_conversion(in, 5, day, 5);
	size_t ch = stdout_putChrs(dayo.cast(), 3);
	std::cout << ch << std::endl;
}

