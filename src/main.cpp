#include <iostream>
#include <string>
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>

#include "AudioFile.h"

#include "./include/fft.hpp"
#include "./include/filter.hpp"
#include "./include/io.hpp"

#define comp(a, b) std::complex<double>(a, b)

void verbose_msg(const bool verbose, const std::string msg) {
    if (verbose) std::cout << msg << "\n";
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "Usage: ctf infile.wav [options]\n"
                     "\t-o <filename.wav> for user defined output filename (defaults to out.wav)\n"
                     "\t-v to run in verbose mode"<< std::endl;
        return 0;
    }

    std::string in_name = argv[1];
    std::string out_name = "out.wav";

    bool verbose = false;

    std::string arg;

    for (int i = 2; i < argc; i++) {
        arg = argv[i];

        if (arg.compare("-o") == 0) {
            out_name = argv[++i];
        } else if (arg.compare("-v") == 0) {
            verbose = true;
        }
    }

    if ((in_name.ends_with(".wav") == false && in_name.ends_with(".WAV") == false) ||
        (out_name.ends_with(".wav") == false && out_name.ends_with(".WAV") == false)) {
        std:: cout << "Audio input & output files must be .wav format" << std::endl;
        return 0;
    }

    AudioFile<double> audio;
    if(!audio.load(argv[1])) {
        std::cerr << "Failed to load " << in_name << "\n";
        return 1;
    }

    if (audio.getNumChannels() != 1) {
        std::cerr << "This program supports only mono audio" << std::endl;
        return 1;
    }

    auto freq_bands = ctf::get_user_input(audio.getSampleRate());
    verbose_msg(verbose, "\nProcessing..");

    auto fourier_series = ctf::radix2fft(audio.samples[0]);
    verbose_msg(verbose, "Audio transformed to Fourier series..");

    for (const auto& band : freq_bands) {
        ctf::band_cut(fourier_series, audio.getSampleRate(), band);
    }
    verbose_msg(verbose, "Filter applied..");

    auto output = ctf::radix2fft_inverse(fourier_series);
    verbose_msg(verbose, "Fourier series transformed back to audio samples..");

    output.resize(audio.getNumSamplesPerChannel());
    audio.samples[0] = output;
    audio.save("./" + out_name, AudioFileFormat::Wave);
    verbose_msg(verbose, "Outfile written");

    return 0;
}
