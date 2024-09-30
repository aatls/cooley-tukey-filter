#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>

#include "./include/fft.hpp"
#include "./include/wavtools.hpp"

#define comp(a, b) std::complex<long double>(a, b)
#define d(msg) std::cout << msg << "\n" // for debugging

// Changes the endianess of 16-bit data array
void change_endianess_16bit(char *data, size_t bytes) {
    for (int i = 0; i < bytes; i += 2) {
        std::swap(data[i], data[i+1]);
    }
}

/* Filters out frequencies in a given band from input Fourier series. Band cut gain will be linearly interpolated using gain1 & gain2. Parameters:
    - Fourier series
    - wave header
    - band cut start frequency
    - band cut end frequency
    - band cut gain at the start freqency (0-1)
    - band cut gain at the end frequency (0-1)*/
void band_cut(std::vector<std::complex<long double>> &samples, wavtools::wav_hdr* header, uint32_t freq1, uint32_t freq2, double gain1, double gain2) {
    uint32_t bin1 = freq1 * samples.size() / header->sample_rate;
    uint32_t bin2 = freq2 * samples.size() / header->sample_rate;
    auto filter = [gain1, gain2, bin1, bin2](auto sample){ 
        static uint32_t bin = 0;
        const long double mul = (double)bin++ / (bin2 - bin1);
        const long double real = sample.real() * (1 - mul) * gain1 + sample.real() * mul * gain2;
        const long double imag = sample.imag() * (1 - mul) * gain1 + sample.imag() * mul * gain2;
        return comp(real, imag);
    };
    std::transform(samples.begin() + bin1, samples.begin() + bin2, samples.begin() + bin1, filter);
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "Usage: cooley-tukey-filter infile [-o outfile]\n\tinfile & outfile must be of .wav format\n\tinfile must be 16-bit mono audio" << std::endl;
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

    FILE* infile = fopen(argv[1], "rb");
    if (infile == nullptr) {
        std::cout << "cannot open " << argv[1] << std::endl;
        return 0;
    }

    // Read input file to memory
    char *data8 = nullptr;
    wavtools::wav_hdr header;

    if (wavtools::read_wav(infile, &header, &data8)) {
        free(data8);
        return 0;
    }

    if (header.num_channels != 1) {
        std::cout << "this program supports only mono audio" << std::endl;
        free(data8);
        return 0;
    }

    if (header.bits_per_sample != 16) {
        std::cout << "this program supports only 16 bit audio" << std::endl;
        free(data8);
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

        correct_input = freq1 <= header.sample_rate && freq2 <= header.sample_rate && gain1 >= 0 && gain1 <= 1 && gain2 >= 0 && gain2 <= 1;

        if (!correct_input) {
            std::cout << "Freq must be between 0 and " << header.sample_rate << ". Gain must be between 0 and 1" << std::endl;
        }
    }

    if (freq2 < freq1) {
        std::swap(freq1, freq2);
        std::swap(gain1, gain2);
    }

    change_endianess_16bit(data8, header.subchunk2_size);

    // Create complex vector to be filled with sample data
    std::vector<std::complex<long double>> samples(header.subchunk2_size / 2);
    for (int i = 0; i < header.subchunk2_size / 2; i++) {
        samples[i] = comp(reinterpret_cast<int16_t*>(data8)[i], 0);
    }
    free(data8);

    // Expand the sample vector to 2^n values
    uint32_t up = std::pow(2, std::ceil(std::log2(samples.size())));
    samples.insert(samples.end(), up - samples.size(), comp(0, 0));

    // Transform the sample vector to it's DFS
    fft::radix2fft(samples);

    // Filter dfs here
    band_cut(samples, &header, freq1, freq2, gain1, gain2);

    // Inverse FFT
    fft::radix2fft(samples, true);

    // Create data array from samples
    int16_t *data16 = (int16_t*)malloc(header.subchunk2_size);
    for (int i = 0; i < header.subchunk2_size / 2; i++) {
        data16[i] = (int16_t)samples[i].real();
    }

    // Prepare data for writing out
    change_endianess_16bit(reinterpret_cast<char*>(data16), header.subchunk2_size);

    wavtools::write_wav(&out_name[0], &header, reinterpret_cast<char*>(data16));

    free(data16);

    return 0;
}