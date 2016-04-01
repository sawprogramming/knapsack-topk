#ifndef _SCORESET_H_
#define _SCORESET_H_

#include <utility>
#include "indexed_vector.h"

namespace topk {

typedef std::pair<size_t, double> score;
typedef indexed_vector<size_t, double> scoreset;

} // topk namespace

#endif