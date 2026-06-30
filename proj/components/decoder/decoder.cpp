#include <stdio.h>
#include "decoder.h"
#include <stdbool.h>
#include "esp_heap_caps.h"
#include "micro_flac/flac_decoder.h"

using namespace micro_flac;
FLACDecoder dec;
const size_t buffer_size = 4 * 1024 * 1024;
size_t valid_data;



void decode_song(const char* filePath){

    size_t input_len = 4 * 1024 * 1024;
    
    fseek(fptr, 0, SEEK_END);
    rewind(fptr);

    uint8_t* input = (uint8_t*)heap_caps_malloc(input_len, MALLOC_CAP_SPIRAM);
    size_t data = fread(input, 1, input_len, fptr);

    uint8_t* output = nullptr;
    size_t outputSizeBytes = 0;

    //function to decode song
    while(have_data){
        size_t bytesConsumed = 0;
        size_t samplesDecoded = 0;

        auto result = dec.decode(input,
            data,
            output,
            outputSizeBytes,
            bytesConsumed,
            samplesDecoded );

        input += bytesConsumed;
        input_len -= bytesConsumed;
        
    }
    fclose(fptr);
}

void refill(){

}

void consume(){
    
}