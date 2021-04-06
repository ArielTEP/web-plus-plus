#include "Utils.h"


std::string Utils::b2hex(const char* in, size_t len)
{
	std::string hex = "0123456789ABCDEF";
	std::string ans;

	for (int i = 0; i < (int)len; i++)
	{
		ans.push_back(hex[in[i] >> 4]);
		ans.push_back(hex[in[i] & 0x0F]);
		ans += " ";
	}

	return ans;
}