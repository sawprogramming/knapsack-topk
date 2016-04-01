#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <string>
#include <numeric>
#include <algorithm>

class StringUtil {
public:
	static int ldistance(const std::string &s1, const std::string &s2);
};

#endif