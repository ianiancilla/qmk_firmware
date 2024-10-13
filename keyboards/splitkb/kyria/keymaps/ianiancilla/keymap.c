/* Copyright 2019 Thomas Baart <thomas@splitkb.com>
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
#include QMK_KEYBOARD_H
#include "keymap_german.h"


// LAYERS
enum layers {
    _COLEMAK = 0,
    _SPECIAL,
    _NUM,
    _NAV,
};


// Aliases for readability
#define COLEMAK  DF(_COLEMAK)
#define SPEC     DF(_SPECIAL)
#define NUM      DF(_NUM)
#define NAV      MO(_NAV)

//These are standard on Kyria default
//#define CTL_ESC  MT(MOD_LCTL, KC_ESC)
//#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
//#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
//#define ALT_ENT  MT(MOD_LALT, KC_ENT)



// TAP DANCES
// Define a type for as many tap dance states as you need
typedef enum { TD_NONE, TD_UNKNOWN, TD_SINGLE_TAP, TD_SINGLE_HOLD, TD_DOUBLE_TAP } td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// all my tapdances
enum {
    NAV_LAYR, // hold for MO(NAV), double tap to TO(NAV)
    HA_SLA,   // tap for /, hold for #
    CAPS_CW,  // tap for CAPS WORD, double tap for CAPS
    LOCK, // tap for <, hold for <3, double tap for lock
    EMOJI, // tap for =), hold for >.<, double tap for ò.ó
};

// Function associated with all tap dances
td_state_t cur_dance(tap_dance_state_t *state);

//Remove if it compiles without this
////// Functions associated with individual tap dances
////void ql_finished(tap_dance_state_t *state, void *user_data);
////void ql_reset(tap_dance_state_t *state, void *user_data);



// ************* LAYERS *************

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: COLEMAK
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  ESC   |   Q  |   W  |   F  |   P  |   G  |                              |   J  |   L  |   U  |   Y  |Cap/WC|  < > <3|
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  TAB   |   A  |   R  |   S  |   T  |   D  |                              |   H  |   N  |   E  |   I  |   O  |   ^ °  |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |  LGUI  |   Z  |   X  |   C  |   V  |   B  | RAlt |NAV/Mo|  |TO NUM|  DEL |   M  |  , ; | , ;  | . :  | - _  | MO NUM |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | LAlt | LCtrl| Enter| / #  | OSL  |  | RShft|Bckspc| Space| RCtrl|  =)  |
 *                        |      |      | LShft|      | SPEC |  |      |      |      |      | >.<  |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_COLEMAK] = LAYOUT(
     KC_ESC  , KC_Q ,  KC_W   ,  KC_F  ,   KC_P ,   KC_G ,                                        KC_J,   KC_L ,  KC_U ,   KC_Y ,TD(CAPS_CW),TD(LOCK),
     KC_TAB  , KC_A ,  KC_R   ,  KC_S  ,   KC_T ,   KC_D ,                                        KC_H,   KC_N ,  KC_E ,   KC_I ,KC_O   ,DE_CIRC ,
     KC_LGUI , KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B , KC_ALGR,TD(NAV_LAYR), TO(NUM), KC_DEL, KC_N,   KC_M ,DE_COMM, DE_DOT ,DE_MINS, MO(NUM),
                                 KC_LALT, KC_LCTL,SC_SENT,TD(HA_SLA),OSL(SPEC),  KC_RSFT,KC_BSPC,KC_SPC,KC_RCTL, TD(EMOJI)
    ),

  /*
  * SPECIAL
  *
  * ,-------------------------------------------.                              ,-------------------------------------------.
  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        `----------------------------------'  `----------------------------------'
  */
     [_SPECIAL] = LAYOUT(
       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
     ),

/*
 * Nav Layer: Media, navigation
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              | PgUp | Home |   ↑  | End  | VolUp| Delete |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  GUI |  Alt | Ctrl | Shift|      |                              | PgDn |  ←   |   ↓  |   →  | VolDn| Insert |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |ScLck |  |      |      | Pause|M Prev|M Play|M Next|VolMut| PrtSc  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_NAV] = LAYOUT(
      _______, _______, _______, _______, _______, _______,                                     KC_PGUP, KC_HOME, KC_UP,   KC_END,  KC_VOLU, KC_DEL,
      _______, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, _______,                                     KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_VOLD, KC_INS,
      _______, _______, _______, _______, _______, _______, _______, KC_SCRL, _______, _______,KC_PAUSE, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_PSCR,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

     /*
  * Layer template
  *
  * ,-------------------------------------------.                              ,-------------------------------------------.
  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        `----------------------------------'  `----------------------------------'
  */
     [_NUM] = LAYOUT(
       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
     ),



// /*
//  * Layer template
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),
};

/* The default OLED and rotary encoder code can be found at the bottom of qmk_firmware/keyboards/splitkb/kyria/rev1/rev1.c
 * These default settings can be overriden by your own settings in your keymap.c
 * For your convenience, here's a copy of those settings so that you can uncomment them if you wish to apply your own modifications.
 * DO NOT edit the rev1.c file; instead override the weakly defined default functions by your own.
 */


//#ifdef OLED_ENABLE
//oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_180; }
//
//bool oled_task_user(void) {
//    if (is_keyboard_master()) {
//        // QMK Logo and version information
//        // clang-format off
//        static const char PROGMEM qmk_logo[] = {
//            0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
//            0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
//            0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0};
//        // clang-format on
//
//        oled_write_P(qmk_logo, false);
//        oled_write_P(PSTR("Kyria rev1.0\n\n"), false);
//
//        // Host Keyboard Layer Status
//        oled_write_P(PSTR("Layer: "), false);
//        switch (get_highest_layer(layer_state|default_layer_state)) {
//            case _QWERTY:
//                oled_write_P(PSTR("QWERTY\n"), false);
//                break;
//            case _DVORAK:
//                oled_write_P(PSTR("Dvorak\n"), false);
//                break;
//            case _COLEMAK_DH:
//                oled_write_P(PSTR("Colemak-DH\n"), false);
//                break;
//            case _NAV:
//                oled_write_P(PSTR("Nav\n"), false);
//                break;
//            case _SYM:
//                oled_write_P(PSTR("Sym\n"), false);
//                break;
//            case _FUNCTION:
//                oled_write_P(PSTR("Function\n"), false);
//                break;
//            case _ADJUST:
//                oled_write_P(PSTR("Adjust\n"), false);
//                break;
//            default:
//                oled_write_P(PSTR("Undefined\n"), false);
//        }
//
//        // Write host Keyboard LED Status to OLEDs
//        led_t led_usb_state = host_keyboard_led_state();
//        oled_write_P(led_usb_state.num_lock    ? PSTR("NUMLCK ") : PSTR("       "), false);
//        oled_write_P(led_usb_state.caps_lock   ? PSTR("CAPLCK ") : PSTR("       "), false);
//        oled_write_P(led_usb_state.scroll_lock ? PSTR("SCRLCK ") : PSTR("       "), false);
//    } else {
//        // clang-format off
//        static const char PROGMEM kyria_logo[] = {
//            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,192,224,240,112,120, 56, 60, 28, 30, 14, 14, 14,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 14, 14, 14, 30, 28, 60, 56,120,112,240,224,192,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//            0,  0,  0,  0,  0,  0,  0,192,224,240,124, 62, 31, 15,  7,  3,  1,128,192,224,240,120, 56, 60, 28, 30, 14, 14,  7,  7,135,231,127, 31,255,255, 31,127,231,135,  7,  7, 14, 14, 30, 28, 60, 56,120,240,224,192,128,  1,  3,  7, 15, 31, 62,124,240,224,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//            0,  0,  0,  0,240,252,255, 31,  7,  1,  0,  0,192,240,252,254,255,247,243,177,176, 48, 48, 48, 48, 48, 48, 48,120,254,135,  1,  0,  0,255,255,  0,  0,  1,135,254,120, 48, 48, 48, 48, 48, 48, 48,176,177,243,247,255,254,252,240,192,  0,  0,  1,  7, 31,255,252,240,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//            0,  0,  0,255,255,255,  0,  0,  0,  0,  0,254,255,255,  1,  1,  7, 30,120,225,129,131,131,134,134,140,140,152,152,177,183,254,248,224,255,255,224,248,254,183,177,152,152,140,140,134,134,131,131,129,225,120, 30,  7,  1,  1,255,255,254,  0,  0,  0,  0,  0,255,255,255,  0,  0,  0,  0,255,255,  0,  0,192,192, 48, 48,  0,  0,240,240,  0,  0,  0,  0,  0,  0,240,240,  0,  0,240,240,192,192, 48, 48, 48, 48,192,192,  0,  0, 48, 48,243,243,  0,  0,  0,  0,  0,  0, 48, 48, 48, 48, 48, 48,192,192,  0,  0,  0,  0,  0,
//            0,  0,  0,255,255,255,  0,  0,  0,  0,  0,127,255,255,128,128,224,120, 30,135,129,193,193, 97, 97, 49, 49, 25, 25,141,237,127, 31,  7,255,255,  7, 31,127,237,141, 25, 25, 49, 49, 97, 97,193,193,129,135, 30,120,224,128,128,255,255,127,  0,  0,  0,  0,  0,255,255,255,  0,  0,  0,  0, 63, 63,  3,  3, 12, 12, 48, 48,  0,  0,  0,  0, 51, 51, 51, 51, 51, 51, 15, 15,  0,  0, 63, 63,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 48, 48, 63, 63, 48, 48,  0,  0, 12, 12, 51, 51, 51, 51, 51, 51, 63, 63,  0,  0,  0,  0,  0,
//            0,  0,  0,  0, 15, 63,255,248,224,128,  0,  0,  3, 15, 63,127,255,239,207,141, 13, 12, 12, 12, 12, 12, 12, 12, 30,127,225,128,  0,  0,255,255,  0,  0,128,225,127, 30, 12, 12, 12, 12, 12, 12, 12, 13,141,207,239,255,127, 63, 15,  3,  0,  0,128,224,248,255, 63, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//            0,  0,  0,  0,  0,  0,  0,  3,  7, 15, 62,124,248,240,224,192,128,  1,  3,  7, 15, 30, 28, 60, 56,120,112,112,224,224,225,231,254,248,255,255,248,254,231,225,224,224,112,112,120, 56, 60, 28, 30, 15,  7,  3,  1,128,192,224,240,248,124, 62, 15,  7,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//            0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  3,  7, 15, 14, 30, 28, 60, 56,120,112,112,112,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,112,112,112,120, 56, 60, 28, 30, 14, 15,  7,  3,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
//        };
//        // clang-format on
//        oled_write_raw_P(kyria_logo, sizeof(kyria_logo));
//    }
//    return false;
//}
//#endif

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {

    if (index == 0) {
        // Volume control
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    } else if (index == 1) {
        // Page up/Page down
        if (clockwise) {
            tap_code(KC_PGDN);
        } else {
            tap_code(KC_PGUP);
        }
    }
    return false;
}
#endif

// TAP DANCE
// Determine the current tap dance state
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) return TD_DOUBLE_TAP;
    else return TD_UNKNOWN;
}

// Initialize tap structure associated with example tap dance key
static td_tap_t ql_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

// NAV_LAYR tap dance
void navlayr_finished(tap_dance_state_t *state, void *user_data) {
    ql_tap_state.state = cur_dance(state);
    switch (ql_tap_state.state) {
        case TD_SINGLE_TAP:
            // do nothing
            break;
        case TD_SINGLE_HOLD:
            layer_on(_NAV);
            break;
        case TD_DOUBLE_TAP:
            // Check to see if the layer is already set
            if (layer_state_is(_NAV)) {
                // If already set, then switch it off
                layer_off(_NAV);
            } else {
                // If not already set, then switch the layer on
                layer_on(_NAV);
            }
            break;
        default:
            break;
    }
}

void navlayr_reset(tap_dance_state_t *state, void *user_data) {
    // If the key was held down and now is released then switch off the layer
    if (ql_tap_state.state == TD_SINGLE_HOLD) {
        layer_off(_NAV);
    }
    ql_tap_state.state = TD_NONE;
}

// SLASH_HASH tap dance
void slash_hash_finished(tap_dance_state_t *state, void *user_data) {
    ql_tap_state.state = cur_dance(state);
    switch (ql_tap_state.state) {
        case TD_SINGLE_TAP:
            tap_code(S(KC_7));
            break;
        case TD_SINGLE_HOLD:
            tap_code(KC_NUHS);
            break;
        case TD_DOUBLE_TAP:
            tap_code(S(KC_7));
            tap_code(S(KC_7));
            break;
        default:
            break;
    }
}

void slash_hash_reset(tap_dance_state_t *state, void *user_data) {
    ql_tap_state.state = TD_NONE;
}

// LOCK tap dance
void lock_finished(tap_dance_state_t *state, void *user_data) {
    ql_tap_state.state = cur_dance(state);
    switch (ql_tap_state.state) {
        //case TD_SINGLE_TAP:
        //    tap_code(DE_LABK);
        //    break;
        //case TD_SINGLE_HOLD:
        //    tap_code(DE_LABK);
        //    tap_code(KC_3);
        //    break;
        //case TD_DOUBLE_TAP:
        //    tap_code(LGUI(KC_L));
        //    break;
        default:
            break;
    }
}

void lock_reset(tap_dance_state_t *state, void *user_data) {
    ql_tap_state.state = TD_NONE;
}

// EMOJI tap dance
void emoji_finished(tap_dance_state_t *state, void *user_data) {
    ql_tap_state.state = cur_dance(state);
    switch (ql_tap_state.state) {
        //case TD_SINGLE_TAP:
        //    tap_code(DE_EQL);
        //    tap_code(DE_RPRN);
        //    break;
        //case TD_SINGLE_HOLD:
        //    tap_code(DE_RABK);
        //    tap_code(DE_DOT);
        //    tap_code(DE_LABK);
        //    break;
        //case TD_DOUBLE_TAP:
        //    tap_code(DE_GRV);
        //    tap_code(KC_O);
        //    tap_code(DE_DOT);
        //    tap_code(DE_ACUT);
        //    tap_code(KC_O);
        //    break;
        default:
            break;
    }
}

void emoji_reset(tap_dance_state_t *state, void *user_data) {
    // If the key was held down and now is released then switch off the layer
    ql_tap_state.state = TD_NONE;
}


// Associate our tap dance key with its functionality
tap_dance_action_t tap_dance_actions[] = {
    [NAV_LAYR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, navlayr_finished, navlayr_reset),
    [HA_SLA] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, slash_hash_finished, slash_hash_reset),
    [CAPS_CW] = ACTION_TAP_DANCE_DOUBLE(CW_TOGG, KC_CAPS),
    [LOCK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, lock_finished, lock_reset),
    [EMOJI] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, emoji_finished, emoji_reset),
};

// Set a long-ish tapping term for tap-dance keys
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            return 275;
        default:
            return TAPPING_TERM;
    }
}
