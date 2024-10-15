#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>

#include "AudioFile.h"

#include "./include/fft.hpp"
#include "./include/filter.hpp"

#define comp(a, b) std::complex<double>(a, b)
#define d(msg) std::cout << msg << "\n" // for debugging

void get_user_input(uint32_t sample_rate, uint32_t* freq1, uint32_t* freq2, double* gain1, double* gain2) {
    std::cout << "Greetings!\n\n";
    std::cout << "NOTE that the linear interpolation feature is not working correctly! For good results, choose the same gain value for both gain inputs.\n\n";
    std::cout << "Please enter frequencies between 0 and " << sample_rate/2 << ".\nGain must be between 0 and 1\n\n";

    bool correct_input = false;

    while (!correct_input) {
        std::cout << "Low freq & gain: ";
        std::cin >> *freq1 >> *gain1;

        std::cout << "High freq & gain: ";
        std::cin >> *freq2 >> *gain2;

        correct_input = *freq1 <= sample_rate/2 && *freq2 <= sample_rate/2 && *gain1 >= 0 && *gain1 <= 1 && *gain2 >= 0 && *gain2 <= 1;

        if (!correct_input) {
            std::cout << "\nInput out of range\n\n";
        }
    }

    if (*freq2 < *freq1) {
        std::cout << "Freqs are in the wrong order but ok";
        std::swap(*freq1, *freq2);
        std::swap(*gain1, *gain2);
    }
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "Usage: cooley-tukey-filter infile [-o outfile]\n\tinfile & outfile must be of .wav format" << std::endl;
        return 0;
    }

    std::string in_name = argv[1];
    std::string out_name = "out.wav";

    if (argc > 2) {
        if (std::string_view(argv[2]).compare("-o") == 0) {
            out_name = argv[3];
        }
    }

    if (in_name.ends_with(".wav") == false || std::string_view(out_name).ends_with(".wav") == false) {
        std:: cout << "Audio input & output files must be .wav format" << std::endl;
        return 0;
    }

    // Read input file to memory
    AudioFile<double> audio;
    audio.load(argv[1]);

    if (audio.getNumChannels() != 1) {
        std::cout << "This program supports only mono audio" << std::endl;
        return 0;
    }

    uint32_t freq1, freq2;
    double gain1, gain2;

    get_user_input(audio.getSampleRate(), &freq1, &freq2, &gain1, &gain2);

    // Transform the sample vector to it's discrete Fourier series
    auto fourier_series = ctf::radix2fft(audio.samples[0]);

    d("\nAudio transformed to Fourier series..");

    // Filter users freqs here
    ctf::band_cut(fourier_series, audio.getSampleRate(), freq1, freq2, gain1, gain2);

    d("Filter applied..");

    // Inverse FFT
    auto output = ctf::radix2fft_inverse(fourier_series);

    d("Fourier series transformed back to audio samples..");

    // Cut extra samples
    output.resize(audio.getNumSamplesPerChannel());

    // Write out
    audio.samples[0] = output;
    audio.save("./" + out_name, AudioFileFormat::Wave);

    d("Outfile written");

    return 0;
}
