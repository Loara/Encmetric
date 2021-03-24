#include <encmetric.hpp>
#include <iostream>

using namespace adv;

int main(){
	UTF16BE rout{};
	UTF32LE rin{};

	astr_view res{"Ciao ciao   bambino"};
	aout << res << std::endl;
	astr input = ain.read_enc_string();
	aout << input << " " << input.size() << " " << input.length() << std::endl;
}

