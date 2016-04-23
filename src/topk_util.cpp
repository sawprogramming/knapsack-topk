#include "topk_util.h"

namespace topk {

TopkUtil::TopkUtil(std::initializer_list<scoreset*> sets, const size_t& num_elements) 
	: contingency_table_(sets.size()), num_elements_(num_elements) {
	// copy reference to score sets
	for (auto set : sets) {
		lists_.push_back(set);
	}

	// initialize and fill contingency table
	for (size_t i = 0; i < contingency_table_.size(); ++i) {
		std::swap(contingency_table_[i], std::vector<size_t>(sets.size()));
	}
	FillContingencyTable();
}

double TopkUtil::PredictScore(const score_range& sr, const double& target, const std::vector<statistics::histogram> histograms) const {
	size_t                list;
	statistics::histogram conv;

	// find the first unseen list
	for (list = 0; list < lists_.size(); ++list) {
		if (!sr.DiscoveredOn(list)) {
			conv = histograms[list];
			break;
		}
	}

	// get the convolution of any other unseen lists (or if everything has been seen, return 0 or 1)
	if (list != lists_.size()) {
		for (list = list + 1; list < lists_.size(); ++list) {
			if (!sr.DiscoveredOn(list)) {
				conv = conv.GetConvolution(histograms[list]);
			}
		}
	} else return sr.worst_score() > target ? 1 : 0;

	// return probability that we can get enough score from the unseen lists to beat the target (if such score is possible)
	size_t needed_score = (size_t)((target - sr.worst_score()) * statistics::NUM_BINS) + 1;
	return needed_score > (conv.GetNumBins() - 1) ? 0 : 1 - conv.CumulativeFrequencyOf(needed_score);
}

double TopkUtil::FeatureCorrelation(const size_t& i, const score_range& sr) const {
	double max_correlation = 0;

	for (size_t list = 0; list < lists_.size(); ++list) {
		if (i != list && sr.DiscoveredOn(list)) {
			double correlation = ((double)contingency_table_[i][list] / lists_[list]->size());
			if(correlation > max_correlation) max_correlation = correlation;
		}
	}

	return max_correlation;
}

double TopkUtil::EstimateSelectivity(const score_range& sr, const std::vector<size_t>& positions) const {
	double selectivity = 1;

	for (size_t list = 0; list < lists_.size(); ++list) {
		if (!sr.DiscoveredOn(list)) {
			selectivity *= 1 - (
				(double)(lists_[list]->size() - positions[list]) 
			  / (num_elements_ - positions[list])
			);
		}
	}

	return 1 - selectivity;
}

void TopkUtil::FillContingencyTable() {
	for (size_t i = 0; i < lists_.size(); ++i) {
		for (size_t j = 0; j < lists_.size(); ++j) {
			size_t common_elements = 0;

			if (i != j) {
				for (auto itr = lists_[i]->begin(false); itr != lists_[i]->end(); ++itr) {
					if (lists_[j]->HasKey(itr->first)) {
						++common_elements;
					}
				}
			} else common_elements = lists_[i]->size();

			contingency_table_[i][j] = common_elements;
			contingency_table_[j][i] = common_elements;
		}
	}
}

} // topk namespace