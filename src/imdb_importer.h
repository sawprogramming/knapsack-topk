#ifndef _IMDB_IMPORTER_
#define _IMDB_IMPORTER_

#include <ppl.h>
#include <ctime>
#include <string>
#include <fstream>
#include <windows.h>
#include "graph.h"
#include "movie.h"
#include "pcre.h"
#include "imdb_dataset.h"

namespace IMDB {

class Importer {
public:
	bool Import(dataset& s);

private:
	bool ImportActors ();
	bool ImportGenres ();
	bool ImportMovies ();
	bool ImportTags   ();

	std::ofstream log_file_;
	dataset*      data_;
};

} // IMDB namespace

#endif