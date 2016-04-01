#ifndef _IMDB_DATASET_H_
#define _IMDB_DATASET_H_

#include "movie.h"
#include "graph.h"

namespace IMDB {

struct dataset {
	// data
	std::vector<Movie>       movies;
	std::vector<std::string> actors;
	std::vector<std::string> genres;
	std::vector<std::string>   tags;

	// graphs
	graph<Movie, std::string> movie_to_actor;
	graph<Movie, std::string> movie_to_genre;
	graph<Movie, std::string>   movie_to_tag;
};

} // IMDB namespace

#endif