#include "topk_score_range.h"

namespace topk {

score_range::score_range(const size_t& num_lists) : worst_score_(0), best_score_(0), num_lists_(num_lists) {
	discovered_fields_ = new double[num_lists_]{0};
}

score_range::score_range(const score_range& ref) : score_range(ref.num_lists_) {
	worst_score_ = ref.worst_score_;
	best_score_  = ref.best_score_;
	std::copy(ref.discovered_fields_, ref.discovered_fields_ + 4, discovered_fields_);
}

score_range::score_range(score_range&& other) : score_range(other.num_lists_) {
	swap(*this, other);
}

score_range::~score_range() {
	delete[] discovered_fields_;
}

score_range& score_range::operator=(score_range other) {
	swap(*this, other);
	return *this;
}

bool operator< (const score_range& left, const score_range& right) {
	if      (left.worst_score_  < right.worst_score_) return  true;
	else if (right.worst_score_ <  left.worst_score_) return false;
	else if (left.best_score_   <  right.best_score_) return  true;
	else                                              return false;
}

void swap(score_range& first, score_range& second) {
	std::swap(first.worst_score_       ,       second.worst_score_);
	std::swap(first.best_score_        ,        second.best_score_);
	std::swap(first.num_lists_         ,         second.num_lists_);
	std::swap(first.discovered_fields_ , second.discovered_fields_);
}

void score_range::Discover(const size_t& index, const double& value) {
	worst_score_              += value;
	best_score_               += discovered_fields_[index];
	discovered_fields_[index]  = value;
	best_score_               += discovered_fields_[index];
}

void score_range::UpdateRange(const size_t& index, const double& value) {
	if (discovered_fields_[index] <= 0) {
		best_score_               += discovered_fields_[index];
		discovered_fields_[index]  = value * -1;
		best_score_               += value;
	}
}

double score_range::best_score  () const { return  best_score_; }
double score_range::worst_score () const { return worst_score_; }

} // topk namespace