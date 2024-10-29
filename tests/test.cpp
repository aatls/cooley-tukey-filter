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

TEST_CASE("Roll-off works" "[ctf::roll_off]") {
    auto rolled(test_vector);
    constexpr const int f1 = 1000, f2 = 2000, roll = 500;
    constexpr const double g1 = 0, g2 = 0.5;
    const ctf::Band band (f1, f2, g1, g2);

    ctf::roll_off(rolled, sample_rate, band, roll);

    // Before 1st roll
    for (int i = 1; i < f1 - roll; i++) {
        const auto corr = comp(1, 1);

        REQUIRE(close_enough(rolled[i], corr));
        REQUIRE(close_enough(rolled[sample_rate - i], corr));
    }
    // 1st roll
    for (int i = f1 - roll + 1; i < f1; i++) {
        const double correct_gain = 1 - (f1 - i) / roll;
        const auto corr = comp(correct_gain, correct_gain);

        REQUIRE(close_enough(rolled[i], corr));
        REQUIRE(close_enough(rolled[sample_rate - i], corr));
    }
    // Before 2nd roll
    for (int i = f1 + 1; i < f2; i++) {
        const auto corr = comp(1, 1);

        REQUIRE(close_enough(rolled[i], corr));
        REQUIRE(close_enough(rolled[sample_rate - i], corr));
    }
    // 2nd roll
    for (int i = f2; i < f2 + roll; i++) {
        const double correct_gain =  1 - g2 * (f2 - i) / roll;
        const auto corr = comp(correct_gain, correct_gain);

        REQUIRE(close_enough(rolled[i], corr));
        REQUIRE(close_enough(rolled[sample_rate - i], corr));
    }
    // After 2nd roll
    for (int i = f2 + roll; i < sample_rate / 2; i++) {
        const auto corr = comp(1, 1);

        REQUIRE(close_enough(rolled[i], corr));
        REQUIRE(close_enough(rolled[sample_rate - i], corr));
    }
}

TEST_CASE("Band-cut argument validation" "[ctf::band_cut]") {
    auto test_v(test_vector);
    ctf::Band test_b;

    test_b = ctf::Band(0, 30000, 0, 0);
    REQUIRE_THROWS(ctf::band_cut(test_v, sample_rate, test_b, 0));

    test_b = ctf::Band(1000, 500, 0, 0);
    REQUIRE_THROWS(ctf::band_cut(test_v, sample_rate, test_b, 0));

    test_b = ctf::Band(0, 1, -1, 0);
    REQUIRE_THROWS(ctf::band_cut(test_v, sample_rate, test_b, 0));

    test_b = ctf::Band(0, 1, 0, 2);
    REQUIRE_THROWS(ctf::band_cut(test_v, sample_rate, test_b, 0));

    test_b = ctf::Band(0, 1, 0, 0);
    REQUIRE_THROWS(ctf::band_cut(test_v, sample_rate, test_b, -1));
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

// --------------------- TEST IO --------------------

// do that