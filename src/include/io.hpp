#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "filter.hpp"

namespace ctf {
namespace details {

void get_numeric(uint32_t& freq1, uint32_t& freq2, double& gain1, double& gain2) {
    std::cout << "\nLow freq & gain: ";
    std::cin >> freq1 >> gain1;

    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Bad input\n";

        std::cout << "Low freq & gain: ";
        std::cin >> freq1 >> gain1;
    }

    std::cout << "High freq & gain: ";
    std::cin >> freq2 >> gain2;

    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Bad input\n";

        std::cout << "High freq & gain: ";
        std::cin >> freq2 >> gain2;
    }
}
} // Namespace details

// Returns a vector of user defined frequency bands
std::vector<ctf::Band> get_user_input(uint32_t sample_rate) {
    std::cout << "Greetings!\n\n";
    std::cout << "Please enter frequencies between 0 and " << sample_rate/2 << ".\n"
                 "Gain must be between 0 and 1\n";

    std::vector<ctf::Band> bands;
    bool correct_input, ask_freqs = true;

    while (ask_freqs) {
        uint32_t freq1, freq2;
        double gain1, gain2;

        details::get_numeric(freq1, freq2, gain1, gain2);

        correct_input = freq1 <= sample_rate/2 &&
                        freq2 <= sample_rate/2 &&
                        gain1 >= 0 && gain1 <= 1 &&
                        gain2 >= 0 && gain2 <= 1;

        if (!correct_input) {
            std::cout << "\nInput out of range\n";
            continue;
        }

        if (freq2 < freq1) {
            std::cout << "\nPlease enter frequencies so that low freq < high freq\n";
            continue;
        }

        bands.push_back(ctf::Band(freq1, freq2, gain1, gain2));

        std::cout << "Want to enter more frequency bands? (y/n)\n";
        while (true) {
            std::string ans;
            std::cin >> ans;

            // Ans to lowercase
            std::transform(ans.begin(), ans.end(), ans.begin(), [](unsigned char c){return std::tolower(c);});
            
            if (ans == "y" || ans == "yes") {
                ask_freqs = true;
                break;
            } else if (ans == "n" || ans == "no") {
                ask_freqs = false;
                break;
            } else {
                std::cout << "Enter 'yes' or 'no'\n";
            }
        }
    }
    return bands;
}

// Returns false on valid input
bool validate_input(std::vector<ctf::Band> &bands, uint32_t sample_rate, int roll) {
    for (auto band : bands) {
        if (band.freq1 > sample_rate / 2 || band.freq2 > sample_rate / 2) {
            std::cerr << "Band frequencies must be below sample rate / 2" << std::endl;
            return 1;
        }
        if (band.freq1 >= band.freq2) {
            std::cerr << "Low frequency must be below high frequency" << std::endl;
            return 1;
        }
        if (band.gain1 < 0 || band.gain1 > 1 ||
            band.gain2 < 0 || band.gain2 > 1){
            std::cerr << "Gain values must be between 0 & 1" << std::endl;
            return 1;
        }
        if (roll < 0) {
            std::cerr << "Roll off amount must be non-negative" << std::endl;
            return 1;
        }
    }
    return 0;
}
} // Namespace ctf