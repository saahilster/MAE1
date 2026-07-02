#include <stdio.h>
#include "decoder.h"
#include <stdbool.h>
#include <cstring>
#include "esp_heap_caps.h"
#include "micro_flac/flac_decoder.h"

using namespace micro_flac;

FLACDecoder dec;

const size_t buffer_size = 4 * 1024 * 1024;

void decode_song(const char *filePath)
{
    FILE *f = fopen(filePath, "rb");
    if (!f)
    {
        printf("failed to open file\n");
        return;
    }

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

    size_t outputSizeSamples = 0;
    int32_t *output = nullptr;

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

        // Because the next decode call starts at buffer,
        // move the leftover bytes to the front.
        if (leftOver > 0 && bytesConsumed > 0)
        {
            memmove(buffer, buffer + bytesConsumed, leftOver);
        }

        if (result == FLAC_DECODER_HEADER_READY)
        {
            outputSizeSamples = dec.get_output_buffer_size_samples(); 

            if (!output)
            {
                output = (int32_t *)heap_caps_malloc(outputSizeSamples * sizeof(int32_t), MALLOC_CAP_SPIRAM);

                if (!output)
                {
                    printf("failed to allocate output buffer\n");
                    done = true;
                }
            }
        }
        else if (result == FLAC_DECODER_SUCCESS)
        {
            // PROCESS PCM HERE LATER
            // output contains decoded audio
            // samplesDecoded tells you how many samples are valid

            //Test out if it prints.
            size_t pcmChunk = samplesDecoded * outputSizeSamples;
            printf("\n%zu\n", pcmChunk);
        }
        else if (result == FLAC_DECODER_NEED_MORE_DATA)
        {
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