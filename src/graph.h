#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <map>
#include <vector>
#include <algorithm>

typedef size_t vertex;
typedef std::vector<std::vector<vertex>> adjacency_list;

template <class F, class S>
class graph {
public:
	bool HasVertex(const F& value) const;
	bool HasVertex(const S& value) const;

	void SetSize (const size_t& size);
	bool AddEdge (const F& from,  const      S&  to);
	bool Insert  (const F& value, const vertex& pos);
	bool Insert  (const S& value, const vertex& pos);

	const adjacency_list& GetAdjacencyList() const;

private:
	std::map<F, vertex> f_vertices_;
	std::map<S, vertex> s_vertices_;
	adjacency_list      adjacency_list_;
};

// ********************************************************************************************************************
template <class F, class S>
bool graph<F, S>::Insert(const F& value, const vertex& pos) {
	if (!HasVertex(value)) {
		f_vertices_[value] = pos;
		return true;
	}

	return false;
}

template <class F, class S>
bool graph<F, S>::Insert(const S& value, const vertex& pos) {
	if (!HasVertex(value)) {
		s_vertices_[value] = pos;
		return true;
	}

	return false;
}

template <class F, class S>
bool graph<F, S>::HasVertex(const F& value) const {
	return f_vertices_.find(value) != f_vertices_.end(); 
}

template <class F, class S>
bool graph<F, S>::HasVertex(const S& value) const {
	return s_vertices_.find(value) != s_vertices_.end(); 
}

template <class F, class S>
bool graph<F, S>::AddEdge(const F& from, const S& to) {
	vertex f, t;

	if (HasVertex(from) && HasVertex(to)) {
		f = f_vertices_.at(from);
		t = s_vertices_.at(to);
		
		// add the edge if it hasn't been already
		for (int i = 0; i < adjacency_list_[f].size(); ++i) {
			if (adjacency_list_[f][i] == t) return true;
		}
		adjacency_list_[f].push_back(t);

		return true;
	}

	return false;
}

template <class F, class S>
void graph<F, S>::SetSize(const size_t& size) {
	adjacency_list_.resize(size);
}

template <class F, class S>
const adjacency_list& graph<F, S>::GetAdjacencyList() const {
	return adjacency_list_;
}

#endif