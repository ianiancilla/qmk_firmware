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
#include "sendstring_german.h"


 // LAYERS
enum layers {
    _COLEMAK = 0,
    _SPECIAL,
    _NUM,
    _NAV,
    _MOUSE,
};


// Aliases for readability
#define CLMK     _COLEMAK
#define SPEC     _SPECIAL
#define NUM      _NUM
#define NAV      _NAV
#define MOU      _MOUSE

// ************* TAP DANCE AND MACROS************* 
#ifndef TAP_DANCE_AND_MACRO

// Definitions

// all my macros
enum custom_keycodes {
    M_AGRAVE = SAFE_RANGE, //à
    M_AACUTE,              //á
    M_EGRAVE,              //è
    M_EACUTE,              //é
    M_IGRAVE,              //ì
    M_OGRAVE,              //ò
    M_UGRAVE,              //ù
    M_ETH,                 //ð
    M_NTILDE,              //ñ
    M_CHARAMAP,            // opens character map
    M_TM                   // TM symbol
};

// all my tapdances
enum {
    HA_SLA,    // tap for /, hold for #
    SPEC_A,    // tap for à, hold for ä
    SPEC_U,    // tap for ù, hold for ü
    SPEC_O,    // tap for ò, hold for ö
    CAPS_CW,   // tap for CAPS WORD, double tap for CAPS
    LOCK,      // tap for <, hold for <3, double tap for lock
    EMOJI,     // tap for =), hold for >.<, double tap for ò.ó
    Z_PRINT,   // tap for z, hold for print
    DEL_MOUSE, // tap for delete, hold to move to nav layer
};


// structs and functions needed for different tapdance behaviours
typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#    ifndef PERMISSIVE_HOLD
            && !state->interrupted
#    endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
        { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

#    define ACTION_TAP_DANCE_TAP_HOLD_FN(tap, hold) \
        { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }


// single tap-dance functions behaviours

//// LOCK tap dance
// void lock(tap_dance_state_t *state, void *user_data) {
//    ql_tap_state.state = cur_dance(state);
//    switch (ql_tap_state.state) {
//        case TD_SINGLE_TAP:
//            tap_code16(DE_LABK);
//            break;
//        case TD_SINGLE_HOLD:
//            tap_code16(DE_LABK);
//            tap_code16(KC_3);
//            break;
//        case TD_DOUBLE_TAP:
//            tap_code16(LGUI(KC_L));
//            break;
//        default:
//            break;
//    }
//}
//
//// EMOJI tap dance
// void emoji(tap_dance_state_t *state, void *user_data) {
//    ql_tap_state.state = cur_dance(state);
//    switch (ql_tap_state.state) {
//        case TD_SINGLE_TAP:
//            tap_code16(DE_EQL);
//            tap_code16(DE_RPRN);
//            break;
//        case TD_SINGLE_HOLD:
//            tap_code16(DE_RABK);
//            tap_code16(DE_DOT);
//            tap_code16(DE_LABK);
//            break;
//        case TD_DOUBLE_TAP:
//            tap_code16(DE_GRV);
//            tap_code16(KC_O);
//            tap_code16(DE_DOT);
//            tap_code16(DE_ACUT);
//            tap_code16(KC_O);
//            break;
//        default:
//            break;
//    }
//}
//
//// CAPS_CW tap dance
// void caps_cw(tap_dance_state_t *state, void *user_data) {
//    ql_tap_state.state = cur_dance(state);
//    switch (ql_tap_state.state) {
//        case TD_SINGLE_TAP:
//            caps_word_toggle();
//            break;
//        case TD_DOUBLE_TAP:
//            tap_code(KC_CAPS);
//            break;
//        default:
//            break;
//    }
//}

// NOTE: all tap dances that use tap-hold function also need to be mentioned in process_record_user in the "macros" section, around line 55
// Associate tap dance keys with their functionality
tap_dance_action_t tap_dance_actions[] = {
    [HA_SLA] = ACTION_TAP_DANCE_TAP_HOLD(DE_SLSH, DE_HASH),
    [SPEC_A] = ACTION_TAP_DANCE_TAP_HOLD(M_AGRAVE, DE_ADIA),
    [SPEC_U] = ACTION_TAP_DANCE_TAP_HOLD(M_UGRAVE, DE_UDIA),
    [SPEC_O] = ACTION_TAP_DANCE_TAP_HOLD(M_OGRAVE, DE_ODIA),
    [Z_PRINT] = ACTION_TAP_DANCE_TAP_HOLD(DE_Z, KC_PRINT_SCREEN),

    //[CAPS_CW] = ACTION_TAP_DANCE_FN(caps_cw),
    //[LOCK] = ACTION_TAP_DANCE_FN(lock),
    //[EMOJI] = ACTION_TAP_DANCE_FN(emoji),
};

// associate custom buttons with their behaviour
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    tap_dance_action_t *action;

    switch (keycode) {
        case TD(HA_SLA): // list all tap dance keycodes with tap-hold configurations
        case TD(SPEC_A):
        case TD(SPEC_U):
        case TD(SPEC_O):
        case TD(Z_PRINT):
            action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }
            break;

        case M_AGRAVE: // à
            if (record->event.pressed) {
                tap_code16(DE_GRV);
                tap_code16(KC_A);
            }
            break;

        case M_AACUTE: // á (does not work shifted, will still give À)
            if (record->event.pressed) {
                tap_code16(DE_ACUT);
                tap_code16(KC_A);
            }
            break;

        case M_EGRAVE: // è
            if (record->event.pressed) {
                tap_code16(DE_GRV);
                tap_code16(KC_E);
            }
            break;

        case M_EACUTE: // é
            if (record->event.pressed) {
                tap_code16(DE_ACUT);
                tap_code16(KC_E);
            }
            break;

        case M_IGRAVE: // ì
            if (record->event.pressed) {
                tap_code16(DE_GRV);
                tap_code16(KC_I);
            }
            break;

        case M_OGRAVE: // ò
            if (record->event.pressed) {
                tap_code16(DE_GRV);
                tap_code16(KC_O);
            }
            break;

        case M_UGRAVE: // ù
            if (record->event.pressed) {
                tap_code16(DE_GRV);
                tap_code16(KC_U);
            }
            break;

        case M_ETH: // ð
            if (record->event.pressed) {
                SEND_STRING(SS_DOWN(X_LALT) SS_TAP(X_P0) SS_TAP(X_P2) SS_TAP(X_P4) SS_TAP(X_P0) SS_UP(X_LALT));
            }
            break;

        case M_NTILDE: // ñ
            if (record->event.pressed) {
                SEND_STRING(SS_DOWN(X_LALT) SS_TAP(X_P0) SS_TAP(X_P2) SS_TAP(X_P4) SS_TAP(X_P1) SS_UP(X_LALT));
            }
            break;

        case M_CHARAMAP: // opens character map app
            if (record->event.pressed) {
                //SEND_STRING(SS_TAP(X_LGUI) "character map" SS_TAP(KC_ENT));
            }
            break;

        case M_TM: // ™
            if (record->event.pressed) {
                SEND_STRING(SS_DOWN(X_LALT) SS_TAP(X_P0) SS_TAP(X_P1) SS_TAP(X_P5) SS_TAP(X_P3) SS_UP(X_LALT));
            }
            break;
    }
    return true;
};

#endif


// ************* COMBOS *************
#ifndef COMBOS

enum combos {
    CB_COLEMAK, // return to Colemak layer with TO
    CB_BKSPACE, // types backspace
    CB_APOSTROPHE, // '
    CB_DEL, // types delete
    CB_HOME, // presses HOME key
    CB_END, // presses END key
    CB_ACUTE_E, // types é
    CB_TM, // types ™
    CB_MUTE, // mutes audio
    CB_SPACE, // presses SPACEBAR
};


const uint16_t PROGMEM enter_space_combo[] = {SC_SENT, KC_SPC, COMBO_END};
const uint16_t PROGMEM pt_combo[] = {KC_P, KC_T, COMBO_END};
const uint16_t PROGMEM tv_combo[] = {KC_T, KC_V, COMBO_END};
const uint16_t PROGMEM gd_combo[] = {KC_G, KC_D, COMBO_END};
const uint16_t PROGMEM ln_combo[] = {KC_L, KC_N, COMBO_END};
const uint16_t PROGMEM ominus_combo[] = {KC_O, DE_MINS, COMBO_END};
const uint16_t PROGMEM hn_combo[] = {KC_H, KC_N, COMBO_END};
const uint16_t PROGMEM tm_combo[] = {KC_T, KC_M, COMBO_END};
const uint16_t PROGMEM wfp_combo[] = {KC_W, KC_F, KC_P, COMBO_END};
const uint16_t PROGMEM cv_combo[]  = {KC_C, KC_V, COMBO_END};


combo_t key_combos[] = {
    [CB_COLEMAK] = COMBO(enter_space_combo, TO(_COLEMAK)),
    [CB_BKSPACE] = COMBO(pt_combo, KC_BSPC),
    [CB_APOSTROPHE] = COMBO(tv_combo, DE_QUOT),
    [CB_DEL] = COMBO(gd_combo, KC_DEL),
    [CB_HOME]    = COMBO(ln_combo, KC_HOME),
    [CB_END]     = COMBO(ominus_combo, KC_END),
    [CB_ACUTE_E] = COMBO(hn_combo, M_EACUTE),
    [CB_TM]      = COMBO(tm_combo, M_TM),
    [CB_MUTE] = COMBO(wfp_combo, KC_AUDIO_MUTE),
    [CB_SPACE] = COMBO(cv_combo, KC_SPC)
};
#endif

// ************* LAYERS *************
#ifndef LAYER_MAP

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: COLEMAK
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  ESC   |   Q  |   W  |   F  |   P  |   G  |                              |   J  |   L  |   U  |   Y  |Emoji |  < > <3|
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  TAB   |   A  |   R  |   S  |   T  |   D  |                              |   H  |   N  |   E  |   I  |   O  |   ^ °  |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |  LGUI  |   Z  |   X  |   C  |   V  |   B  | RAlt |TT NAV|  |TTMOUS|LEADER|   K  |   M  | , ;  | . :  | - _  | MO NUM |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | LAlt | LCtrl| Enter| tap /| MO   |  | RShft|Bckspc| Space| RCtrl|CapsW |
 *                        |      |      | LShft|hold #| SPEC |  |      |      |      |      |CapLck|
 *                        `----------------------------------'  `----------------------------------'
 */
    [_COLEMAK] = LAYOUT(
     KC_ESC  , KC_Q ,  KC_W   ,  KC_F  ,   KC_P ,   KC_G ,                                        KC_J,   KC_L ,  KC_U ,   DE_Y ,TD(EMOJI),TD(LOCK),
     KC_TAB  , KC_A ,  KC_R   ,  KC_S  ,   KC_T ,   KC_D ,                                        KC_H,   KC_N ,  KC_E ,   KC_I ,KC_O   ,DE_CIRC ,
     KC_LGUI,TD(Z_PRINT),KC_X ,  KC_C  ,   KC_V ,   KC_B , KC_ALGR,TT(NAV),      TT(MOU), QK_LEAD, KC_K,   KC_M ,DE_COMM, DE_DOT ,DE_MINS, TT(NUM),
                              KC_LALT, KC_LCTL,SC_SENT,TD(HA_SLA),MO(SPEC),   KC_RSFT,KC_BSPC,KC_SPC,KC_RCTL, TD(CAPS_CW)
    ),

  /*
  * SPECIAL
  *
  * ,-------------------------------------------.                              ,-------------------------------------------.
  * |  TRAN  |  @   |      |  "   |  [   |  !   |                              |  ´   |  ]   |  Ù Ü |      |      |        |
  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
  * |  TRAN  | À Ä  |      |  ß   |  (   |  ?   |                              |  `   |  )   |   È  |   Ì  |  Ò Ö |        |
  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
  * |  TRAN  |  |   |   ð  |   ñ  |  {   |  =   | TRAN | TRAN |  | TRAN | TRAN |  \   |  }   |   ;  |   :  |      |  TRAN  |
  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
  *                        | TRAN | TRAN | TRAN | TRAN | TRAN |  | TRAN | TRAN | TRAN | TRAN | TRAN |
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        `----------------------------------'  `----------------------------------'
  */
     [_SPECIAL] = LAYOUT(
       KC_TRNS , DE_AT  , _______ , DE_DQUO, DE_LBRC, DE_EXLM,                                     DE_ACUT,DE_RBRC,TD(SPEC_U), _______, _______, _______,
       KC_TRNS,TD(SPEC_A), _______, DE_SS  , DE_LPRN, DE_QUES,                                     DE_GRV , DE_RPRN,M_EGRAVE,M_IGRAVE,TD(SPEC_O), _______,
       KC_TRNS , DE_PIPE, M_ETH   ,M_NTILDE, DE_LCBR, DE_EQL , KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, DE_BSLS, DE_RCBR, DE_SCLN, DE_COLN, _______, KC_TRNS,
                                 M_CHARAMAP, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
     ),

  /*
  * Number Layer
  *
  * ,-------------------------------------------.                              ,-------------------------------------------.
  * |  TRAN  |      |  7 / |  8 ( |  9 ) |      |                              |      |      |      |      |      |        |
  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
  * |  TRAN  | 0 =  |  4 $ |  5 % |  6 & | . :  |                              |      |  +   |   -  |      |      |        |
  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
  * |  TRAN  |      |  1 ! |  2 " |  3 § | , ;  | TRAN |COLEMK|  | TRAN | TRAN |      |  *   |   /  |      |      |        |
  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
  *                        | TRAN | TRAN | TRAN | TRAN | TRAN |  | TRAN | TRAN | TRAN | TRAN | TRAN |
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        `----------------------------------'  `----------------------------------'
  */
     [_NUM] = LAYOUT(
       KC_TRNS, _______, DE_7   , DE_8   , DE_9   , _______,                                     _______, _______, _______, _______, _______, _______,
       KC_TRNS, DE_0   , DE_4   , DE_5   , DE_6   , DE_DOT ,                                     _______, DE_PLUS, DE_MINS, _______, _______, _______,
       KC_TRNS, _______, DE_1   , DE_2   , DE_3   , DE_COMM, KC_TRNS,TO(CLMK),KC_TRNS,KC_TRNS,_______, DE_ASTR, DE_SLSH, _______, _______, _______,
                                  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
     ),

 /*
 * Navigation Layer: Media, navigation, function keys
  *
  * ,-------------------------------------------.                              ,-------------------------------------------.
  * |  TRAN  |      |  F7  |  F8  |  F9  |  F12 |                              | INS  | HOME |  UP  |  END | PGUP |  VOL+  |
  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
  * |  TRAN  |      |  F4  |  F5  |  F6  |  F11 |                              | DEL  | LEFT | DOWN |RIGHT | PGDWN|  VOL-  |
  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
  * |  TRAN  |      |  F1  |  F2  |  F3  |  F10 | TRAN | TRAN |  | TRAN | TRAN |PRINT | CALC |      |      |      |  MUTE  |
  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
  *                        | TRAN | TRAN | TRAN | TRAN | TRAN |  | TRAN | TRAN | TRAN | TRAN | TRAN |
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        `----------------------------------'  `----------------------------------'
 */
    [_NAV] = LAYOUT(
       KC_TRNS, _______, KC_F7  , KC_F8  , KC_F9  , KC_F12 ,                                     KC_INS , KC_HOME,  KC_UP , KC_END , KC_PGUP, KC_AUDIO_VOL_UP,
       KC_TRNS, _______, KC_F4  , KC_F5  , KC_F6  , KC_F11 ,                                     KC_DEL , KC_LEFT, KC_DOWN,KC_RIGHT, KC_PGDN, KC_AUDIO_VOL_DOWN,
       KC_TRNS, _______, KC_F1  , KC_F2  , KC_F3  , KC_F10 , KC_TRNS,TO(CLMK),KC_TRNS,KC_TRNS,KC_PSCR,KC_CALCULATOR, _______, _______, _______, KC_AUDIO_MUTE,
                                  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

 /*
  * Mouse layer
  *
  * ,-------------------------------------------.                              ,-------------------------------------------.
  * | TRAN   |      |      |      |      |      |                              |      |      |      |      |      |        |
  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
  * | TRAN   |      |RCLICK|MCLICK|LCLICK|      |                              |      |      |      |      |      |        |
  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
  * | TRAN   |      |      |      |      |      | TRAN | TRAN |  | TRAN | TRAN |      |      |      |      |      |        |
  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
  *                        | TRAN | TRAN | TRAN | TRAN | TRAN |  | TRAN | TRAN | TRAN | TRAN | TRAN |
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        `----------------------------------'  `----------------------------------'
  */
     [_MOUSE] = LAYOUT(
       KC_TRNS, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
       KC_TRNS, _______, MS_BTN2, MS_BTN3, MS_BTN1, _______,                                     _______, _______, _______, _______, _______, _______,
       KC_TRNS, _______, _______, _______, _______, _______, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, _______, _______, _______, _______, _______, _______,
                                  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
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
#endif

// ************* ENCODER *************
#ifndef ENCODER
/* The default OLED and rotary encoder code can be found at the bottom of qmk_firmware/keyboards/splitkb/kyria/rev1/rev1.c
 * These default settings can be overriden by your own settings in your keymap.c
 * For your convenience, here's a copy of those settings so that you can uncomment them if you wish to apply your own modifications.
 * DO NOT edit the rev1.c file; instead override the weakly defined default functions by your own.
 */
#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise)
{
    if (IS_LAYER_ON(_NAV) || IS_LAYER_ON(_MOUSE)) // on NAV or MOUSE, scroll wheel
    {
        if (clockwise){
          tap_code(KC_MS_WH_DOWN);
        } else{
          tap_code(KC_MS_WH_UP);
        }
    }
    else // volume control on other layers
    {
        if (clockwise){
          tap_code(KC_VOLU);
        } else{
          tap_code(KC_VOLD);
        }
    }
    return false;
};
#endif

#endif

// TODO
// - fix tap dances
// - mouse scroll
// - leader key behaviour
// - charamap macro
//
