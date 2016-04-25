#ifndef _TOPK_UTIL_H_
#define _TOPK_UTIL_H_

#include "histogram.h"
#include "topk_scoreset.h"
#include "topk_score_range.h"

namespace topk {

class TopkUtil {
public:
	TopkUtil(std::initializer_list<scoreset*> sets, const size_t& num_elements);

	double PredictScore        (const score_range& sr, const double& target, const std::vector<statistics::histogram> histograms) const;
	double FeatureCorrelation  (const size_t& i,       const score_range& sr)                                                     const;
	double EstimateSelectivity (const score_range& sr, const std::vector<size_t>& positions)                                      const;

private:
	void FillContingencyTable();

	size_t                           num_elements_;
	std::vector<std::vector<size_t>> contingency_table_;
	std::vector<scoreset*>           lists_;
};

} // topk namespace

#endif