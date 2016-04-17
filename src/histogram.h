#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include <cmath>
#include <vector>
#include <iomanip>
#include <iostream>

namespace statistics {

const size_t NUM_BINS = 100;

class histogram {
public:
	histogram(const size_t& num_elements, const size_t& bins = NUM_BINS);

	// overloads
	friend std::ostream& operator<<(std::ostream& os, const histogram& h);

	// useful functions
	histogram GetConvolution(const histogram& h) const;

	// mutators
	void Insert         (const size_t& value);
	void SetNumElements (const size_t& num_elements);
	void SetNumBins     (const size_t& bins);

	// statistics
	double GetExpectedValue     () const;
	double GetVariance          () const;
	double GetStandardDeviation () const;

	// accessors
	double FrequencyOf           (const size_t& value) const;
	double CumulativeFrequencyOf (const size_t& value) const;

private:
	size_t              num_elements_;
	std::vector<double> freq_, cum_freq_;
};

} // statistics namespace

#endif