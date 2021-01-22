#include <encmetric/enc_strings.hpp>
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
}

