# Cooley-Tukey Filter user manual

### Building the program

See [README.md](../README.md)

### Program overview

This program can be used to remove user defined frequencies from an input audio file.

### Parameters

- Input file
- Frequency bands (non-interactive mode)
- Options

#### Input file

The input file must be of WAVE format (.wav suffix). Currently the program supports mono audio only.

You may want to limit the input audio's length. For example filtering 1 minute of audio takes about 12 seconds, 10 minutes takes about 120 seconds.

#### Frequency bands

Frequency band consists of a low frequency f1 and a high frequency f2 and gain values for both frequencies g1 and g2. F1 and f2 determines the start and end of the frequency band. The gain value at f1 will be g1 and at f2 will be g2. Between f1 and f2 the gain value will be interpolated logarithmically using g1 and g2.

If you are running the program in non-interactive mode (default) you must input frequency bands as command line arguments. You should input at least 1 frequency band. Give the bands in the following format "band f1 f2 g1 g2". For example a low cut filter could be given as "band 0 0 200 0".

If you run the program in interactive mode, the frequencies and gain values will be prompted from the user.

Overlapping of frequency bands is not checked. All overlapping bands are applied.

#### Options

- -o &lt;filename.wav&gt; for user defined output filename (defaults to out.wav)
- -r &lt;amount&gt; to set frequency cut roll off amount in hertz (defaults to 50)
- -i to run in interactive mode
- -v to run in verbose mode
- -h to print this help message
