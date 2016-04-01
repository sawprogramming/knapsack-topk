#ifndef _TOPK_SCORE_RANGE_
#define _TOPK_SCORE_RANGE_

#include "indexed_vector.h"

namespace topk {

class score_range {
public:
	score_range(const size_t& num_lists);
	score_range(const score_range& ref);
	score_range(score_range&& other);
	~score_range();

	score_range& operator= (const score_range other);
	friend bool  operator< (const score_range& left, const score_range& right);

	friend void swap(score_range& first, score_range& second);

	void Discover    (const size_t& index, const double& value);
	void UpdateRange (const size_t& index, const double& value);

	double best_score  () const;
	double worst_score () const;

private:
	size_t  num_lists_;
	double  worst_score_, best_score_, *discovered_fields_;
};

typedef std::pair<size_t, score_range> score_range_pair;
typedef indexed_vector<size_t, score_range> score_range_set;

} // topk namespace

#endif