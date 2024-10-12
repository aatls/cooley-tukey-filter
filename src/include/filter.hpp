#pragma once

#include <vector>
#include <complex>
#include <algorithm>

#define comp(a, b) std::complex<double>(a, b)

namespace ctf {

// Filters out frequencies in a given band from input Fourier series. Band cut gain will be linearly interpolated using gain1 & gain2.
void band_cut(std::vector<std::complex<double>> &fourier_series, uint32_t sample_rate, uint32_t freq1, uint32_t freq2, double gain1, double gain2) {

    uint32_t bin1 = freq1 * fourier_series.size() / sample_rate;
    uint32_t bin2 = freq2 * fourier_series.size() / sample_rate;

    auto interpolate = [gain1, gain2, bin1, bin2](uint32_t bin){
        double ratio = (bin - bin1) / (bin2 - bin1);
        return gain1 + gain2 * ratio;
    };

    // Cut given freqs
    for (auto bin = bin1; bin <= bin2; bin++) {
        fourier_series[bin] *= interpolate(bin);
    }

    // Cut mirrored freqs over Nyquist freq
    for (auto bin = fourier_series.size() - bin2; bin <= fourier_series.size() - bin1; bin++) {
        fourier_series[bin] *= interpolate(bin2 - (fourier_series.size() - bin));
    }
}
} // Namespace ctf