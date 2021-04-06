#pragma once
#include <string>

#define LOG_TRACE(data,size) \
				{\
					std::cout << "\n" << \
					Utils::b2hex(data, size) \
					<< "\t" << data << "\n"; \
				}


class Utils
{
public:
	static std::string b2hex(const char *in, size_t len);
};
