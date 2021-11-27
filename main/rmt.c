#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"

static const char *RMT_TX_TAG = "RMT Tx";

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define RMT_TX_GPIO 18
#define SONY_CARRIER_FREQ_HZ 40000
#define SONY_BITS_NUM 12


rmt_item32_t sony_start = {{{ 1280, 1, 320, 0 }}}; //clk_div of 150
rmt_item32_t sony_zero = {{{ 320, 1, 320, 0 }}};
rmt_item32_t sony_one = {{{ 640, 1, 320, 0 }}};



/*
 * Initialize the RMT Tx channel
 */
void rmt_tx_init()
{
    rmt_config_t config;
    config.rmt_mode = RMT_MODE_TX;
    config.channel = RMT_TX_CHANNEL;
    config.gpio_num = RMT_TX_GPIO;
    config.mem_block_num = 1;
    config.tx_config.loop_en = 0;
    // enable the carrier to be able to hear the Morse sound
    // if the RMT_TX_GPIO is connected to a speaker
    config.tx_config.carrier_en = 1;
    config.tx_config.idle_output_en = 1;
    config.tx_config.idle_level = 0;
    config.tx_config.carrier_duty_percent = 50;
    // set audible career frequency of 611 Hz
    // actually 611 Hz is the minimum, that can be set
    // with current implementation of the RMT API
    config.tx_config.carrier_freq_hz = SONY_CARRIER_FREQ_HZ;
    config.tx_config.carrier_level = 1;
    // set the maximum clock divider to be able to output
    // RMT pulses in range of about one hundred milliseconds
    config.clk_div = 150; //clock is 1066666.66 Hz, 640 click will be 600uS

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
    //ESP_ERROR_CHECK(rmt_translator_init(config.channel, u8_to_rmt));
}


void rmt_sony_power()
{
	rmt_item32_t rmt_buffer[SONY_BITS_NUM + 1]; // the 1 is the start bit
	rmt_buffer[0] = sony_start;
	// comand is 7 bits, power command is 21 - 0010101
	rmt_buffer[1] = sony_one; // lsb
	rmt_buffer[2] = sony_zero;
	rmt_buffer[3] = sony_one;
	rmt_buffer[4] = sony_zero;
	rmt_buffer[5] = sony_one;
	rmt_buffer[6] = sony_zero;
	rmt_buffer[7] = sony_zero; // msb
	// address is 5 bits, television is 1
	rmt_buffer[8] = sony_one; // lsb
	rmt_buffer[9] = sony_zero;
	rmt_buffer[10] = sony_zero;
	rmt_buffer[11] = sony_zero;
	rmt_buffer[12] = sony_zero;

	rmt_write_items(RMT_TX_CHANNEL, rmt_buffer, SONY_BITS_NUM + 1, true);
	ESP_LOGI(RMT_TX_TAG, "Transmission complete");
}


void rmt_sony_v_up()
{
    rmt_item32_t rmt_buffer[SONY_BITS_NUM + 1]; // the 1 is the start bit
    rmt_buffer[0] = sony_start;
    // comand is 7 bits, v_up command is 18 - 0010010
    rmt_buffer[1] = sony_zero; // lsb
    rmt_buffer[2] = sony_one;
    rmt_buffer[3] = sony_zero;
    rmt_buffer[4] = sony_zero;
    rmt_buffer[5] = sony_one;
    rmt_buffer[6] = sony_zero;
    rmt_buffer[7] = sony_zero; // msb
    // address is 5 bits, television is 1
    rmt_buffer[8] = sony_one; // lsb
    rmt_buffer[9] = sony_zero;
    rmt_buffer[10] = sony_zero;
    rmt_buffer[11] = sony_zero;
    rmt_buffer[12] = sony_zero;

    rmt_write_items(RMT_TX_CHANNEL, rmt_buffer, SONY_BITS_NUM + 1, true);
    ESP_LOGI(RMT_TX_TAG, "Transmission complete");
}

void rmt_sony_v_down()
{
    rmt_item32_t rmt_buffer[SONY_BITS_NUM + 1]; // the 1 is the start bit
    rmt_buffer[0] = sony_start;
    // comand is 7 bits, v_down command is 19 - 0010011
    rmt_buffer[1] = sony_one; // lsb
    rmt_buffer[2] = sony_one;
    rmt_buffer[3] = sony_zero;
    rmt_buffer[4] = sony_zero;
    rmt_buffer[5] = sony_one;
    rmt_buffer[6] = sony_zero;
    rmt_buffer[7] = sony_zero; // msb
    // address is 5 bits, television is 1
    rmt_buffer[8] = sony_one; // lsb
    rmt_buffer[9] = sony_zero;
    rmt_buffer[10] = sony_zero;
    rmt_buffer[11] = sony_zero;
    rmt_buffer[12] = sony_zero;

    rmt_write_items(RMT_TX_CHANNEL, rmt_buffer, SONY_BITS_NUM + 1, true);
    ESP_LOGI(RMT_TX_TAG, "Transmission complete");
}


void rmt_sony_input()
{
    rmt_item32_t rmt_buffer[SONY_BITS_NUM + 1]; // the 1 is the start bit
    rmt_buffer[0] = sony_start;
    // comand is 7 bits, v_up command is 37 - 0100101
    rmt_buffer[1] = sony_one; // lsb
    rmt_buffer[2] = sony_zero;
    rmt_buffer[3] = sony_one;
    rmt_buffer[4] = sony_zero;
    rmt_buffer[5] = sony_zero;
    rmt_buffer[6] = sony_one;
    rmt_buffer[7] = sony_zero; // msb
    // address is 5 bits, television is 1
    rmt_buffer[8] = sony_one; // lsb
    rmt_buffer[9] = sony_zero;
    rmt_buffer[10] = sony_zero;
    rmt_buffer[11] = sony_zero;
    rmt_buffer[12] = sony_zero;

    rmt_write_items(RMT_TX_CHANNEL, rmt_buffer, SONY_BITS_NUM + 1, true);
    ESP_LOGI(RMT_TX_TAG, "Transmission complete");
}


void rmt_sony_up()
{
    rmt_item32_t rmt_buffer[SONY_BITS_NUM + 1]; // the 1 is the start bit
    rmt_buffer[0] = sony_start;
    // comand is 7 bits, up command is 116 - 1110100
    rmt_buffer[1] = sony_zero; // lsb
    rmt_buffer[2] = sony_zero;
    rmt_buffer[3] = sony_one;
    rmt_buffer[4] = sony_zero;
    rmt_buffer[5] = sony_one;
    rmt_buffer[6] = sony_one;
    rmt_buffer[7] = sony_one; // msb
    // address is 5 bits, television is 1
    rmt_buffer[8] = sony_one; // lsb
    rmt_buffer[9] = sony_zero;
    rmt_buffer[10] = sony_zero;
    rmt_buffer[11] = sony_zero;
    rmt_buffer[12] = sony_zero;

    rmt_write_items(RMT_TX_CHANNEL, rmt_buffer, SONY_BITS_NUM + 1, true);
    ESP_LOGI(RMT_TX_TAG, "Transmission complete");
}


void rmt_sony_down()
{
    rmt_item32_t rmt_buffer[SONY_BITS_NUM + 1]; // the 1 is the start bit
    rmt_buffer[0] = sony_start;
    // comand is 7 bits, down command is 117 - 1110101
    rmt_buffer[1] = sony_one; // lsb
    rmt_buffer[2] = sony_zero;
    rmt_buffer[3] = sony_one;
    rmt_buffer[4] = sony_zero;
    rmt_buffer[5] = sony_one;
    rmt_buffer[6] = sony_one;
    rmt_buffer[7] = sony_one; // msb
    // address is 5 bits, television is 1
    rmt_buffer[8] = sony_one; // lsb
    rmt_buffer[9] = sony_zero;
    rmt_buffer[10] = sony_zero;
    rmt_buffer[11] = sony_zero;
    rmt_buffer[12] = sony_zero;

    rmt_write_items(RMT_TX_CHANNEL, rmt_buffer, SONY_BITS_NUM + 1, true);
    ESP_LOGI(RMT_TX_TAG, "Transmission complete");
}


void rmt_sony_ok()
{
    rmt_item32_t rmt_buffer[SONY_BITS_NUM + 1]; // the 1 is the start bit
    rmt_buffer[0] = sony_start;
    // comand is 7 bits, ok command is 101 - 1100101
    rmt_buffer[1] = sony_one; // lsb
    rmt_buffer[2] = sony_zero;
    rmt_buffer[3] = sony_one;
    rmt_buffer[4] = sony_zero;
    rmt_buffer[5] = sony_zero;
    rmt_buffer[6] = sony_one;
    rmt_buffer[7] = sony_one; // msb
    // address is 5 bits, television is 1
    rmt_buffer[8] = sony_one; // lsb
    rmt_buffer[9] = sony_zero;
    rmt_buffer[10] = sony_zero;
    rmt_buffer[11] = sony_zero;
    rmt_buffer[12] = sony_zero;

    rmt_write_items(RMT_TX_CHANNEL, rmt_buffer, SONY_BITS_NUM + 1, true);
    ESP_LOGI(RMT_TX_TAG, "Transmission complete");
}











