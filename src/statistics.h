#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include <vector>
#include <algorithm>

namespace statistics {

// thanks to http://www.cplusplus.com/forum/general/184536/
template <typename SEQUENCE>
std::vector<SEQUENCE> Permutenr(const SEQUENCE& input, SEQUENCE output, size_t r)
{
	std::vector<SEQUENCE> s;

	if (output.size() == r) {
		s.push_back(output);
		return s;
	}

	else {
		for (std::size_t i = 0; i < input.size(); ++i) {
			SEQUENCE temp_output = output; // we do not want to modify output
			temp_output.push_back(input[i]);
			std::vector<SEQUENCE> temp = Permutenr(input, temp_output, r);
			s.insert(s.end(), temp.begin(), temp.end());
		}
	}

	return s;
}

} // statistics namespace

#endif