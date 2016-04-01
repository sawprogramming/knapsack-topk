#ifndef _IMDB_SCORER_H_
#define _IMDB_SCORER_H_

#include <sstream>
#include "imdb_dataset.h"
#include "topk_scoreset.h"
#include "string_util.h"

namespace IMDB {

class Scorer {
public:
	Scorer(const dataset& ds);

	void ScoreGenres (std::string query, topk::scoreset& scores) const;
	void ScoreTitles (std::string query, topk::scoreset& scores) const;
	void ScoreActors (std::string query, topk::scoreset& scores) const;
	void ScoreTags   (std::string query, topk::scoreset& scores) const;

private:
	dataset const * ds_;
};

} // IMDB namespace

#endif