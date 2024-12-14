/* Copyright 2022 Thomas Baart <thomas@splitkb.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


//https://github.com/qmk/qmk_firmware/blob/master/docs/config_options.md

#pragma once

#ifdef POINTING_DEVICE_ENABLE

// commented out DEBOUNCE as it does not work with Relative Mode

#    define CIRQUE_PINNACLE_DIAMETER_MM 40
#    define POINTING_DEVICE_ROTATION_90

#    define CIRQUE_PINNACLE_ATTENUATION EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_4X //was 2X before


#    define CIRQUE_PINNACLE_POSITION_MODE CIRQUE_PINNACLE_RELATIVE_MODE
//#    define CIRQUE_PINNACLE_TAP_ENABLE  //might give problems with auto-mouse
//#    define CIRQUE_PINNACLE_SECONDARY_TAP_ENABLE  //never managed to make this work anyway
//#    define POINTING_DEVICE_GESTURES_SCROLL_ENABLE  //might give problems with auto-mouse


// ******** taken from Rikiki: ********
#    define DEBOUNCE 8 // added debounce to prevent the trackpad occasionally teleporting
#    define POINTING_DEVICE_CIRQUE_PINNACLE_DPI 4200 // change DPI to what is more comfortable

// cursor glide function, to flick for extended scroll
#    define POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE

/*Enables support for extended reports (-32767 to 32767, instead of -127 to 127),
which may allow for smoother reporting, and prevent maxing out of the reports.
Applies to both Pointing Device and Mousekeys.*/
#    define MOUSE_EXTENDED_REPORT  // Enables support for extended i16 mouse reports (instead of i8).
#    define POINTING_DEVICE_TASK_THROTTLE_MS 10 // Limits the frequency that the sensor is polled for motion.

// MOUSE AUTO-LAYER
// https://docs.qmk.fm/features/pointing_device#how-to-customize

#    define POINTING_DEVICE_AUTO_MOUSE_ENABLE
// only required if not setting mouse layer elsewhere
#    define AUTO_MOUSE_DEFAULT_LAYER 5
#    define AUTO_MOUSE_TIME 1000 // default would be 650ms
#    define AUTO_MOUSE_DELAY 100  // default would be 100ms
#    define AUTO_MOUSE_DEBOUNCE 25  // default would be 25ms
#    define AUTO_MOUSE_THRESHOLD 0  // default is 10
// ******** taken from Rikiki end ********

#endif

#ifdef TAP_DANCE_ENABLE
#    define TAPPING_TERM 200
#    define TAPPING_TERM_PER_KEY
#    define TAPPING_TOGGLE 2
#endif

#define ENCODER_RESOLUTION 6

#ifdef RGBLIGHT_ENABLE
#    define RGBLIGHT_EFFECT_BREATHING
#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#    define RGBLIGHT_EFFECT_SNAKE
#    define RGBLIGHT_EFFECT_KNIGHT
#    define RGBLIGHT_EFFECT_CHRISTMAS
#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
#    define RGBLIGHT_EFFECT_RGB_TEST
#    define RGBLIGHT_EFFECT_ALTERNATING
#    define RGBLIGHT_EFFECT_TWINKLE
#    define RGBLIGHT_HUE_STEP 8
#    define RGBLIGHT_SAT_STEP 8
#    define RGBLIGHT_VAL_STEP 8
#    ifndef RGBLIGHT_LIMIT_VAL
#    	define RGBLIGHT_LIMIT_VAL 150
#    endif
#endif

#ifdef LEADER_ENABLE
#   define LEADER_TIMEOUT 800   
#endif
