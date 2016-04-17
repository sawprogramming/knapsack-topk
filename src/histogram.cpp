#include "histogram.h"

namespace statistics {

histogram::histogram(const size_t& num_elements, const size_t& bins) : freq_(bins + 1, 0), cum_freq_(bins + 1, 0), num_elements_(num_elements) {}

std::ostream& operator<<(std::ostream& os, const histogram& h) {
	std::streamsize         oldprecision = os.precision();
	char                    oldfill      = os.fill();
	std::ios_base::fmtflags oldflags     = os.flags();

	// output the histogram
	for (size_t i = 0; i < h.freq_.size(); ++i) {
		os << std::fixed << std::showpoint << std::setprecision(9)
		   << "freq["  << std::setw(3) << i << "] = " << h.freq_[i] 
		   << "\tcum[" << std::setw(3) << i << "] = " << h.cum_freq_[i] 
		   << std::endl;
	}

	// restore old output formmating
	os.precision(oldprecision);	
	os.fill(oldfill);			
	os.flags(oldflags);

	return os;
}


void histogram::Insert(const size_t& value) {
	freq_[value] += 1.0 / num_elements_;
	for (size_t i = value; i < cum_freq_.size(); ++i) {
		cum_freq_[i] += 1.0 / num_elements_;
	}
}

void histogram::SetNumElements(const size_t& num_elements) { 
	num_elements_ = num_elements; 
}

void histogram::SetNumBins(const size_t& bins) { 
	std::vector<double>(bins + 1, 0).swap(freq_);
	std::vector<double>(bins + 1, 0).swap(cum_freq_);
}

double histogram::GetExpectedValue() const {
	double expected_value = 0;

	// calculate expected value
	for (size_t i = 0; i < freq_.size(); ++i) {
		expected_value += freq_[i] * i;
	}

	return expected_value;
}

double histogram::GetVariance() const {
	double variance = 0, expected_value = GetExpectedValue();

	// calculate variance
	for (size_t i = 0; i < freq_.size(); ++i) {
		variance += freq_[i] * pow(i - expected_value, 2);
	}

	return variance;
}

double histogram::GetStandardDeviation() const {
	return sqrt(GetVariance());
}

double histogram::FrequencyOf           (const size_t& value) const { return     freq_[value]; }
double histogram::CumulativeFrequencyOf (const size_t& value) const { return cum_freq_[value]; }

// idea from http://www.mathworks.com/help/matlab/ref/conv.html#bucr92l-2
histogram histogram::GetConvolution(const histogram& v) const {
	const histogram& u = *this;
	histogram w(u.num_elements_ + v.num_elements_, u.freq_.size() + v.freq_.size() - 2);

	// get frequencies
	for (size_t k = 0; k < w.freq_.size(); ++k) {
		for (size_t j = 0; j < w.freq_.size(); ++j) {
			if (j < u.freq_.size() && (k - j) < v.freq_.size()) {
				w.freq_[k] += u.freq_[j] * v.freq_[k - j];
			}
		}
	}

	// get cumulatives
	for (size_t k = 0; k < w.freq_.size(); ++k) {
		for (size_t j = 0; j <= k; ++j) {
			w.cum_freq_[k] += w.freq_[j];
		}
	}

	return w;
}

} // statistics namespace