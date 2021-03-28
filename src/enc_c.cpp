#define encmetric_library 1
#include <encmetric/enc_c.hpp>

using namespace adv;

template class adv::tchar_pt<CENC>;
template class adv::const_tchar_pt<CENC>;
template class adv::adv_string_view<CENC>;
template class adv::adv_string<CENC>;
