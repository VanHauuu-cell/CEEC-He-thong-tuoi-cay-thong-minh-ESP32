#include "esp_adc/adc_oneshot.h"
#include "config.h"
#include "soil.h"

adc_oneshot_unit_handle_t adc1_handle;

void soil_sensor_init(void) {
    adc_oneshot_unit_init_cfg_t init_config = { .unit_id = ADC_UNIT_1 };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));
    adc_oneshot_chan_cfg_t config = { .bitwidth = ADC_BITWIDTH_DEFAULT, .atten = ADC_ATTEN_DB_12 };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, SOIL_ADC_CHAN, &config));
}

int soil_sensor_read(void) {
    int val = 0;
    adc_oneshot_read(adc1_handle, SOIL_ADC_CHAN, &val);
    return val;
}
