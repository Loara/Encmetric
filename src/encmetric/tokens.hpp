#pragma once
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
#include <encmetric/utf_string.hpp>

namespace adv{

template<typename T>
class Token{
	private:
		const_tchar_pt<T> s, e, end;
	public:
		Token(adv_string_view<T> main) : s{main.begin()}, e{main.begin()}, end{main.end()} {}
		bool eof() const noexcept {return s == end;}
		void flush() noexcept {s = e;}
		//le funzioni che ritornano bool ritornano false se e solo se l'incremento fallisce (ad esempio se e==end)
		bool step() noexcept {
			if(e == end)
				return false;
			++e;
			return true;
		}
		adv_string_view<T> share() const noexcept {return adv_string_view<T>(s, e-s, true);}
		//interrompe quando incontra un carattere nella stringa, Se trova il carattere lo mette in e. Se già e contiene il carattere allora ritorna automaticamente
		bool goUp(const adv_string_view<T> &delim) noexcept{
			if(e == end)
				return false;
			while(!delim.containsChar(e)){
				++e;
				if(e == end)
					return false;
			}
			return true;
		}
		//interrompe quando incontra un carattere NON nella stringa, Se trova il carattere lo mette in e. Se già e contiene il carattere allora ritorna automaticamente
		bool goUntil(const adv_string_view<T> &delim) noexcept{
			if(e == end)
				return false;
			while(delim.containsChar(e)){
				++e;
				if(e == end)
					return false;
			}
			return true;
		}
		adv_string_view<T> proceed(const adv_string_view<T> &delim) noexcept{
			flush();
			goUp(delim);
			adv_string_view<T> ret = share();
			goUntil(delim);
			return ret;
		}
};

}
