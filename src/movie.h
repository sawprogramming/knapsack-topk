#ifndef _MOVIE_H_
#define _MOVIE_H_

#include <string>
#include <iostream>

class Movie {
public:
	Movie(const std::string& title = "", const unsigned __int16& year = 0);

	friend bool          operator<  (const Movie& left, const Movie& right);
	friend std::ostream& operator<< (std::ostream&  os, const Movie&     m);
	
	const __int16&     GetYear  () const;
	const std::string& GetTitle () const;

private:
	__int16     year_;
	std::string title_;
};

#endif