# weekly report 4

Used time this week ~18h

- This week I have focused on file IO & making the program usable.
- The program can now read & write .wav files (decided to limit input files to only 16bit mono audio for simplicity, I'll leave one such audio file in the repo for testing). The program can also read user input & modify the input audio.
- This week I have learned about some features of C++, memory management, file IO. Bytes are fun to work with :)
- There is one major issue in my program. Instead of filtering out the given freq range, the program amplifies it... There might be something wrong with my DFT algorithms, or maybe I have misunderstood something about the discrete Fourier series. This amplification causes integer overflow and major distortion in the output audio.
- Next week I'll start unit testing & possibly refactor the main program to smaller functions. I'll also try to get my program working correctly.