#include "driver/i2c.h"
#include "config.h"
#include "rtc.h"

#define I2C_PORT I2C_NUM_0
#define DS3231_ADDR 0x68

static uint8_t bcd2dec(uint8_t val) { return ((val / 16 * 10) + (val % 16)); }

void ds3231_init(void) {  
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA, .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL, .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
}

void rtc_get_time(int *hour, int *minute) {
    uint8_t data[3]; 
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (DS3231_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true); 
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (DS3231_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 3, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    *minute = bcd2dec(data[1]);           
    *hour = bcd2dec(data[2] & 0x3F);      
}
