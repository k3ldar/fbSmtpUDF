#include <string>

#ifndef _BASE64_H_
#define _BASE64_H_

#ifdef _WIN64
#pragma warning(disable:4267)
#endif

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

#endif