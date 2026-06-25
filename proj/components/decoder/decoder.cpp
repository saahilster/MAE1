#include <stdio.h>
#include "decoder.h"
#include "esp_heap_caps.h"
#include "micro_flac/flac_decoder.h"

using namespace micro_flac;
FLACDecoder dec;

void decode_song(const char* filePath){

    FILE* fptr = fopen(filePath, "rb");
    fseek(fptr, 0, SEEK_END);
    size_t input_len = ftell(fptr);
    rewind(fptr);

    if (fptr == NULL){
        printf("Cannot find file.");
        return;
    }
    
    uint8_t* input = (uint8_t*)heap_caps_malloc(input_len, MALLOC_CAP_SPIRAM);
    fread(input, 1, input_len, fptr);
    fclose(fptr);
}