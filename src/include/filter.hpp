#pragma once

#include <vector>
#include <complex>
#include <algorithm>
#include <cmath>
#include <string>
#include <stdexcept>

#define comp(a, b) std::complex<double>(a, b)

namespace ctf {

struct Band {
    uint32_t freq1, freq2;
    double gain1, gain2;
};

// Calculate gain values between frequencies
double interpolate(int bin_curr, int bin1, int bin2, double gain1, double gain2, std::string curve="log") {
    const double lin_ratio = (double)(bin_curr - bin1) / (bin2 - bin1);
    const double log_ratio = log(1 + lin_ratio * (M_E - 1));
    return gain1 + (gain2 - gain1) * (curve == "log" ? log_ratio : lin_ratio);
}

// Filters out frequencies in a given band from input Fourier series.
// Band cut gain will be logarithmically or linearly interpolated between given gain values.
void rm_freqs(std::vector<std::complex<double>> &fourier_series, uint32_t sample_rate, Band band, std::string curve="log") {
    if (band.freq1 == band.freq2) return;

    uint32_t bin1 = band.freq1 * fourier_series.size() / sample_rate;
    uint32_t bin2 = band.freq2 * fourier_series.size() / sample_rate;

    // Cut given & mirrored freqs
    for (auto bin = bin1; bin <= bin2; bin++) {
        const auto bin_mirror = fourier_series.size() - bin;
        const double gain = interpolate(bin, bin1, bin2, band.gain1, band.gain2);

        fourier_series[bin] *= gain;
        fourier_series[bin_mirror] *= gain;
    }
}

// Apply gradual roll off to both ends of a frequency band
// 'roll_amount' sets the length of the roll off i.e. number of frequency bins affected
// Rolling off happens only next to the frequency bands, the bands are not affected
// Roll off curve is linear (ideally it probably should be S or F shaped)
void roll_off(std::vector<std::complex<double>> &fourier_series, uint32_t sample_rate, Band band, uint32_t roll_amount) {
    if (roll_amount == 0) return;

    Band low_roll(band.freq1 - roll_amount, band.freq1, 1, band.gain1);
    if (band.freq1 < roll_amount) {
        low_roll.freq1 = 0;
        low_roll.gain1 = interpolate(0, (int)band.freq1 - roll_amount, band.freq1, 1, band.gain1, "lin");
    }

    Band high_roll(band.freq2, band.freq2 + roll_amount, band.gain2, 1);
    if (band.freq2 + roll_amount > sample_rate / 2) {
        high_roll.freq2 = sample_rate / 2;
        high_roll.gain2 = interpolate(sample_rate / 2, band.freq2, band.freq2 + roll_amount, band.gain2, 1, "lin");
    }
    rm_freqs(fourier_series, sample_rate, low_roll, "lin");
    rm_freqs(fourier_series, sample_rate, high_roll, "lin");
}

void band_cut(std::vector<std::complex<double>> &fourier_series, uint32_t sample_rate, Band band, uint32_t roll) {
    rm_freqs(fourier_series, sample_rate, band);
    roll_off(fourier_series, sample_rate, band, roll);
}
} // Namespace ctf