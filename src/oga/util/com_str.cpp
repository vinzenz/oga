#include <oga/util/com_str.hpp>
#include <oga/util/wmiclient.hpp>
namespace oga {
namespace util {
#if defined(_WIN32)
com_str::com_str(){

}
com_str::com_str(utf8_string const & s)
: bstr_(_com_util::ConvertStringToBSTR(s.c_str()))
{
}

com_str::com_str(utf16_string const & s) 
: bstr_(SysAllocString(s.c_str()))
{
}

com_str::com_str(BSTR b) 
: bstr_(b) 
{

}
com_str::~com_str() {

}

utf8_string com_str::to_utf8() const {
	utf8_string ret;
	if (bstr_) {
		char * str = _com_util::ConvertBSTRToString(bstr_);
		ret = str;
		delete[] str;
	}
	return ret;
}

utf16_string com_str::to_utf16() const {
	utf16_string ret;
	if (bstr_) ret.assign(bstr_);
	return ret;
}

BSTR com_str::get() const {
	return bstr_;
}

BSTR * com_str::attach_to() {
	return &bstr_;
}

void com_str::attach(BSTR str) {
	release();
	bstr_ = str;
}

BSTR com_str::detach() {
	BSTR ret = bstr_;
	bstr_ = 0;
	return ret;
}

bool com_str::empty() const {
	return bstr_ == 0;
}

void com_str::release() {
	if (bstr_ != 0) {
		::SysFreeString(detach());
	}
}
#endif
}}
