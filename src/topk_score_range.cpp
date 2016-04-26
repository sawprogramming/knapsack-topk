#include "topk_score_range.h"

namespace topk {

score_range::score_range(const size_t& num_lists) : worst_score_(0), best_score_(0), num_lists_(num_lists) {
	fields_ = new score_range_field[num_lists];
	std::fill_n(fields_, num_lists_, score_range_field(false, 0));
}

score_range::score_range(const score_range& ref) : score_range(ref.num_lists_) {
	worst_score_ = ref.worst_score_;
	best_score_  = ref.best_score_;
	std::copy(ref.fields_, ref.fields_ + ref.num_lists_, fields_);
}

score_range::score_range(score_range&& other) : score_range(other.num_lists_) {
	swap(*this, other);
}

score_range::~score_range() {
	delete[] fields_;
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

std::ostream& operator<< (std::ostream& os, const score_range& sr) {
	return os << "[" << sr.worst_score_ << ", " << sr.best_score_ << "]";
}


void swap(score_range& first, score_range& second) {
	std::swap(first.worst_score_ , second.worst_score_);
	std::swap(first.best_score_  , second.best_score_);
	std::swap(first.num_lists_   , second.num_lists_);
	std::swap(first.fields_      , second.fields_);
}

void score_range::Discover(const size_t& index, const double& value) {
	// book keeping
	best_score_          -= fields_[index].second;
	fields_[index].first  = true;
	fields_[index].second = value;

	// update the range
	worst_score_ += value;
	best_score_  += value;
}

void score_range::UpdateRange(const size_t& index, const double& value) {
	if (!fields_[index].first) {
		best_score_          -= fields_[index].second;
		fields_[index].second = value;
		best_score_          += value;
	}
}

double score_range::best_score      ()                    const { return          best_score_;  }
double score_range::worst_score     ()                    const { return         worst_score_;  }
double score_range::score_at        (const size_t& index) const { return fields_[index].second; }
bool   score_range::DiscoveredOn    (const size_t& index) const { return fields_[index].first;  }
bool   score_range::FullyDiscovered () const {
	for (size_t i = 0; i < num_lists_; ++i) {
		if (fields_[i].first == false) {
			return false;
		}
	}
	return true;
}

} // topk namespace