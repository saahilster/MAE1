#include <stdio.h>
#include "decoder.h"
#include <stdbool.h>
#include "esp_heap_caps.h"
#include "micro_flac/flac_decoder.h"

using namespace micro_flac;
FLACDecoder dec;

void decode_song(const char* filePath){

    bool have_data = true;

    FILE* fptr = fopen(filePath, "rb");
    fseek(fptr, 0, SEEK_END);



    //input_len should be chunked for reading
    //set to 4MiB
    size_t input_len = 4 * 1024 * 1024;

    rewind(fptr);

    if (fptr == NULL){
        printf("Cannot find file.");
        return;
    }
    
    uint8_t* input = (uint8_t*)heap_caps_malloc(input_len, MALLOC_CAP_SPIRAM);
    fread(input, 1, input_len, fptr);
    fclose(fptr);

    //function to decode song
    while(have_data){

    }
}