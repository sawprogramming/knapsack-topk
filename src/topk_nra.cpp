#include "topk_nra.h"

namespace topk {

scoreset NRA::TopK(const size_t& K, std::initializer_list<scoreset*> sets) {
	scoreset        results;
	score_range_set topk;
	size_t          i = 0, k = 0, rows = (*sets.begin())->size();

	// get the iterators to the sets passed in so we can do sequential access
	std::vector<std::pair<size_t, double>>::const_iterator* itrs = new std::vector<std::pair<size_t, double>>::const_iterator[sets.size()];
	for (auto set : sets) {
		itrs[i++] = set->begin();
	}

	// for each row...
	for(size_t row = 0; row < rows; ++row) {
		// ... look at each score list on that row ...
		for (size_t list = 0; list < sets.size(); ++list) {
			score rank = *(itrs[list]);

			// add the key to the top-k if it isn't in there
			if (!topk.HasKey(rank.first)) {
				topk.push_back(score_range_pair(rank.first, sets.size()));
			}

			// update the worst and best scores for each value in the topk list
			for (auto temp = topk.begin(false); temp != topk.end(); ++temp) {
				if   (temp->first == rank.first) temp->second.Discover(list,    rank.second);
				else                             temp->second.UpdateRange(list, rank.second);
			}

			(itrs[list])++;
		}

		// stop if mink > best_score(c)
		if ((topk.size() > K) && (topk.at(K - 1).worst_score() > topk.at(K).best_score())) {
			break;
		}
	}
	delete[] itrs;

	// return the top-k
	for (auto scorerange : topk) {
		results.push_back(score(scorerange.first, scorerange.second.worst_score()));
		if (++k == K) break;
	}
	return results;
}

} // topk namespace