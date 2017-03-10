/**
 * Arduheater - Telescope heat controller
 * Copyright (C) 2016-2017 João Brázio [joao@brazio.org]
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define BAUDRATE 57600

/**
 * User interface options
 */
#define UI_LCD_TYPE U8GLIB_SSD1306_128X64
#define UI_LCD_OPTIONS U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST
#define UI_LCD_FONT u8g_font_profont11r
#define UI_LCD_FONT_BIG u8g_font_profont22r
#define UI_LCD_ROTATED true
//#define UI_LCD_CONTRAST 255

//#define UI_ENCODER_ENABLE
#define UI_KEYPAD_ENABLE
#define UI_KEYPAD_A_PIN 12
#define UI_KEYPAD_B_PIN 8

// ----------------------------------------------------------------------------

//#define NOAA_DEW_FUNCTION

#define AMBIENT_SENSOR_DHT22
#define AMBIENT_PIN 7

#define HEATER_A_PIN 5

#define THERMISTORNOMINAL 10000.0
#define TEMPERATURENOMINAL 25.0
#define BCOEFFICIENT 3950.0
#define SERIESRESISTOR 10000.0
#define THERMISTOR_MIN_TEMP -20
#define THERMISTOR_MAX_TEMP 50
#define THERMISTOR_ERROR -273

#endif
