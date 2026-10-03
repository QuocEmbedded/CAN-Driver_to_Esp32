#include <Arduino.h>
#include "driver/twai.h"

#define CAN_TX_PIN GPIO_NUM_5
#define CAN_RX_PIN GPIO_NUM_4

void setup() {
  Serial.begin(115200);
  while (!Serial);

  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
  twai_timing_config_t  t_config = TWAI_TIMING_CONFIG_250KBITS();
  twai_filter_config_t  f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK && twai_start() == ESP_OK) {
    Serial.println("[ESP32] TWAI running @ 250 kbps. Waiting for STM32...");
  } else {
    Serial.println("[ESP32] Init Failed!");
  }
}

void loop() {
  twai_message_t rx_msg;

  // Lắng nghe dữ liệu gửi từ STM32
  if (twai_receive(&rx_msg, pdMS_TO_TICKS(50)) == ESP_OK) {
    Serial.printf("[ESP32 RX] ID: 0x%03X | DLC: %d | Data: ", rx_msg.identifier, rx_msg.data_length_code);
    for (int i = 0; i < rx_msg.data_length_code; i++) {
      Serial.printf("0x%02X ", rx_msg.data[i]);
    }
    Serial.println();

    // Bắn lại một gói phản hồi sang STM32 mang ID 0x321
    twai_message_t tx_msg;
    tx_msg.identifier = 0x321;
    tx_msg.extd = 0;          // Standard 11-bit ID
    tx_msg.rtr = 0;
    tx_msg.data_length_code = 4;
    tx_msg.data[0] = 0xDE;
    tx_msg.data[1] = 0xAD;
    tx_msg.data[2] = 0xBE;
    tx_msg.data[3] = 0xEF;

    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(50)) == ESP_OK) {
      Serial.println("[ESP32 TX] -> Da phan hoi goi ID 0x321 sang STM32");
    }
  }
}