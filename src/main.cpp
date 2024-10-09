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

#define comp(a, b) std::complex<double>(a, b)
#define d(msg) std::cout << msg << "\n" // for debugging

/* Filters out frequencies in a given band from input Fourier series. Band cut gain will be linearly interpolated using gain1 & gain2. Parameters:
    - Fourier series
    - wave header
    - band cut start frequency
    - band cut end frequency
    - band cut gain at the start freqency (0-1)
    - band cut gain at the end frequency (0-1)*/
void band_cut(std::vector<std::complex<double>> &fourier_series, uint32_t sample_rate, uint32_t freq1, uint32_t freq2, double gain1, double gain2) {
    uint32_t bin1 = freq1 * fourier_series.size() / sample_rate;
    uint32_t bin2 = freq2 * fourier_series.size() / sample_rate;
    auto filter = [gain1, gain2, bin1, bin2](auto complex){ 
        static uint32_t bin = 0;
        const double mul = (double)bin++ / (bin2 - bin1);
        const double real = complex.real() * (1 - mul) * gain1 + complex.real() * mul * gain2;
        const double imag = complex.imag() * (1 - mul) * gain1 + complex.imag() * mul * gain2;
        return comp(real, imag);
    };
    std::transform(fourier_series.begin() + bin1, fourier_series.begin() + bin2, fourier_series.begin() + bin1, filter);
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
        std:: cout << "audio input & output files must be .wav format" << std::endl;
        return 0;
    }

    // Read input file to memory
    AudioFile<double> audio;
    audio.load(argv[1]);

    if (audio.getNumChannels() != 1) {
        std::cout << "this program supports only mono audio" << std::endl;
        return 0;
    }

    // UI
    std::cout << "Greetings!\n";
    std::cout << "\nPLEASE NOTE that currently the program is not working correctly!\nInstead of filtering out frequencies, it amplifies them...\nTo get any interesting output from the program, select only a small band to be 'filtered out'.\nFor example, try inputs 500 0.5 & 501 0.5.\n\n";

    bool correct_input = false;
    uint32_t freq1, freq2;
    double gain1, gain2;

    while (!correct_input) {
        std::cout << "Low freq & gain: ";
        std::cin >> freq1 >> gain1;

        std::cout << "high freq & gain: ";
        std::cin >> freq2 >> gain2;

        correct_input = freq1 <= audio.getSampleRate()/2 && freq2 <= audio.getSampleRate()/2 && gain1 >= 0 && gain1 <= 1 && gain2 >= 0 && gain2 <= 1;

        if (!correct_input) {
            std::cout << "Freq must be between 0 and " << audio.getSampleRate()/2 << ". Gain must be between 0 and 1" << std::endl;
        }
    }

    if (freq2 < freq1) {
        std::swap(freq1, freq2);
        std::swap(gain1, gain2);
    }

    // Transform the sample vector to it's discrete Fourier series
    auto fourier_series = fft::radix2fft(audio.samples[0]);

    d("\nAudio transformed to Fourier series..");

    // Filter users freqs here
    band_cut(fourier_series, audio.getSampleRate(), freq1, freq2, gain1, gain2);
    band_cut(fourier_series, audio.getSampleRate(), freq1 + audio.getSampleRate()/2, freq2 + audio.getSampleRate()/2, gain1, gain2);

    d("Filter applied..");

    // Inverse FFT
    auto output = fft::radix2fft_inverse(fourier_series);

    d("Fourier series transformed back to audio samples..");

    // Cut extra samples
    output.resize(audio.getNumSamplesPerChannel());

    // Write out
    audio.samples[0] = output;
    audio.save("./" + out_name, AudioFileFormat::Wave);

    d("Outfile written");

    return 0;
}