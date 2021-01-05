#include <encmetric/enc_strings.hpp>
#include <encmetric/tokens.hpp>
#include <iostream>

using namespace adv;

using atok = Token<CENC>;

int main(){
	astr_view res = getstring("Ciao ciao   bambino");
	astr_view del = getstring(" ");
	atok t{res};
	while(!t.eof())
		std::cout << t.proceed(del) << std::endl;
}

