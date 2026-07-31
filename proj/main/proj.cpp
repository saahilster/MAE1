#include <stdio.h>
#include "reader.h"
#include "decoder.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "I2S.h"

extern "C" void app_main(void)
{
    initialize_bus();
    // initialize();
    // run();
    test_silence();
}
