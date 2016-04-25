#ifndef _TOPK_KNAPSACK_
#define _TOPK_KNAPSACK_

#include "histogram.h"
#include "statistics.h"
#include "topk_util.h"
#include "topk_scoreset.h"
#include "topk_score_range.h"

namespace topk {

const size_t RECOMPUTE_HISTOGRAM_THRESHOLD = 400;

class Knapsack {
public:
	Knapsack(std::initializer_list<scoreset*> sets, const size_t& num_elements);

	scoreset TopK(const size_t& k);

private:
	void   ComputeHistograms    ();
	void   SetValidPermutations (const size_t& accesses);
	double FindBestSchedule     (std::vector<size_t>& schedule)                                    const;
	double BenefitOfList        (const size_t& list,     const size_t& depth)                      const;
	double BenefitOfObj         (const score_range& obj, const size_t& list,  const size_t& depth) const;

	score_range_set                                       candidates_;
	TopkUtil                                              util_;
	std::vector<size_t>                                   positions_;
	std::vector<scoreset*>                                lists_;
	std::vector<statistics::histogram>                    histograms_;
	std::vector<indexed_vector<size_t, double>::iterator> itrs_;
	std::vector<std::vector<size_t>>                      valid_permutations_;
};

} // topk namespace

#endif