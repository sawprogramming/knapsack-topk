#include "imdb_importer.h"

namespace IMDB {

bool Importer::Import(dataset& data) {
	clock_t begin, end;
	double  time_spent;
	bool    m = false, g = false, t = false, a = false;

	data_ = &data;
	log_file_.open("../logs/imdb_error.log");

	// import
	std::cout << "Importing IMDB database... " << std::flush;
	begin = clock();

	// prepare the graphs
	m = ImportMovies();
	data_->movie_to_actor.SetSize(data_->movies.size());
	data_->movie_to_genre.SetSize(data_->movies.size());
	data_->movie_to_tag.SetSize(data_->movies.size());

	// load the graphs
	concurrency::parallel_invoke(
		[&] () { a = ImportActors(); },
		[&] () { g = ImportGenres(); },
		[&] () { t = ImportTags();   }
	);

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	std::cout << "finished after " << time_spent << "s!" << std::endl;
	log_file_.clear();
	log_file_.close();

	return m && g && t && a;
}

bool Importer::ImportActors() {
	__int16      year;
	size_t      lineEnd, line_length;
	const char* error;
	int         rc, erroffset, progress = 0, ovector[15];
	std::string file_name = "../data/actors.list";

	// memory map the file
	HANDLE  hFile         = CreateFile(file_name.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE  hMMFile       = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, "imdb_actors");
	LPCTSTR file_contents = (LPTSTR)MapViewOfFile(hMMFile, FILE_MAP_READ, 0, 0, 0);
	size_t  total_length  = strlen(file_contents);

	// compile the regexes
	const char* name_pattern   = "(.*?)\\t+(?:.*)";
	const char* year_pattern   = "\\t+(.*?)\\s\\((\\d{4})(?:\\/\\w+)*\\)(?:.*)";
	const char* noyear_pattern = "\\t+(.*?)\\s\\((?:\\?{4})(?:\\/\\w+)*\\)(?:.*)";
	pcre*       name_regex     = pcre_compile(name_pattern,   0, &error, &erroffset, 0);
	pcre*       year_regex     = pcre_compile(year_pattern,   0, &error, &erroffset, 0);
	pcre*       noyear_regex   = pcre_compile(noyear_pattern, 0, &error, &erroffset, 0);

	// extract information from file
	for (size_t lineStart = 0; lineStart < total_length; lineStart = lineEnd + 1) {
		for (lineEnd = lineStart; lineEnd < total_length; ++lineEnd) {
			if (file_contents[lineEnd] == '\n') break;
		}
		if (file_contents[lineStart] == '\n') continue;

		// allocate enough memory then copy the line
		line_length       = lineEnd - lineStart;
		char* line        = new char[line_length + 1];
		line[line_length] = '\0';
		memcpy(line, file_contents + lineStart, line_length);

		// get offsets of actor name
		if   (pcre_exec(name_regex, NULL, line, (int)line_length, 0, 0, ovector, 15) == 2);
		else {
			log_file_ << "Actors: failed to match:" << std::endl
				      << "\t" <<               line << std::endl;
			continue;
		}

		// extract actor
		size_t actor_length = ovector[3] - ovector[2];
		char*  actor        = new char[actor_length + 1];
		actor[actor_length] = '\0';
		memcpy(actor, line + ovector[2], actor_length);
		delete[] line;

		// find movies they starred in
		for (lineStart = lineStart + ovector[3]; lineStart < total_length; lineStart = lineEnd + 1) {
			if (file_contents[lineStart] == '\n') break;
			for (lineEnd = lineStart; lineEnd < total_length; ++lineEnd) {
				if (file_contents[lineEnd] == '\n') break;
			}

			// allocate enough memory then copy the line
			line_length       = lineEnd - lineStart;
			line              = new char[line_length + 1];
			line[line_length] = '\0';
			memcpy(line, file_contents + lineStart, line_length);

			if      ((rc = pcre_exec(year_regex, NULL, line, (int)line_length, 0, 0, ovector, 15))   == 3);
			else if ((rc = pcre_exec(noyear_regex, NULL, line, (int)line_length, 0, 0, ovector, 15)) == 2);
			else 	{
				log_file_ << "Actors: failed to match:" << std::endl
						  << "\t" <<               line << std::endl;
				continue;
			}

			// skip any TV shows
			if (line[ovector[2]] == '"') {
				delete [] line;
				continue;
			}

			// extract title
			size_t title_length = ovector[3] - ovector[2];
			char*  title        = new char[title_length + 1];
			title[title_length] = '\0';
			memcpy(title, line + ovector[2], title_length);
			
			// extract year
			if (rc == 3) {
				char* syear = new char[5];
				syear[4]    = '\0';
				memcpy(syear, line + ovector[4], 4);
				year        = (__int16)atoi(syear);
				delete [] syear;
			} else year = 0;


			// add the actor and relation if it hasn't been already
			if (data_->movie_to_actor.Insert(actor, data_->actors.size())) {
				data_->actors.push_back(actor);
			}
			if (!data_->movie_to_actor.AddEdge(Movie(title, year), actor)) {
				log_file_ << "Actors: missing movie '" << Movie(title, year).GetTitle() << "'" << std::endl;
			}

			delete [] title;
			delete [] line;
		}
	}

	UnmapViewOfFile(file_contents);
	CloseHandle(hMMFile);
	return true;
}

bool Importer::ImportGenres() {
	__int16      year;
	size_t      lineEnd, line_length;
	const char* error;
	int         rc, erroffset, progress = 0, ovector[15];
	std::string file_name = "../data/genres.list";

	// memory map the file
	HANDLE  hFile         = CreateFile(file_name.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE  hMMFile       = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, "imdb_genres");
	LPCTSTR file_contents = (LPTSTR)MapViewOfFile(hMMFile, FILE_MAP_READ, 0, 0, 0);
	size_t  total_length  = strlen(file_contents);

	// compile the regexes
	const char* year_pattern   = "(.*?)\\s\\((\\d{4})(?:\\/\\w+)*\\)(?:.*?)(?:\\t+)(\\w+)";
	const char* noyear_pattern = "(.*?)\\s\\((?:\\?{4})(?:\\/\\w+)*\\)(?:.*?)(?:\\t+)(\\w+)";
	pcre*       year_regex     = pcre_compile(year_pattern,   0, &error, &erroffset, 0);
	pcre*       noyear_regex   = pcre_compile(noyear_pattern, 0, &error, &erroffset, 0);

	// extract information from file
	for (size_t lineStart = 0; lineStart < total_length; lineStart = lineEnd + 1) {
		for (lineEnd = lineStart; lineEnd < total_length; ++lineEnd) {
			if (file_contents[lineEnd] == '\n') break;
		}

		// just skip any TV shows
		if (file_contents[lineStart] == '"') continue;

		// allocate enough memory then copy the line
		line_length       = lineEnd - lineStart;
		char* line        = new char[line_length + 1];
		line[line_length] = '\0';
		memcpy(line, file_contents + lineStart, line_length);

		// get offsets of matching data
		if      ((rc = pcre_exec(year_regex, NULL, line, (int)line_length, 0, 0, ovector, 15))   == 4);
		else if ((rc = pcre_exec(noyear_regex, NULL, line, (int)line_length, 0, 0, ovector, 15)) == 3);
		else 	{
			log_file_ << "Genres: failed to match:" << std::endl
				      << "\t" <<             line << std::endl;
			continue;
		}

		// extract title
		size_t title_length = ovector[3] - ovector[2];
		char*  title        = new char[title_length + 1];
		title[title_length] = '\0';
		memcpy(title, line + ovector[2], title_length);
			
		// extract year
		if (rc == 4) {
			char* syear = new char[5];
			syear[4]    = '\0';
			memcpy(syear, line + ovector[4], 4);
			year        = (__int16)atoi(syear);
			delete [] syear;
		} else year = 0;

		// extract genre
		size_t genre_length = rc == 4 ? ovector[7] - ovector[6] : ovector[5] - ovector[4];
		char*  genre        = new char[genre_length + 1];
		genre[genre_length] = '\0';
		memcpy(genre, line + (rc == 4 ? ovector[6] : ovector[4]), genre_length);

		// add the genre and relation if it hasn't been already
		if (data_->movie_to_genre.Insert(genre, data_->genres.size())) {
			data_->genres.push_back(genre);
		}
		if (!data_->movie_to_genre.AddEdge(Movie(title, year), genre)) {
			log_file_ << "Genres: missing movie '" << Movie(title, year).GetTitle() << "'" << std::endl;
		}

		delete [] title;
		delete [] line;
	}

	UnmapViewOfFile(file_contents);
	CloseHandle(hMMFile);
	return true;
}

bool Importer::ImportMovies() {
	__int16     year;
	size_t      lineEnd, line_length;
	const char* error;
	int         rc, erroffset, progress = 0, ovector[15];
	std::string file_name = "../data/movies.list";
	
	// memory map the file
	HANDLE  hFile         = CreateFile(file_name.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE  hMMFile       = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, "imdb_movies");
	LPCTSTR file_contents = (LPTSTR)MapViewOfFile(hMMFile, FILE_MAP_READ, 0, 0, 0);
	size_t  total_length  = strlen(file_contents);

	// compile the regexes
	const char* year_pattern   = "(.*?)\\s\\((\\d{4})(?:\\/\\w+)*\\)(?:.*)";
	const char* noyear_pattern = "(.*?)\\s\\((?:\\?{4})(?:\\/\\w+)*\\)(?:.*?)(?:.*)";
	pcre*       year_regex     = pcre_compile(year_pattern,   0, &error, &erroffset, 0);
	pcre*       noyear_regex   = pcre_compile(noyear_pattern, 0, &error, &erroffset, 0);

	// extract information from file
	for (size_t lineStart = 0; lineStart < total_length; lineStart = lineEnd + 1) {
		for (lineEnd = lineStart; lineEnd < total_length; ++lineEnd) {
			if (file_contents[lineEnd] == '\n') break;
		}

		// just skip any TV shows
		if (file_contents[lineStart] == '"') continue;

		// allocate enough memory then copy the line
		line_length       = lineEnd - lineStart;
		char* line        = new char[line_length + 1];
		line[line_length] = '\0';
		memcpy(line, file_contents + lineStart, line_length);

		// get offsets of matching data
		if      ((rc = pcre_exec(year_regex, NULL, line, (int)line_length, 0, 0, ovector, 15))   == 3);
		else if ((rc = pcre_exec(noyear_regex, NULL, line, (int)line_length, 0, 0, ovector, 15)) == 2);
		else 	{
			log_file_ << "Movies: failed to match:" << std::endl
				      << "\t" <<               line << std::endl;
			continue;
		}

		// extract title
		size_t title_length = ovector[3] - ovector[2];
		char*  title        = new char[title_length + 1];
		title[title_length] = '\0';
		memcpy(title, line + ovector[2], title_length);
			
		// extract year
		if (rc == 3) {
			char* syear = new char[5];
			syear[4]    = '\0';
			memcpy(syear, line + ovector[4], 4);
			year        = (__int16)atoi(syear);
			delete [] syear;
		} else year = 0;

		// add the movie to our set
		Movie m(title, year);
		size_t pos = data_->movies.size();
		if (data_->movie_to_actor.Insert(m, pos)) {
			data_->movie_to_genre.Insert(m, pos);
			data_->movie_to_tag.Insert(m, pos);
			data_->movies.push_back(m);
		}

		delete [] title;
		delete [] line;
	}

	UnmapViewOfFile(file_contents);
	CloseHandle(hMMFile);
	return true;
}

bool Importer::ImportTags() {
	__int16      year;
	size_t      lineEnd, line_length;
	const char* error;
	int         rc, erroffset, progress = 0, ovector[15];
	std::string file_name = "../data/keywords.list";

	// memory map the file
	HANDLE  hFile         = CreateFile(file_name.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE  hMMFile       = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, "imdb_keywords");
	LPCTSTR file_contents = (LPTSTR)MapViewOfFile(hMMFile, FILE_MAP_READ, 0, 0, 0);
	size_t  total_length  = strlen(file_contents);

	// compile the regexes
	const char* year_pattern   = "(.*?)\\s\\((\\d{4})(?:\\/\\w+)*\\)(?:.*?)(?:\\t+)(.*)";
	const char* noyear_pattern = "(.*?)\\s\\((?:\\?{4})(?:\\/\\w+)*\\)(?:.*?)(?:\\t+)(.*)";
	pcre*       year_regex     = pcre_compile(year_pattern,   0, &error, &erroffset, 0);
	pcre*       noyear_regex   = pcre_compile(noyear_pattern, 0, &error, &erroffset, 0);

	// extract information from file
	for (size_t lineStart = 0; lineStart < total_length; lineStart = lineEnd + 1) {
		for (lineEnd = lineStart; lineEnd < total_length; ++lineEnd) {
			if (file_contents[lineEnd] == '\n') break;
		}

		// just skip any TV shows
		if (file_contents[lineStart] == '"') continue;

		// allocate enough memory then copy the line
		line_length       = lineEnd - lineStart;
		char* line        = new char[line_length + 1];
		line[line_length] = '\0';
		memcpy(line, file_contents + lineStart, line_length);

		// get offsets of matching data
		if      ((rc = pcre_exec(year_regex, NULL, line, (int)line_length, 0, 0, ovector, 15))   == 4);
		else if ((rc = pcre_exec(noyear_regex, NULL, line, (int)line_length, 0, 0, ovector, 15)) == 3);
		else 	{
			log_file_ << "Tags: failed to match:" << std::endl
				      << "\t" <<             line << std::endl;
			continue;
		}

		// extract title
		size_t title_length = ovector[3] - ovector[2];
		char*  title        = new char[title_length + 1];
		title[title_length] = '\0';
		memcpy(title, line + ovector[2], title_length);
			
		// extract year
		if (rc == 4) {
			char* syear = new char[5];
			syear[4]    = '\0';
			memcpy(syear, line + ovector[4], 4);
			year        = (__int16)atoi(syear);
			delete [] syear;
		} else year = 0;

		// extract tag
		size_t tag_length = rc == 4 ? ovector[7] - ovector[6] : ovector[5] - ovector[4];
		char*  tag        = new char[tag_length + 1];
		tag[tag_length]   = '\0';
		memcpy(tag, line + (rc == 4 ? ovector[6] : ovector[4]), tag_length);

		// add the tag and relation if it hasn't been already
		if (data_->movie_to_tag.Insert(tag, data_->tags.size())) {
			data_->tags.push_back(tag);
		}
		if (!data_->movie_to_tag.AddEdge(Movie(title, year), tag)) {
			log_file_ << "Tags: missing movie '" << Movie(title, year).GetTitle() << "'" << std::endl;
		}

		delete [] title;
		delete [] line;
	}

	UnmapViewOfFile(file_contents);
	CloseHandle(hMMFile);
	return true;
}

} // IMDB namespace