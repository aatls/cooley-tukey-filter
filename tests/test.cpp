#include <vector>
#include <complex>
#include <cstdlib>
#include <random>
#include <iostream> // debugging

// Testing framework
#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include "../src/include/filter.hpp"
#include "../src/include/fft.hpp"

#define comp(a, b) std::complex<double>(a, b)

// Utility functions
bool close_enough(double d1, double d2) {
    return abs(d1 - d2) < 1e-9;
}

bool close_enough(std::complex<double> c1, std::complex<double> c2) {
    return close_enough(c1.real(), c2.real()) && close_enough(c1.imag(), c2.imag());
}

// --------------------- TEST FILTER --------------------

// Generate a complex vector for testing
constexpr const uint16_t sample_rate = 44100;
std::vector<std::complex<double>> test_vector(sample_rate, comp(1.0,1.0));

constexpr const ctf::Band cut_band (100, 10000, 0, 0);
constexpr const ctf::Band partial_cut_band (100, 10000, 0.1, 0.1);

TEST_CASE("Input freqs are cut" "[ctf::rm_freqs]") {
    auto filtered(test_vector);
    ctf::rm_freqs(filtered, sample_rate, cut_band);

    for (uint32_t i = cut_band.freq1; i <= cut_band.freq2; i++) {
        REQUIRE(close_enough(filtered[i], comp(0,0)));
    }
}

TEST_CASE("Mirrored freqs are cut" "[ctf::rm_freqs]") {
    auto filtered(test_vector);
    ctf::rm_freqs(filtered, sample_rate, cut_band);

    for (uint32_t i = sample_rate - cut_band.freq2; i <= sample_rate - cut_band.freq1; i++) {
        REQUIRE(close_enough(filtered[i], comp(0,0)));
    }
}

TEST_CASE("Outside freqs are untouched" "[ctf::rm_freqs]") {
    auto filtered(test_vector);
    ctf::rm_freqs(filtered, sample_rate, cut_band);

    for (uint32_t i = 0; i < filtered.size(); i++) {
        if ((i < cut_band.freq1 && i > cut_band.freq2) && (i < sample_rate - cut_band.freq2 && i > sample_rate - cut_band.freq1)) {
            REQUIRE(close_enough(filtered[i], comp(1,1)));
        }
    }
}

TEST_CASE("Gain reducing works" "[ctf::rm_freqs]") {
    auto filtered(test_vector);
    ctf::rm_freqs(filtered, sample_rate, partial_cut_band);

    for(uint32_t i = partial_cut_band.freq1; i <= partial_cut_band.freq2; i++) {
        REQUIRE(close_enough(filtered[i], comp(partial_cut_band.gain1, partial_cut_band.gain2)));
    }
}

// --------------------- TEST FFT --------------------

TEST_CASE("Flip-all utility works" "[ctf::details::flip_all]") {
    constexpr const double a = 0.5;
    constexpr const double b = 1.5;
    std::vector<std::complex<double>> vec (100, comp(a, b));

    ctf::details::flip_all(vec);

    for (auto c : vec) {
        REQUIRE(c == comp(b, a));
    }
}

TEST_CASE("Input vectors are expanded to the next 2^n size" "[ctf::radix2fft]") {
    constexpr const int start = 1024;
    constexpr const int end = 32768;

    for (int i = start; i <= end; i*=2) {
        std::vector<double> in (i+1, 0);
        auto out = ctf::radix2fft(in);
        REQUIRE(out.size() == i*2);
    }
}

TEST_CASE("FFT recursion works as expected" "[ctf::radix2fft_rec]") {
    constexpr const uint32_t test_size = 1024;

    std::vector<std::complex<double>> in (test_size);

    std::uniform_real_distribution<double> unif(0,1);
    std::default_random_engine re;

    for (int i = 0; i < test_size; i++) {
        in[i] = comp(unif(re), 0);
    }

    std::vector<std::complex<double>> out_ctf (in);
    ctf::details::radix2fft_rec(out_ctf, out_ctf.size());

    auto out_correct = in;
    ctf::details::dft_naive(out_correct);

    for (int i = 0; i < out_ctf.size(); i++) {
        REQUIRE(close_enough(out_ctf[i], out_correct[i]));
    }
}

TEST_CASE("FFT goes around" "[ctf::radix2fft][ctf::radix2fft_inverse]") {
    constexpr const uint32_t test_size = 131072;

    std::vector<double> original (test_size);

    std::uniform_real_distribution<double> unif(0,1);
    std::default_random_engine re;

    for (int i = 0; i < test_size; i++) {
        original[i] = unif(re);
    }

    auto series = ctf::radix2fft(original);
    auto transformed = ctf::radix2fft_inverse(series);

    for (int i = 0; i < test_size; i++) {
        REQUIRE(close_enough(original[i], transformed[i]));
    }
}
