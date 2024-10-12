#include <vector>
#include <complex>
#include <cstdlib>

#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include "../src/include/filter.hpp"
#include "../src/include/fft.hpp"
// #include "../src/main.cpp"

#define comp(a, b) std::complex<double>(a, b)

// Generate a complex vector for testing
constexpr const uint16_t sample_rate = 44100;
std::vector<std::complex<double>> test_vector(sample_rate, comp(1.0,1.0));

// Utility functions
bool close_enough(double d1, double d2) {
    return abs(d1 - d2) < 1e-9;
}

bool close_enough(std::complex<double> c1, std::complex<double> c2) {
    return close_enough(c1.real(), c2.real()) && close_enough(c1.imag(), c2.imag());
}

// ----------- TEST FILTER ----------

constexpr const uint32_t f1 = 1000;
constexpr const uint32_t f2 = 10000;

TEST_CASE("Input freqs are cut" "[ctf::band_cut]") {
    auto filtered(test_vector);
    ctf::band_cut(filtered, sample_rate, f1, f2, 0, 0);

    for (uint32_t i = 0; i < filtered.size(); i++) {
        if (i >= f1 && i <= f2) {
            REQUIRE(close_enough(filtered[i], comp(0,0)));
        }
    }
}

TEST_CASE("Mirrored freqs are cut" "[ctf::band_cut]") {
    auto filtered(test_vector);
    ctf::band_cut(filtered, sample_rate, f1, f2, 0, 0);

    for (uint32_t i = 0; i < filtered.size(); i++) {
        if (i >= sample_rate - f2 && i <= sample_rate - f1) {
            REQUIRE(close_enough(filtered[i], comp(0,0)));
        }
    }
}

TEST_CASE("Outside freqs are untouched" "[ctf::band_cut]") {
    auto filtered(test_vector);
    ctf::band_cut(filtered, sample_rate, f1, f2, 0, 0);

    for (uint32_t i = 0; i < filtered.size(); i++) {
        if ((i < f1 && i > f2) && (i < sample_rate - f2 && i > sample_rate - f1)) {
            REQUIRE(close_enough(filtered[i], comp(1,1)));
        }
    }
}