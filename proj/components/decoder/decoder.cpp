#include <stdio.h>
#include "decoder.h"
#include <stdbool.h>
#include <cstring>
#include "esp_heap_caps.h"
#include "micro_flac/flac_decoder.h"
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "I2S.h"

#include <math.h>

using namespace micro_flac;

FLACDecoder dec;

size_t buffer_size = 1024 * 32;
size_t outputSizeSamples = 0;
int32_t *output = nullptr;

void decode_song(const char *filePath)
{
    output = nullptr;
    outputSizeSamples = 0;
    dec.reset();
    // Work on figuring out the watch dog error
    // Look into FreeRTOS.
    vTaskDelay(10);
    FILE *f = fopen(filePath, "rb");
    if (f == NULL)
    {
        printf("failed to open file\n");
        return;
    }   
    printf("file was found and opened\n");

    uint8_t *buffer = (uint8_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
    if (!buffer)
    {
        printf("failed to allocate input buffer\n");
        fclose(f);
        return;
    }

    // Initial read of buffer_size = 4 MiB
    size_t readBytes = fread(buffer, 1, buffer_size, f);
    size_t validBytes = readBytes;

    bool can_read = true;
    bool done = false;

    while (!done && (can_read || validBytes > 0))
    {

        size_t bytesConsumed = 0;
        size_t samplesDecoded = 0;

        auto result = dec.decode(
            buffer,
            validBytes,
            output,
            outputSizeSamples,
            bytesConsumed,
            samplesDecoded);

        // Consume input bytes.
        validBytes -= bytesConsumed;
        size_t leftOver = validBytes;
        printf("Bytes Consumed: %zu\n", bytesConsumed);
        printf("Data left: %zu\n", leftOver);

        // Because the next decode call starts at buffer,
        // move the leftover bytes to the front.
        if (leftOver > 0 && bytesConsumed > 0)
        {
            printf("Updating Consumtion\n");
            memmove(buffer, buffer + bytesConsumed, leftOver);
            printf("buffer[0..7] = %02X %02X %02X %02X %02X %02X %02X %02X\n",
                   (unsigned char)buffer[0], (unsigned char)buffer[1], (unsigned char)buffer[2], (unsigned char)buffer[3],
                   (unsigned char)buffer[4], (unsigned char)buffer[5], (unsigned char)buffer[6], (unsigned char)buffer[7]);
        }

        if (result == FLAC_DECODER_HEADER_READY)
        {
            printf("Header Ready\n");
            outputSizeSamples = dec.get_output_buffer_size_samples();

            if(output != nullptr){
                heap_caps_free(output);
                output = nullptr;
                outputSizeSamples = 0;
            }
            printf("Updating output\n");
            output = (int32_t *)heap_caps_malloc(outputSizeSamples * sizeof(int32_t), MALLOC_CAP_SPIRAM);

            if (!output)
            {
                printf("failed to allocate output buffer\n");
                done = true;
            }
        }
        else if (result == FLAC_DECODER_SUCCESS)
        {
            // Test out if it prints.
            size_t pcmChunk = samplesDecoded;
            printf("\nChunk: %zu\n", pcmChunk);
            printf("Sample Data: %ld\n", (long)output);

            //test to see if it works
           // transmit(&adjustedSample);

            for (size_t i = 0; i < samplesDecoded; i++){
                output[i] = apply_volume(output[i], 0.000f);
            }

            transmit(output, samplesDecoded);
        }
        else if (result == FLAC_DECODER_NEED_MORE_DATA)
        {
            printf("Attempting to refill data");
            if (can_read)
            {
                size_t freeSpace = buffer_size - leftOver;

                if (freeSpace > 0)
                {
                    readBytes = fread(buffer + leftOver, 1, freeSpace, f);
                    validBytes = leftOver + readBytes;

                    if (readBytes == 0)
                    {
                        can_read = false;
                    }
                }
                else
                {
                    printf("decoder needs more data but input buffer is full\n");
                    done = true;
                }
            }

            if (bytesConsumed == 0 && !can_read)
            {
                printf("file exhausted before decoder finished\n");
                done = true;
            }
        }
        else if (result == FLAC_DECODER_END_OF_STREAM)
        {
            printf("end of stream\n");
            done = true;
        }
        else
        {
            printf("decoder error occurred\n");
            done = true;
        }
    }

    if (output)
    {
        heap_caps_free(output);
    }

    heap_caps_free(buffer);
    fclose(f);
}

int32_t apply_volume(int32_t rawSample, float gain){
    long scale = lroundf((float)rawSample * gain);

    //clamp
    if(scale > INT32_MAX){
        scale = INT32_MAX;
    }
    else if(scale < INT32_MIN){
        scale = INT32_MIN;
    }
    return scale;
}