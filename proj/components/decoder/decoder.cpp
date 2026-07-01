#include <stdio.h>
#include "decoder.h"
#include <stdbool.h>
#include "esp_heap_caps.h"
#include "micro_flac/flac_decoder.h"

using namespace micro_flac;
FLACDecoder dec;
const size_t buffer_size = 4 * 1024 * 1024;
size_t valid_bytes;
size_t readBytes;
size_t freeBytes = 0;

void decode_song(const char* filePath){
    FILE* f = fopen64(filePath, "rb");

    uint8_t* buffer = (uint8_t*)heap_caps_malloc(input_len, MALLOC_CAP_SPIRAM);
    
    //initial read of buffer_size = 1 MiB
    readBytes = fread(buffer, 1, buffer_size, f);
    valid_bytes = readBytes;

    uint8_t* output = nullptr;
    size_t outputSizeBytes = 0;
    bool can_read = true;

    //function to decode song
    while(can_read == true || valid_bytes > 0){
        size_t bytesConsumed = 0;
        size_t samplesDecoded = 0;

        auto result = dec.decode(buffer,
            valid_bytes,
            output,
            outputSizeBytes,
            bytesConsumed,
            samplesDecoded );
        

        if(bytesConsumed > valid_bytes){
            printf("Error more bytes consumed than data given.");
            break;
        }
        size_t left_over_bytes = valid_bytes - bytesConsumed;

        if(valid_bytes == 0 && bytesConsumed == 0){
            //stop process
            can_read = false;
            break;
        }

        //refills buffer by moving the left over bytes to the front
        if(valid_bytes > 0 && bytesConsumed > 0){
            memmove(buffer, buffer + bytesConsumed, left_over_bytes);
        }
        //valid bytes are now equal to the left overs to continue decoding.
        valid_bytes = left_over_bytes;
        freeBytes = buffer_size - valid_bytes;
        
        if(can_read){
            //now we need to read the amount left
            readBytes = fread(buffer + valid_bytes, 1, freeBytes, f);
        }
        
        valid_bytes = valid_bytes + readBytes;
    }
    fclose(f);
}

void refill(size_t data_length){
    
}

void consume(size_t data, size_t bites){
    data -= bites;
    printf(data);

    if(data < 124){
        refill()
    }
}