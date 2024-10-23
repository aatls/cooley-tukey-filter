#pragma once

#include <complex>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>

#define comp(a, b) std::complex<double>(a, b)

namespace ctf {
namespace details {

// Swaps real & imaginary parts of every value in the input vector
void flip_all(std::vector<std::complex<double>>& input) {
    std::transform(input.begin(), input.end(), input.begin(), [](auto val) { return comp(val.imag(), val.real()); });
}

// Naive DFT used for testing
void dft_naive(std::vector<std::complex<double>>& input) {
    std::vector<std::complex<double>> transform (input.size());
    
    for (int i = 0; i < input.size(); i++) {
        for (int j = 0; j < input.size(); j++) {
            auto ex = std::exp(comp(0, -2 * M_PI * i * j / input.size()));
            transform[i] += input[j] * ex;
        }
    }

    input = transform;
}

void radix2fft_rec(std::vector<std::complex<double>>& input, uint32_t size, uint32_t offset=0, uint32_t step=1) {

    if (size == 1) return;

    // Calculate the two halves recursively
    // and combine the calculated serieses into one
    radix2fft_rec(input, size/2, offset, step*2);
    radix2fft_rec(input, size/2, offset+step, step*2);

    std::vector<std::complex<double>> temp(size, comp(0, 0));
    for (int i = 0; i < size/2; i++) {
        const uint32_t even = offset + i * step * 2; 
        const uint32_t odd = even + step;
        const auto p = input[even];
        const auto q = std::exp(comp(0, -2.0 * M_PI * (double)i / (double)size)) * input[odd];
        temp[i] = p + q;
        temp[i + size/2] = p - q;
    }

    for (int i = 0; i < size; i++) {
        const uint32_t pos = offset + i * step;
        input[pos] = temp[i];
    }
}
} // namespace details

// Input: vector of samples
// Output: Fourier series of input vector, size extended to nearest 2^n value
std::vector<std::complex<double>> radix2fft(std::vector<double>& samples) {
    std::vector<std::complex<double>> output_series;
    for (auto sample : samples) {
        output_series.push_back(comp(sample, 0));
    }

    // Expand the sample vector to 2^n values
    uint32_t up = std::pow(2, std::ceil(std::log2(samples.size())));
    output_series.insert(output_series.end(), up - output_series.size(), comp(0, 0));


    details::radix2fft_rec(output_series, output_series.size());

    return output_series;
}

// Input: Fourier series
// Output: vector of audio samples
std::vector<double> radix2fft_inverse(std::vector<std::complex<double>>& fourier_series) {
    if (fourier_series.size() & (fourier_series.size() - 1)) {
        throw std::invalid_argument("Fourier series array size must be a power of 2");
    }

    // Use a trick to calculate the inverse Fourier transform
    details::flip_all(fourier_series);
    details::radix2fft_rec(fourier_series, fourier_series.size());
    std::transform(fourier_series.begin(), fourier_series.end(), fourier_series.begin(), [fourier_series](auto val){ return val / comp(fourier_series.size(), 0); }); // Don't know why this has to be done..
    details::flip_all(fourier_series);

    std::vector<double> output_samples;
    for (auto complex : fourier_series) {
        output_samples.push_back(complex.real());
    }

    return output_samples;
}
} // namespace dft