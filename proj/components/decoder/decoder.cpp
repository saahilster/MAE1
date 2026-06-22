#include <stdio.h>
#include "decoder.h"
#include "micro_flac/flac_decoder.h"

using namespace micro_flac;
FLACDecoder dec;


void decode_song(const char *songName){
    uint8_t* output = nullptr;
    size_t output_size_bytes = 0;
}