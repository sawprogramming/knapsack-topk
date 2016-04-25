#include "topk_knapsack.h"

namespace topk {

Knapsack::Knapsack(std::initializer_list<scoreset*> sets, const size_t& num_elements) 
	: positions_(sets.size(), 0), util_(sets, num_elements) {
	for (auto set : sets) {
		lists_.push_back(set);
		itrs_.push_back(set->begin());
	}

	ComputeHistograms();
	SetValidPermutations(lists_.size());
}

scoreset Knapsack::TopK(const size_t& K) {
	scoreset            topk;
	size_t              spent_accesses = 0;
	std::vector<size_t> accesses;
	double benefit = 0;

	FindBestSchedule(accesses);
	do {
		// spend the accesses
		for (size_t list = 0; list < lists_.size(); ++list) {
			for (size_t batch_size = 0; batch_size < accesses[list]; ++batch_size) {
				score rank = *itrs_[list];

				if (!candidates_.HasKey(rank.first)) {
					candidates_.push_back(score_range_pair(rank.first, lists_.size()));
				}

				for (auto temp = candidates_.begin(false); temp != candidates_.end(); ++temp) {
					if (temp->first == rank.first) temp->second.Discover(list, rank.second);
					else                           temp->second.UpdateRange(list, rank.second);
				}

				++itrs_[list];
				++positions_[list];
				++spent_accesses;
			}
		}

		//stop if nothing more can be done, mink > best_score(c), otherwise prune

		if (candidates_.size() > K){
			score_range a = candidates_.at(K - 1), b = candidates_.at(K);

			if      (a.FullyDiscovered() && b.FullyDiscovered()) break;
			else if (a.worst_score() > b.best_score())           break;
			else {
				//for (auto itr = candidates_.begin(); itr != candidates_.end();) {
				//	if   (itr->second.best_score() < a.worst_score()) itr = candidates_.erase(itr->first);
				//	else                                              ++itr;
				//}
			}
		}

		if(spent_accesses % RECOMPUTE_HISTOGRAM_THRESHOLD == 0) ComputeHistograms();
	} while ((benefit = FindBestSchedule(accesses)) > 0);

	std::cout << "SPENT " << spent_accesses << std::endl;
	size_t k = 0;
	for (auto scorerange : candidates_) {
		topk.push_back(score(scorerange.first, scorerange.second.worst_score()));
		if (++k == K) break;
	}
	return topk;
}


void Knapsack::ComputeHistograms() {
	histograms_.clear();

	for (size_t list = 0; list < lists_.size(); ++list) {
		histograms_.push_back(lists_[list]->size());
		for (indexed_vector<size_t, double>::const_iterator itr = itrs_[list]; itr != lists_[list]->end(); ++itr) {
			histograms_[list].Insert((size_t)(itr->second * statistics::NUM_BINS));
		}
	}
}

double Knapsack::FindBestSchedule(std::vector<size_t>& schedule) const {
	double                           best_value = 0;
	size_t                           best_index = 0;

	// find the best schedule
	for (size_t permutation = 0; permutation < valid_permutations_.size(); ++permutation) {
		double value = 0;
		for (size_t list = 0; list < valid_permutations_[permutation].size(); ++list) {
			value += BenefitOfList(list, valid_permutations_[permutation][list]);
		}
		if (value > best_value) {
			best_value = value;
			best_index = permutation;
		}
	}

	schedule = valid_permutations_[best_index];
	return best_value;
}

double Knapsack::BenefitOfList(const size_t& list, const size_t& depth) const {
	double benefit = 0;

	if ((positions_[list] + depth) > lists_[list]->size()) {
		return 0;
	}
	for (auto candidate : candidates_) {
		if (!candidate.second.DiscoveredOn(list)) {
			benefit += BenefitOfObj(candidate.second, list, depth);
		}
	}

	return benefit;
}

double Knapsack::BenefitOfObj(const score_range& obj, const size_t& list, const size_t& depth) const {
	double probability, average, reduction;

	if ((lists_[list]->size() - positions_[list]) == 0) return 0;

	probability = ((double)depth / (lists_[list]->size() - positions_[list])) * util_.FeatureCorrelation(list, obj);
	average     = histograms_[list].GetExpectedValue() / statistics::NUM_BINS;
	reduction   = (1 - probability) * depth;
	
	if (average == 0) {
		if (!obj.DiscoveredOn(list)) return obj.score_at(list);
		else                         return 0;
	}
	return (probability * average) + reduction;
}

// ****************************************************************************
// Stick to small numbers with this function.
// Grows at the rate of (parameter + 1)^(num_lists)
//    Example:
//       With 4 lists and a parameter of 4, 625 intermediate results are generated
// ****************************************************************************
void Knapsack::SetValidPermutations(const size_t& accesses) {
	std::vector<std::vector<size_t>> permutations;

	// generate the input the permutation function needs and get the permutations
	std::vector<size_t> input;
	for (size_t i = 0; i < accesses + 1; ++i) {
		input.push_back(i);
	}
	permutations = statistics::Permutenr(input, std::vector<size_t>(), accesses);

	// only keep permutations that use this right number of accesses
	for (size_t i = 0; i < permutations.size(); ++i) {
		size_t sum = 0;
		for (size_t j = 0; j < permutations[i].size(); ++j) {
			sum += permutations[i][j];
		}
		if (sum == accesses) {
			valid_permutations_.push_back(permutations[i]);
		}
	}
}

} // topk namespace