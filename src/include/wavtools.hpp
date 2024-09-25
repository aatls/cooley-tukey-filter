#pragma once

#include <fstream>
#include <iostream>

namespace wavtools {
typedef struct WAV_HEADER
{
    char riff[4];
    uint32_t chunk_size;
    char wave[4];
    char fmt[4]; 
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data[4];
    uint32_t subchunk2_size;

} wav_hdr;

// Reads wav header into wav_hdr struct & wav data into char array
int read_wav(FILE* infile, wav_hdr* header, char** data)
{
    size_t elems_read = fread(header, sizeof(wav_hdr), 1, infile);
    if (elems_read == 0)
    {
        std::cout << "unable to read header data" << std::endl;
        return 1;
    }

    if (header->num_channels != 1)
    {
        std::cout << "this program supports only mono audio" << std::endl;
        return 1;
    }

    *data = (char*) malloc(header->subchunk2_size);
    if (*data == nullptr)
    {
        std::cout << "memory allocation failed" << std::endl;
        return 1;
    }
    elems_read = fread(*data, header->subchunk2_size, 1, infile);
    if (elems_read == 0)
    {
        std::cout << "unable to read audio data" << std::endl;
        return 1;
    }
    return 0;
}

// Writes wav header data & wav data onto an output file
int write_wav(const char* filename, wav_hdr* header, char* data)
{
    FILE* outfile = fopen(filename, "wb");
    if (outfile == nullptr)
    {
        std::cout << "error while creating an output file" << std::endl;
        return 1;
    }
    
    size_t elems_written = fwrite(header, sizeof(wav_hdr), 1, outfile);
    if (elems_written == 0)
    {
        std::cout << "unable to write heared data" << std::endl;
        fclose(outfile);
        return 1;
    }
    
    elems_written = fwrite(data, header->subchunk2_size, 1, outfile);
    if (elems_written == 0)
    {
        std::cout << "unable to write audio data" << std::endl;
        fclose(outfile);
        return 1;
    }
    
    fclose(outfile);
    return 0;
}
} // namespace oma