#include <stdio.h>
#include "reader.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    initialize();
    run();
}
