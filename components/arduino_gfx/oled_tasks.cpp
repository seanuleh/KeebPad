/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * Copyright 2018 Gal Zaidenstein.
 */

#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include "oled_tasks.h"

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"

#include "keyboard_config.h"
#include "battery_monitor.h"
#include "nvs_keymaps.h"

#include "Arduino_ESP32SPI.h"
#include "Arduino_GFX.h"     // Core graphics library
#include "Arduino_ST7789.h"

#include "icons.h"

static const char *TAG = "	OLED";

Arduino_DataBus *bus;
Arduino_ST7789 *tft;

uint8_t prev_led = 0;

QueueHandle_t layer_recieve_q;
QueueHandle_t led_recieve_q;

uint32_t battery_percent = 0;
uint32_t prev_battery_percent = 0;

uint8_t curr_layout = 0;
uint8_t current_led = 0;

int BATT_FLAG = 0;
int DROP_H = 0;

int offset_x_batt = 0;
int offset_y_batt = 0;

#define BT_ICON 0x5e
#define BATT_ICON 0x5b
#define LOCK_ICON 0xca
#define TFT_BL 4 // Backlight Control Pin

//Erasing area from oled
void erase_area(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
	  tft->fillRect(x, y, w, h, RED);
}

//Function for updating the OLED
void update_oled(void) {
#ifdef BATT_STAT
	battery_percent = get_battery_level();
#endif

	if (xQueueReceive(layer_recieve_q, &curr_layout, (TickType_t) 0)) {
		erase_area(0, 0, 45, 45);
		tft->setCursor(0, 14);
		tft->println(layer_names_arr[curr_layout]);
		ESP_LOGI("SEAN", "Received Task");
	}
	// // if (xQueueReceive(led_recieve_q, &current_led, (TickType_t) 0)) {
	// 	// erase_area(0, 24, 127, 8);
	// 	// if (CHECK_BIT(current_led,0) != 0) {
	// // 		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	// // 		u8g2_DrawStr(&u8g2, 0, 31, "NUM");
	// // 		u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t);
	// // 		u8g2_DrawGlyph(&u8g2, 16, 32, LOCK_ICON);
	// 		// tft->setCursor(0, 14);
	// 		// tft->println(layer_names_arr[curr_layout]);
	// 	// }

	// // 	if (CHECK_BIT(current_led,1) != 0) {
	// // 		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	// // 		u8g2_DrawStr(&u8g2, 27, 31, "CAPS");
	// // 		u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t);
	// // 		u8g2_DrawGlyph(&u8g2, 48, 32, LOCK_ICON);
	// // 	}
	// // 	if (CHECK_BIT(current_led,2) != 0) {
	// // 		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	// // 		u8g2_DrawStr(&u8g2, 57, 31, "SCROLL");
	// // 		u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t);
	// // 		u8g2_DrawGlyph(&u8g2, 88, 32, LOCK_ICON);
	// // 	}
	// // 	u8g2_SendBuffer(&u8g2);
	// // }

	// if (battery_percent != prev_battery_percent) {
	// 	char buf[sizeof(uint32_t)];
	// 	snprintf(buf, sizeof(uint32_t), "%d", battery_percent);
	// 	// u8g2_DrawStr(&u8g2, 103 + offset_x_batt , 7 + offset_y_batt, "%");
	// 	tft->setCursor(103 + offset_x_batt , 7 + offset_y_batt);
	// 	tft->println(buf);
	// 	// if ((battery_percent < 100)
	// 	// 		&& (abs(battery_percent - prev_battery_percent) >= 2)) {
	// 	// 	erase_area(85 + offset_x_batt , 0 + offset_y_batt, 15, 7);
	// 	// 	u8g2_DrawStr(&u8g2, 90 + offset_x_batt , 7 + offset_y_batt, buf);
	// 	// 	u8g2_SendBuffer(&u8g2);
	// 	// }
	// 	// if ((battery_percent > 100) && (BATT_FLAG = 0)) {
	// 	// 	erase_area(85  + offset_x_batt, 0 + offset_y_batt, 15, 7);
	// 	// 	u8g2_DrawStr(&u8g2, 85 + offset_x_batt, 7 + offset_y_batt, "100");
	// 	// 	BATT_FLAG = 1;
	// 	// 	u8g2_SendBuffer(&u8g2);
	// 	// }
	// 	// if (battery_percent == 100) {
	// 	// 	erase_area(85 + offset_x_batt , 0 + offset_y_batt, 15, 7);
	// 	// 	u8g2_DrawStr(&u8g2, 85 + offset_x_batt, 7 + offset_y_batt , "100");
	// 	// 	u8g2_SendBuffer(&u8g2);
	// 	// }
	// 	// prev_battery_percent = battery_percent;

	// }
}

//oled on connection
void ble_connected_oled(void) {

// 	u8g2_ClearBuffer(&u8g2);
// 	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
// 	u8g2_DrawStr(&u8g2, 0, 6, GATTS_TAG);
// 	u8g2_DrawStr(&u8g2, 0, 14, layer_names_arr[current_layout]);
// 	u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t);
// 	u8g2_DrawGlyph(&u8g2, 110 + offset_x_batt , 8 + offset_y_batt, BATT_ICON);
// 	u8g2_DrawGlyph(&u8g2, 120 + offset_x_batt, 8 + offset_y_batt , BT_ICON);

// //	if(CHECK_BIT(curr_led,0)!=0){
// //		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
// //		u8g2_DrawStr(&u8g2, 0,31,"NUM");
// //		u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
// //		u8g2_DrawGlyph(&u8g2, 16,32,LOCK_ICON);
// //	}
// //
// //	if(CHECK_BIT(curr_led,1)!=0){
// //		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
// //		u8g2_DrawStr(&u8g2, 27,31,"CAPS");
// //		u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
// //		u8g2_DrawGlyph(&u8g2,48,32,LOCK_ICON);
// //	}
// //	if(CHECK_BIT(curr_led,2)!=0){
// //		u8g2_SetFont(&u8g2, u8g2_font_5x7_tf );
// //		u8g2_DrawStr(&u8g2, 57,31,"SCROLL");
// //		u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t );
// //		u8g2_DrawGlyph(&u8g2,88,32,LOCK_ICON);
// //	}

// 	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
// 	char buf[sizeof(uint32_t)];
// 	snprintf(buf, sizeof(uint32_t), "%d", battery_percent);
// 	u8g2_DrawStr(&u8g2, + offset_x_batt, + offset_y_batt, "%");
// 	if (battery_percent < 100) {
// 		u8g2_DrawStr(&u8g2, + offset_x_batt, 7 + offset_y_batt , buf);
// 	} else {
// 		u8g2_DrawStr(&u8g2, 85 + offset_x_batt, 7 + offset_y_batt, "100");
// 	}
// 	u8g2_SendBuffer(&u8g2);
}

//Slave oled display
void ble_slave_oled(void) {
	battery_percent = get_battery_level();

	// if (battery_percent != prev_battery_percent) {
	// 	u8g2_ClearBuffer(&u8g2);
	// 	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	// 	u8g2_DrawStr(&u8g2, 0, 6, GATTS_TAG);
	// 	u8g2_DrawStr(&u8g2, 0, 14, "Slave pad 1");
	// 	u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t);
	// 	u8g2_DrawGlyph(&u8g2, 110 + offset_x_batt, 8 + offset_y_batt, BATT_ICON);
	// 	u8g2_DrawGlyph(&u8g2, 120 + offset_x_batt, 8 + offset_y_batt, BT_ICON);

	// 	u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	// 	char buf[sizeof(uint32_t)];
	// 	snprintf(buf, sizeof(uint32_t), "%d", battery_percent);
	// 	u8g2_DrawStr(&u8g2, 103 + offset_x_batt, 7 + offset_y_batt, "%");
	// 	if ((battery_percent < 100)
	// 			&& (battery_percent - prev_battery_percent >= 2)) {
	// 		u8g2_SetDrawColor(&u8g2, 0);
	// 		u8g2_DrawBox(&u8g2, 85  + offset_x_batt, 90 + offset_y_batt, 0, 7);
	// 		u8g2_SetDrawColor(&u8g2, 1);
	// 		u8g2_DrawStr(&u8g2, 90 + offset_x_batt, 7 + offset_y_batt, buf);
	// 		u8g2_SendBuffer(&u8g2);
	// 	}
	// 	if ((battery_percent < 100)
	// 			&& (battery_percent - prev_battery_percent >= 2)) {
	// 		erase_area(85 + offset_x_batt, 0 + offset_y_batt, 15, 7);
	// 		u8g2_DrawStr(&u8g2, 90, 7, buf);
	// 		u8g2_SendBuffer(&u8g2);
	// 	}
	// 	if ((battery_percent > 100) && (BATT_FLAG = 0)) {
	// 		erase_area(85 + offset_x_batt, 0 + offset_y_batt, 15, 7);
	// 		u8g2_DrawStr(&u8g2, 85 + offset_x_batt, 7 + offset_y_batt, "100");
	// 		BATT_FLAG = 1;
	// 		u8g2_SendBuffer(&u8g2);
	// 	}
	// 	if (battery_percent == 100) {
	// 		erase_area(85 + offset_x_batt, 0 + offset_y_batt, 15, 7);
	// 		u8g2_DrawStr(&u8g2, 85 + offset_x_batt, 7 + offset_y_batt, "100");
	// 		u8g2_SendBuffer(&u8g2);
	// 	}
	// 	prev_battery_percent = battery_percent;
	// }

}

//Waiting for connecting animation
void waiting_oled(void) {
	char waiting[] = "Waiting for ";
	char conn[] = "connection";

#ifdef BATT_STAT
	battery_percent = get_battery_level();
#endif

	// u8g2_ClearBuffer(&u8g2);
	// u8g2_SetFont(&u8g2, u8g2_font_open_iconic_all_1x_t);
	// u8g2_DrawGlyph(&u8g2, 110 + offset_x_batt, 8 + offset_y_batt, BATT_ICON);
	// u8g2_DrawGlyph(&u8g2, 120 + offset_x_batt, 8 + offset_y_batt, BT_ICON);
	// u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	// u8g2_DrawStr(&u8g2, 0, 6, GATTS_TAG);

	// char buf[sizeof(uint32_t)];
	// snprintf(buf, sizeof(uint32_t), "%d", battery_percent);
	// u8g2_DrawStr(&u8g2, 103 + offset_x_batt, 7 + offset_y_batt, "%");
	// if ((battery_percent < 100)
	// 		&& (battery_percent - prev_battery_percent >= 2)) {
	// 	u8g2_DrawStr(&u8g2, 90 + offset_x_batt, 7 + offset_y_batt, buf);
	// }
	// if (battery_percent < 100) {
	// 	u8g2_DrawStr(&u8g2, 90 + offset_x_batt, 7 + offset_y_batt, buf);
	// } else {
	// 	u8g2_DrawStr(&u8g2, 85 + offset_x_batt, 7 + offset_y_batt, "100");
	// }

	// for (int i = 0; i < 3; i++) {
	// 	u8g2_DrawStr(&u8g2, 0, 26, waiting);
	// 	u8g2_DrawStr(&u8g2, 0, 40, conn);
	// 	u8g2_SendBuffer(&u8g2);
	// 	vTaskDelay(100 / portTICK_PERIOD_MS);
	// 	strcat(conn, ".");
	// }

}

//shut down OLED
void deinit_oled(void) {

	// u8g2_ClearBuffer(&u8g2);
	// u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
	// u8g2_DrawStr(&u8g2, 0, 6, GATTS_TAG);
	// u8g2_DrawStr(&u8g2, 0, 26, "Going to sleep!");
	// u8g2_SendBuffer(&u8g2);
	// vTaskDelay(1000 / portTICK_PERIOD_MS);
	// u8g2_ClearDisplay(&u8g2);
	// u8g2_SetPowerSave(&u8g2, 1);
	// i2c_driver_delete(I2C_NUM_0);
}

//initialize oled
void init_oled() {

	ESP_LOGI("Oled", "init OLED function");

	layer_recieve_q = xQueueCreate(32, sizeof(uint8_t));
	led_recieve_q = xQueueCreate(32, sizeof(uint8_t));

	bus = new Arduino_ESP32SPI(16, 5, 18, 19, -1, VSPI);
	tft = new Arduino_ST7789(bus, -1, 1, true, 135, 240, 53, 40, 52, 40);

    tft->begin();
    tft->fillScreen(BLACK);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    tft->fillRect(0, 0, 240, 140, BLACK);
    tft->setCursor(84, 5);
    tft->setTextSize(2);
    tft->setTextColor(BLUE);
    tft->println("KeebPad");
    
    int x = 24, y = 32;
    while (x < 300) {
      while (y < 300) {
        tft->draw16bitRGBBitmap(x, y, whatsApp, 32, 32);  
        y = y + 32;
      }
      x = x + 32 + 24;
      y = 32;
    }

	ESP_LOGI("OLED", "All done!");

	// vTaskDelete(NULL);
}
