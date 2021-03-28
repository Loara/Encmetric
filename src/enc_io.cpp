#include <encmetric/enc_io.hpp>
#include <encmetric/enc_io.hpp>
using namespace adv;

size_t adv::stdin_getChrs(iochar_pt ptr, size_t len){
	if(len == 0)
		return 0;
	ssize_t siz = 0;
	size_t chrRead = 0;
	bool eof;
	do{
		siz = raw_stdin_readbytes(ptr.data(), len - chrRead);
		if(siz < 0)
			throw encoding_error{"IO error"};
		eof = siz < (len - chrRead);//less character than required
		while(siz > 0){
			size_t dwsiz = ptr.chLen();
			if(dwsiz > siz){
				ptr += siz;
				//read half-character
				if(raw_stdin_readbytes(ptr.data(), dwsiz - siz) < (dwsiz - siz))
					throw encoding_error{"Incomplete character"};
				chrRead++;
				break;
			}
			siz -= dwsiz;
			ptr.next();
			chrRead++;	
		}
	}
	while(!eof && chrRead != len);
	return chrRead;
}

size_t adv::stdout_putChrs(c_iochar_pt ptr, size_t len){
	if(len == 0)
		return 0;
	ssize_t siz = 0;
	size_t chrRead = 0;
	bool eof;
	do{
		siz = raw_stdout_writebytes(ptr.data(), len - chrRead);
		if(siz < 0)
			throw encoding_error{"IO error"};
		eof = siz < (len - chrRead);//less character than required
		while(siz > 0){
			size_t dwsiz = ptr.chLen();
			if(dwsiz > siz){
				ptr += siz;
				//read half-character
				if(raw_stdout_writebytes(ptr.data(), dwsiz - siz) < (dwsiz - siz))
					throw encoding_error{"Incomplete character"};
				chrRead++;
				break;
			}
			siz -= dwsiz;
			ptr.next();
			chrRead++;	
		}
	}
	while(!eof && chrRead != len);
	return chrRead;
}
size_t adv::stderr_putChrs(c_iochar_pt ptr, size_t len){
	if(len == 0)
		return 0;
	ssize_t siz = 0;
	size_t chrRead = 0;
	bool eof;
	do{
		siz = raw_stderr_writebytes(ptr.data(), len - chrRead);
		if(siz < 0)
			throw encoding_error{"IO error"};
		eof = siz < (len - chrRead);//less character than required
		while(siz > 0){
			size_t dwsiz = ptr.chLen();
			if(dwsiz > siz){
				ptr += siz;
				//read half-character
				if(raw_stderr_writebytes(ptr.data(), dwsiz - siz) < (dwsiz - siz))
					throw encoding_error{"Incomplete character"};
				chrRead++;
				break;
			}
			siz -= dwsiz;
			ptr.next();
			chrRead++;	
		}
	}
	while(!eof && chrRead != len);
	return chrRead;
}
