#include "imdb_scorer.h"

namespace IMDB {

Scorer::Scorer(const dataset& ds) {
	ds_ = &ds;
}

void Scorer::ScoreTitles(std::string query, topk::scoreset& scores) const {
	for(size_t i = 0; i < ds_->movies.size(); ++i) {
		double score = 0;
		
		// the score is 1 if it is an exact match
		int distance = StringUtil::ldistance(ds_->movies[i].GetTitle(), query);
		if (distance == 0) {
			score = 1;
		}

		// otherwise the score is the percentage of matching characters
		else {
			score = 1 - ((double)distance / ds_->movies[i].GetTitle().size());
			if (score < 0 || score > 1) score = 0;
		}

		scores.push_back(topk::score(i, score));
	}
}

void Scorer::ScoreGenres(std::string query, topk::scoreset& scores) const {
	std::string              token;
	std::vector<std::string> query_genres;
	std::istringstream       ss(query);
	const adjacency_list&    genres = ds_->movie_to_genre.GetAdjacencyList();

	// split query into separate tags
	while (std::getline(ss, token, ',')) {
		query_genres.push_back(token);
	}

	// calculate the scores each movie
	for (size_t i = 0; i < ds_->movies.size(); ++i) {
		double score = 0;

		// score is increased by 1/(genres in query) for each matching genre
		for (auto genreid : genres[i]) {
			for (auto tag : query_genres) {
				if (ds_->genres[genreid].compare(tag) == 0) {
					score += ((double)1 / query_genres.size());
				}
			}
		}

		scores.push_back(topk::score(i, score));
	}
}

void Scorer::ScoreActors(std::string query, topk::scoreset& scores) const {
	const adjacency_list& actors = ds_->movie_to_actor.GetAdjacencyList();

	for (size_t i = 0; i < ds_->movies.size(); ++i) {
		double score = 0, temp = 0;

		for (auto actorid : actors[i]) {
			int distance = StringUtil::ldistance(ds_->actors[actorid], query);

			// if the actor matches, set score to 1 and break
			if (distance == 0) {
				score = 1;
				break;
			}
			
			// otherwise the score is the highest score of all the actors in the movie
			// which is determined by the percentage of matching characters
			else {
				temp = 1 - ((double)distance / ds_->actors[actorid].size());
				if (temp < 0 || temp > 1) temp = 0;
				if (temp > score)         score = temp;
			}
		}

		scores.push_back(topk::score(i, score));
	}
}

void Scorer::ScoreTags(std::string query, topk::scoreset& scores) const {
	std::string              token;
	std::vector<std::string> query_tags;
	std::istringstream       ss(query);
	const adjacency_list&    tags = ds_->movie_to_tag.GetAdjacencyList();

	// split query into separate tags
	while (std::getline(ss, token, ',')) {
		query_tags.push_back(token);
	}

	// calculate the scores each movie
	for(size_t i = 0; i < ds_->movies.size(); ++i) {
		double score = 0;

		// score is increased by 1/(tags in query) for each matching tag
		for (auto tagid : tags[i]) {
			for (auto tag : query_tags) {
				if (ds_->tags[tagid].compare(tag) == 0) {
					score += ((double)1 / query_tags.size());
				}
			}
		}

		scores.push_back(topk::score(i, score));
	}
}

} // IMDB namespace