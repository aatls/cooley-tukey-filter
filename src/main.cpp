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

void print_usage() {
    std::cout << "Usage: ctf infile.wav [freq bands] [options]\n"
        "\nFreq bands:\n"
        "\tIn non-interactive mode (default), give frequency bands to be filtered out\n"
        "\tin the following format: <band(low-frequency low-gain high-frequency high-gain)>\n"
        "\tFrequency values should be between 0 & sampling frequency / 2, and gain values between 0 & 1\n"
        "\tFor example, a simple low-cut filter: 'band(0 0 100 0)'\n"
        "\nOptions:\n"
        "\t-o <filename.wav> for user defined output filename (defaults to out.wav)\n"
        "\t-r <amount> to set frequency cut roll off amount in hertz (defaults to 50)\n"
        "\t-i to run in interactive mode\n"
        "\t-v to run in verbose mode\n"
        "\t-h print this help message"<< std::endl;
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        print_usage();
        return 0;
    }

    std::string in_name = argv[1];
    std::string out_name = "out.wav";

    bool verbose = false;
    bool interactive = false;
    int roll_amount = 50;
    std::vector<ctf::Band> freq_bands;

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h") {
            print_usage();
            return 0;
        } else if (arg == "-o") {
            out_name = argv[++i];
        } else if (arg == "-v") {
            verbose = true;
        } else if (arg == "-i") {
            interactive = true;
        } else if (arg == "-r") {
            int roll_input = std::stoi(argv[++i]);
            if (roll_input < 0) {
                std::cerr << "Roll off value should be non-negative" << std::endl;
                return 1;
            }
            roll_amount = roll_input;
        } else if (arg == "band") {
            ctf::Band band;
            band.freq1 = std::stoi(argv[++i]);
            band.gain1 = std::stod(argv[++i]);
            band.freq2 = std::stoi(argv[++i]);
            band.gain2 = std::stod(argv[++i]);
            freq_bands.push_back(band);
        }
    }

    if ((in_name.ends_with(".wav") == false && in_name.ends_with(".WAV") == false) ||
        (out_name.ends_with(".wav") == false && out_name.ends_with(".WAV") == false)) {
        std:: cerr << "Audio input & output files must be .wav format" << std::endl;
        return 1;
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

    if (freq_bands.size() == 0 && interactive == false) {
        std::cerr << "Give frequency bands as parameters or run in interactive mode" << std::endl;
        return 1;
    }

    if (ctf::validate_input(freq_bands, audio.getSampleRate(), roll_amount)) {
        return 1;
    }

    if (interactive) {
        freq_bands = ctf::get_user_input(audio.getSampleRate());
    }
    verbose_msg(verbose, "Processing..");

    auto fourier_series = ctf::radix2fft(audio.samples[0]);
    verbose_msg(verbose, "Audio transformed to Fourier series..");

    for (const auto& band : freq_bands) {
        ctf::band_cut(fourier_series, audio.getSampleRate(), band, roll_amount);
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
