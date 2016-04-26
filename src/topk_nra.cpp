#include "topk_nra.h"
#include <iostream>
namespace topk {

scoreset NRA::TopK(const size_t& K, std::initializer_list<scoreset*> sets) {
	scoreset        results;
	score_range_set topk;
	size_t          i = 0, k = 0, rows = (*sets.begin())->size(), spent_accesses = 0;

	// get the iterators to the sets passed in so we can do sequential access
	std::vector<std::pair<size_t, double>>::const_iterator* itrs = new std::vector<std::pair<size_t, double>>::const_iterator[sets.size()];
	for (auto set : sets) {
		itrs[i++] = set->begin();
	}

	// for each row...
	for(size_t row = 0; row < rows; ++row) {
		std::vector<double> highi;

		// ... look at each score list on that row ...
		for (size_t list = 0; list < sets.size(); ++list) {
			score rank = *(itrs[list]);
			highi.push_back(rank.second);

			// add the key to the top-k if it isn't in there, then discover the new score
			if (!topk.HasKey(rank.first)) {
				topk.push_back(score_range_pair(rank.first, sets.size()));
			}
			topk[rank.first].Discover(list, rank.second);

			++spent_accesses;
			(itrs[list])++;
		}

		// update all best_scores
		for (auto temp = topk.begin(false); temp != topk.end(); ++temp) {
			for (size_t list = 0; list < sets.size(); ++list) {
				temp->second.UpdateRange(list, highi[list]);
			}
		}

		// stop if tie or mink > best_score(c), otherwise prune
		if (topk.size() > K){
			score_range a = topk.at(K - 1), b = topk.at(K);

			if      (a.FullyDiscovered() && b.FullyDiscovered()) break;
			else if (a.worst_score() > b.best_score())           break;
			else {
				for (auto itr = topk.begin(); itr != topk.end();) {
					if (itr->second.best_score() < a.worst_score()) itr = topk.erase(itr->first);
					else                                            ++itr;
				}
			}
		}
	}
	delete[] itrs;

	std::cout << "SPENT " << spent_accesses << " ACCESSES" << std::endl;
	// return the top-k
	for (auto scorerange : topk) {
		results.push_back(score(scorerange.first, scorerange.second.worst_score()));
		if (++k == K) break;
	}
	return results;
}

} // topk namespace