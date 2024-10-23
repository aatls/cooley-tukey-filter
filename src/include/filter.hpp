#pragma once

#include <vector>
#include <complex>
#include <algorithm>
#include <cmath>

#define comp(a, b) std::complex<double>(a, b)

namespace ctf {

struct Band {
    const uint32_t freq1, freq2;
    const double gain1, gain2;
};

// Filters out frequencies in a given band from input Fourier series.
// Band cut gain will be logarithmically interpolated between given gain values.
void band_cut(std::vector<std::complex<double>> &fourier_series, uint32_t sample_rate, Band band) {

    uint32_t bin1 = band.freq1 * fourier_series.size() / sample_rate;
    uint32_t bin2 = band.freq2 * fourier_series.size() / sample_rate;

    auto interpolate = [band, bin1, bin2](uint32_t bin){
        const double ratio = (double)(bin - bin1) / (bin2 - bin1);
        const double log_ratio = log(1 + ratio * (M_E - 1));
        return band.gain1 + (band.gain2 - band.gain1) * log_ratio;
    };

    // Cut given & mirrored freqs
    for (auto bin = bin1; bin <= bin2; bin++) {
        const auto bin_mirror = fourier_series.size() - bin;
        const double gain = interpolate(bin);

        fourier_series[bin] *= gain;
        fourier_series[bin_mirror] *= gain;
    }
}
} // Namespace ctf