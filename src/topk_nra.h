#ifndef _TOPK_NRA_H_
#define _TOPK_NRA_H_

#include <initializer_list>
#include "topk_scoreset.h"
#include "topk_score_range.h"

namespace topk {

class NRA {
public:
	static scoreset TopK(const size_t& k, std::initializer_list<scoreset*> sets);
};

} // topk namespace

#endif

