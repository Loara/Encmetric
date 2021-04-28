#include <encmetric/enc_io.hpp>
using namespace adv;

size_t adv::stdin_getChrs(iochar_pt ptr, size_t len){
	if(len == 0)
		return 0;
	size_t siz = 0;
	size_t chrRead = 0;
	bool eof;

    uint unity = ptr.unity();
	do{
		siz = raw_stdin_readbytes(ptr.data(), (len - chrRead) * unity);
		eof = siz < (len - chrRead);//less character than required
		while(siz > 0){
            if(siz < unity){
                if(raw_stdin_readbytes((ptr+siz).data(), unity - siz) < (unity - siz))
                    throw incorrect_encoding{"Incomplete character"};
                siz = unity;
            }
			size_t dwsiz = ptr.chLen();
			if(dwsiz > siz){
				if(raw_stdin_readbytes((ptr+siz).data(), dwsiz - siz) < (dwsiz - siz))
					throw incorrect_encoding{"Incomplete character"};
				siz = dwsiz;
			}
			siz -= dwsiz;
			if(!ptr.valid_next(siz)){
                throw incorrect_encoding{"Invalid character encoding"};
            }
			chrRead++;	
		}
	}
	while(!eof && chrRead != len);
	return chrRead;
}

size_t adv::stdin_getChrs_validate(iochar_pt ptr, size_t len){
	if(len == 0)
		return 0;
	size_t siz = 0;
	size_t chrRead = 0;
	bool eof;

    uint unity = ptr.unity();
	do{
		siz = raw_stdin_readbytes(ptr.data(), (len - chrRead) * unity);
		eof = siz < (len - chrRead);//less character than required
		while(siz > 0){
            if(siz < unity){
                if(raw_stdin_readbytes((ptr+siz).data(), unity - siz) < (unity - siz))
                    throw incorrect_encoding{"Incomplete character"};
                siz = unity;
            }
			size_t dwsiz = ptr.chLen();
			if(dwsiz > siz){
				if(raw_stdin_readbytes((ptr+siz).data(), dwsiz - siz) < (dwsiz - siz))
					throw incorrect_encoding{"Incomplete character"};
				siz = dwsiz;
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
	size_t siz = 0;
	size_t chrWrite = 0;
	bool eof;

    uint unity = ptr.unity();
	do{
		siz = raw_stdout_writebytes(ptr.data(), (len - chrWrite) * unity);
		if(siz < 0)
			throw encoding_error{"IO error"};
		eof = siz < (len - chrWrite);//less character than required
		while(siz > 0){
            if(siz < unity){
                if(raw_stdout_writebytes((ptr+siz).data(), unity - siz) < (unity - siz))
                    throw incorrect_encoding{"Incomplete character"};
                siz = unity;
            }
			size_t dwsiz = ptr.chLen();
			if(dwsiz > siz){
				if(raw_stdout_writebytes((ptr+siz).data(), dwsiz - siz) < (dwsiz - siz))
					throw incorrect_encoding{"Incomplete character"};
				siz = dwsiz;
			}
			siz -= dwsiz;
			ptr.next();
			chrWrite++;
		}
	}
	while(!eof && chrWrite != len);
	return chrWrite;
}
/*
{
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
*/
size_t adv::stderr_putChrs(c_iochar_pt ptr, size_t len){
	if(len == 0)
		return 0;
	size_t siz = 0;
	size_t chrWrite = 0;
	bool eof;

    uint unity = ptr.unity();
	do{
		siz = raw_stderr_writebytes(ptr.data(), (len - chrWrite) * unity);
		eof = siz < (len - chrWrite);//less character than required
		while(siz > 0){
            if(siz < unity){
                if(raw_stderr_writebytes((ptr+siz).data(), unity - siz) < (unity - siz))
                    throw incorrect_encoding{"Incomplete character"};
                siz = unity;
            }
			size_t dwsiz = ptr.chLen();
			if(dwsiz > siz){
				if(raw_stderr_writebytes((ptr+siz).data(), dwsiz - siz) < (dwsiz - siz))
					throw incorrect_encoding{"Incomplete character"};
				siz = dwsiz;
			}
			siz -= dwsiz;
			ptr.next();
			chrWrite++;
		}
	}
	while(!eof && chrWrite != len);
	return chrWrite;
}
