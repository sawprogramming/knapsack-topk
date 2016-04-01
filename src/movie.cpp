#include "movie.h"

Movie::Movie(const std::string& title, const unsigned __int16& year) : title_(title), year_(year) {}

bool operator<(const Movie& left, const Movie& right) {
	if      (_strcmpi(left.title_.c_str(), right.title_.c_str()) < 0) return  true;
	else if (_strcmpi(left.title_.c_str(), right.title_.c_str()) > 0) return false;
	else if (left.year_ < right.year_)                                return  true;
	else                                                              return false;
}

std::ostream& operator<<(std::ostream& os, const Movie& m) {
	os << m.title_ << " (" << m.year_ << ")";
	return os;
}

const __int16&     Movie::GetYear  () const { return   year_; }
const std::string& Movie::GetTitle () const { return  title_; }