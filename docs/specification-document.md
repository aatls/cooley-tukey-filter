# Tiralabra 2024 1st period

## General info

- I will use C++ to implement this program.
- I will be able to review also Python an Java code.
- I will implement the fast Fourier transform (Cooley-Tukey) as well as the inverse fast Fourier transform in my project.
- My projects goal is to apply a digital filter to an input audio file.
- The program will recieve an input audio file (.wav format) & a range of frequencies to be filtered out completely or partially. The UI of the application will be text based. The program will output an audio file (.wav) that the users filter has been applied to.
- The time and space complexities of the program will be O(nlogn) where n is the number of input audio samples.
- The main source is [wikipedia.org](https://en.wikipedia.org/wiki/Cooley-Tukey_FFT_algorithm). I will also use miscellanious sources that are online and available to the public.

## The core of the program

My implementation will calculate the discrete Fourier series of the input audio signal. This will be done using the Cooley-Tukey algorithm, which is one of the most popular implementations of the discrete Fourier transform. With the discrete Fourier series calcultated in memory, I will be able to do linear operations to the different frequency bins and lower the coefficients of the user specified frequencies. The inverse discrete Fourier transform will be applied to the manipulated Fourier series to create the output audio signal.

## Misc

All documents and code commenting will be done in english.
Matemaattisten tieteiden kandiohjelma.