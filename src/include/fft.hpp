// This header file contains fast Fourier transform functions

#pragma once

#include <complex>
#include <vector>
#include <algorithm>
#include <stdexcept>

#define comp(a, b) std::complex<long double>(a, b)

namespace fft {

namespace details {
constexpr const long double PI = 3.14159265358979323846264338327950288419716939937510L;

// Swaps real & imaginary parts of every value in the input vector
void flip_all(std::vector<std::complex<long double>>& input) {
    std::transform(input.begin(), input.end(), input.begin(), [](auto val) { return comp(val.imag(), val.real()); });
}

void radix2fft_rec(std::vector<std::complex<long double>>& input, uint32_t size, uint32_t offset=0, uint32_t step=1) {

    // Check input size
    if (input.size() & (input.size() - 1)) {
        throw std::invalid_argument("input array size must be a power of 2");
    }

    // Else calculate the two halves recursively
    // and combine the calculated serieses into one
    // (If size == 1, there is no need for calculations)
    if (size > 2) {
        radix2fft_rec(input, size/2, offset, step*2);
        radix2fft_rec(input, size/2, offset+step, step*2);
    }
    std::vector<std::complex<long double>> temp(size, comp(0, 0));
    for (int i = 0; i < size/2; i++) {
        const uint32_t even = offset + i * step * 2;
        const uint32_t odd = even + step;
        const auto p = input[even];
        const auto q = std::exp(comp(0.0, -1.0) * comp(2.0 * details::PI * i / size, 0.0)) * input[odd];
        temp[i] = p + q;
        temp[i + size/2] = p - q;
    }
    for (int i = 0; i < size; i++) {
        const uint32_t pos = offset + i * step;
        input[pos] = temp[i];
    }
}
} // namespace details

void radix2fft(std::vector<std::complex<long double>>& input, bool inverse=false) {
    if (inverse) {
        details::flip_all(input);
        details::radix2fft_rec(input, input.size());
        std::transform(input.begin(), input.end(), input.begin(), [input](auto val){ return val / comp(input.size(), 0); });
        details::flip_all(input);
    } else {
        details::radix2fft_rec(input, input.size());
    }
}
} // namespace dft