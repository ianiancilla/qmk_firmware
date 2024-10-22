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

// auto-mouse and other pointer stuff
#include "pointing_device.h"

 // LAYERS
enum layers {
    _COLEMAK = 0,
    _SPECIAL,
    _NUM,
    _NAV,
    _MOUSE,
    _AUTO_MOUSE,
};


// Aliases for readability
#define CLMK     _COLEMAK
#define SPEC     _SPECIAL
#define NUM      _NUM
#define NAV      _NAV
#define MOU      _MOUSE



// ************* TRACKPAD *************
#ifndef TRACKPAD

//           SCROLLING WITH TRACKPAD
// ------------ For 40mm TRACKPAD ---------------
// Modify these values to adjust the scrolling speed
float aux_dpi = 0;
#    define SCROLL_DIVISOR_H_BASE 26.0 // Horizontal scroll speed
#    define SCROLL_DIVISOR_V_BASE 13.0 // Vertical scroll speed

float scroll_divisor_h = SCROLL_DIVISOR_H_BASE;
float scroll_divisor_v = SCROLL_DIVISOR_V_BASE;

// Variables to store accumulated scroll values
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

// is the trackpad a scrollwheel now
bool set_scrolling = false;


// AUTO-MOUSE LAYER
void pointing_device_init_user(void) {
    set_auto_mouse_layer(_AUTO_MOUSE); // only required if AUTO_MOUSE_DEFAULT_LAYER is not set to index of <mouse_layer>
    set_auto_mouse_enable(true);       // always required before the auto mouse feature will work
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // SCROLLING FUNCTIONALITY
    //  You can activate the scrolling with a custom keycode in which you set set_scrolling to true
    if (set_scrolling) {
        // Accumulate scroll values based on mouse movement and divisors
        scroll_accumulated_h += (float)mouse_report.x / scroll_divisor_h;
        scroll_accumulated_v += (float)mouse_report.y / scroll_divisor_v;

        // Assign integer parts of accumulated scroll values to the mouse report
        mouse_report.h = (int8_t)scroll_accumulated_h;  // Horizontal scroll
        mouse_report.v = -(int8_t)scroll_accumulated_v; // Vertical scroll (negated for natural scroll)

        // Update accumulated scroll values by subtracting the integer parts
        scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
        scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

        // Prevent cursor movement while scrolling
        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    // Return the modified or unmodified mouse report
    return mouse_report;
}

#endif


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
    M_TM,                   // TM symbol
    M_DPI_INC, // Increase trackpad DPI
    M_DPI_DEC, // Decrease trackpad DPI
    M_DPI_RESET, // Reset trackpad DPI
    M_SNIPE, // Decreases temporarily the trackpad DPI (and restore when released)
             // if using for scrolling : decreases scrolling speed
    M_BLITZ, // Increases temporarily the trackpad DPI (and restore when released)
             // if using for scrolling : increases scrolling speed
    M_SCROLL, // makes trackpad a scrollwheel
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

// TAP-HOLD BEHAVIOUR (different from multi-tap in order to fire off taps asap, rather than waiting for tapping term
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

// MULTI FUNCTION BEHAVIOUR
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} tap_dance_multi_state_t;

typedef struct {
    bool       is_press_action;
    tap_dance_multi_state_t state;
} tap_dance_multi_tap_t;


/* Return an integer that corresponds to what kind of tap dance should be executed.
 *
 * How to figure out tap dance state: interrupted and pressed.
 *
 * Interrupted: If the state of a dance is "interrupted", that means that another key has been hit
 *  under the tapping term. This is typically indicative that you are trying to "tap" the key.
 *
 * Pressed: Whether or not the key is still being pressed. If this value is true, that means the tapping term
 *  has ended, but the key is still being pressed down. This generally means the key is being "held".
 *
 * One thing that is currently not possible with qmk software in regards to tap dance is to mimic the "permissive hold"
 *  feature. In general, advanced tap dances do not work well if they are used with commonly typed letters.
 *  For example "A". Tap dances are best used on non-letter keys that are not hit while typing letters.
 *
 * Good places to put an advanced tap dance:
 *  z,q,x,j,k,v,b, any function key, home/end, comma, semi-colon
 *
 * Criteria for "good placement" of a tap dance key:
 *  Not a key that is hit frequently in a sentence
 *  Not a key that is used frequently to double tap, for example 'tab' is often double tapped in a terminal, or
 *    in a web form. So 'tab' would be a poor choice for a tap dance.
 *  Letters used in common words as a double. For example 'p' in 'pepper'. If a tap dance function existed on the
 *    letter 'p', the word 'pepper' would be quite frustrating to type.
 *
 * For the third point, there does exist the 'TD_DOUBLE_SINGLE_TAP', however this is not fully tested
 *
 */
tap_dance_multi_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else
            return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted)
            return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed)
            return TD_DOUBLE_HOLD;
        else
            return TD_DOUBLE_TAP;
    }

    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TD_TRIPLE_SINGLE_TAP', and define that enum just like 'TD_DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (state->interrupted || !state->pressed)
            return TD_TRIPLE_TAP;
        else
            return TD_TRIPLE_HOLD;
    } else
        return TD_UNKNOWN;
}

// functions specific to each custom tap-dance

// CAPS_CW tap dance
static tap_dance_multi_tap_t capscw_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void capscw_finished(tap_dance_state_t *state, void *user_data) {
    capscw_tap_state.state = cur_dance(state);
    switch (capscw_tap_state.state) {
        case TD_SINGLE_TAP:
            caps_word_toggle();
            break;
        case TD_SINGLE_HOLD:
            tap_code(KC_CAPS);
            break;
        case TD_DOUBLE_TAP:
            caps_word_toggle();
            break;
        default:
            break;
    }
}

void capscw_reset(tap_dance_state_t *state, void *user_data) {
    capscw_tap_state.state = TD_NONE;
}

//// LOCK tap dance
static tap_dance_multi_tap_t lock_tap_state = {.is_press_action = true, .state = TD_NONE};

void lock_finished(tap_dance_state_t *state, void *user_data) {
    lock_tap_state.state = cur_dance(state);
    switch (lock_tap_state.state) {
        case TD_SINGLE_TAP:
            tap_code16(DE_LABK);
            break;
        case TD_SINGLE_HOLD:
            tap_code16(DE_LABK);
            tap_code16(KC_3);
            break;
        case TD_DOUBLE_TAP:
            tap_code16(LGUI(KC_L));
            break;
        default:
            break;
    }
}

void lock_reset(tap_dance_state_t *state, void *user_data) {
    lock_tap_state.state = TD_NONE;
}

// EMOJI tap dance
static tap_dance_multi_tap_t emoji_tap_state = {.is_press_action = true, .state = TD_NONE};

// SPEC_ACCENTS function
typedef struct {
    uint16_t tap_key1;
    uint16_t tap_key2;
    uint16_t hold;
    uint16_t held1;
    uint16_t held2;
} accent_tap_hold_t;

// two key inputs on tap, one on hold
#define ACTION_TAP_SPECIAL(tap_key1, tap_key2, hold) \
        { .fn = {NULL, spec_tap_hold_finished, spec_tap_hold_reset}, .user_data = (void *)&((accent_tap_hold_t){tap_key1, tap_key2, hold, 0}), }

void spec_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    accent_tap_hold_t *tap_hold = (accent_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#    ifndef PERMISSIVE_HOLD
            && !state->interrupted
#    endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held1 = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap_key1);
            register_code16(tap_hold->tap_key2);
            tap_hold->held1 = tap_hold->tap_key1;
            tap_hold->held2 = tap_hold->tap_key2;
        }
    }
}

void spec_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    accent_tap_hold_t *tap_hold = (accent_tap_hold_t *)user_data;

    if (tap_hold->held1) {
        unregister_code16(tap_hold->held1);
        tap_hold->held1 = 0;
    }
    if (tap_hold->held2) {
        unregister_code16(tap_hold->held2);
        tap_hold->held2 = 0;
    }
}

void emoji_finished(tap_dance_state_t *state, void *user_data) {
    emoji_tap_state.state = cur_dance(state);
    switch (emoji_tap_state.state) {
        case TD_SINGLE_TAP:
            tap_code16(DE_EQL);
            tap_code16(DE_RPRN);
            break;
        case TD_SINGLE_HOLD:
            tap_code16(DE_RABK);
            tap_code16(DE_DOT);
            tap_code16(DE_LABK);
            break;
        case TD_DOUBLE_TAP:
            tap_code16(DE_GRV);
            tap_code16(KC_O);
            tap_code16(DE_DOT);
            tap_code16(DE_ACUT);
            tap_code16(KC_O);
            break;
        default:
            break;
    }
}

void emoji_reset(tap_dance_state_t *state, void *user_data) {
    emoji_tap_state.state = TD_NONE;
}

// NOTE: all tap dances that use tap-hold function also need to be mentioned in process_record_user in the "macros" section, around line 55
// Associate tap dance keys with their functionality
tap_dance_action_t tap_dance_actions[] = {
    [HA_SLA] = ACTION_TAP_DANCE_TAP_HOLD(DE_SLSH, DE_HASH),
    [SPEC_A] = ACTION_TAP_SPECIAL(DE_GRV, KC_A, DE_ADIA),
    [SPEC_U] = ACTION_TAP_SPECIAL(DE_GRV, KC_U, DE_UDIA),
    [SPEC_O] = ACTION_TAP_SPECIAL(DE_GRV, KC_O, DE_ODIA),
    [Z_PRINT] = ACTION_TAP_DANCE_TAP_HOLD(DE_Z, KC_PRINT_SCREEN),
    [CAPS_CW] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, capscw_finished, capscw_reset),
    [LOCK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, lock_finished, lock_reset),
    [EMOJI] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, emoji_finished, emoji_reset),
};

// associate custom buttons with their behaviour
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    tap_dance_action_t *action;
    
    switch (keycode) {
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
                SEND_STRING(SS_TAP(X_LGUI) SS_DELAY(10) "character map" SS_DELAY(250) SS_TAP(X_ENT));
            }
            break;

        case M_TM: // ™
            if (record->event.pressed) {
                SEND_STRING(SS_DOWN(X_LALT) SS_TAP(X_P0) SS_TAP(X_P1) SS_TAP(X_P5) SS_TAP(X_P3) SS_UP(X_LALT));
            }
            break;

        case TD(HA_SLA): // list all tap dance keycodes with tap-hold configurations
        case TD(Z_PRINT):
            action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }
            break;

        case TD(SPEC_A):  // list all tap dance keycodes with tap-hold configurations and double key on tap
        case TD(SPEC_U):
        case TD(SPEC_O):
            action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                accent_tap_hold_t *tap_hold = (accent_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap_key1);
                tap_code16(tap_hold->tap_key2);
            }
            break;

        // ******** AUTO-MOUSE LAYER ********
        case M_DPI_INC: //Increase trackpad DPI
            if (record->event.pressed) {
                pointing_device_set_cpi(pointing_device_get_cpi() + 100);
            }
            break;

        case M_DPI_DEC: //Decrease trackpad DPI
            if (record->event.pressed) {
                pointing_device_set_cpi(pointing_device_get_cpi() - 100);
            }
            break;

        case M_DPI_RESET: // Resets trackpad DPI
            if (record->event.pressed) {
                pointing_device_set_cpi(4200);
            }
            break;

        case M_SNIPE: //Decreases temporarily the trackpad DPI (and restore when released)
                      //if using for scrolling : decreases scrolling speed
            if (record->event.pressed) {
                if (set_scrolling) {
                    aux_dpi = pointing_device_get_cpi();
                    pointing_device_set_cpi(200);
                    scroll_divisor_h = SCROLL_DIVISOR_H_BASE * 2.0;
                    scroll_divisor_v = SCROLL_DIVISOR_V_BASE * 2.0;
                } else {
                    pointing_device_set_cpi(pointing_device_get_cpi() - 300);
                }
            } else {
                if (set_scrolling) {
                    pointing_device_set_cpi(aux_dpi);
                    scroll_divisor_h = SCROLL_DIVISOR_H_BASE;
                    scroll_divisor_v = SCROLL_DIVISOR_V_BASE;
                } else {
                    pointing_device_set_cpi(pointing_device_get_cpi() + 300);
                }
            }
            break;

        case M_BLITZ: //Increases temporarily the trackpad DPI (and restore when released)
                      //if using for scrolling : increases scrolling speed
            if (set_scrolling) {
                if (record->event.pressed) {
                    aux_dpi = pointing_device_get_cpi();
                    pointing_device_set_cpi(1000);
                    scroll_divisor_h = SCROLL_DIVISOR_H_BASE / 2.0;
                    scroll_divisor_v = SCROLL_DIVISOR_V_BASE / 2.0;
                } else {
                    pointing_device_set_cpi(aux_dpi);
                    scroll_divisor_h = SCROLL_DIVISOR_H_BASE;
                    scroll_divisor_v = SCROLL_DIVISOR_V_BASE;
                }
            } else
            {
                if (record->event.pressed) {
                pointing_device_set_cpi(pointing_device_get_cpi() + 300);
                } else {
                    pointing_device_set_cpi(pointing_device_get_cpi() - 300);
                }
            }
            break;

        case M_SCROLL: // makes trackpad a scrollwheel
            if (record->event.pressed) {
                set_scrolling = true;
            } else {
                set_scrolling = false;
            }
            return false;
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
 * |  LGUI  |   Z  |   X  |   C  |   V  |   B  | RAlt |TT NAV|  |TTMOUS| LAlt |   K  |   M  | , ;  | . :  | - _  | MO NUM |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Leader| LCtrl| Enter| tap /| MO   |  | RShft|Bckspc| Space| RCtrl|CapsW |
 *                        |      |      | LShft|hold #| SPEC |  |      |      |      |      |CapLck|
 *                        `----------------------------------'  `----------------------------------'
 */
    [_COLEMAK] = LAYOUT(
     KC_ESC  , KC_Q ,  KC_W   ,  KC_F  ,   KC_P ,   KC_G ,                                          KC_J,   KC_L ,  KC_U ,   DE_Y ,TD(EMOJI),TD(LOCK),
     KC_TAB  , KC_A ,  KC_R   ,  KC_S  ,   KC_T ,   KC_D ,                                          KC_H,   KC_N ,  KC_E ,   KC_I ,KC_O   ,DE_CIRC ,
     KC_LGUI,TD(Z_PRINT),KC_X ,  KC_C  ,   KC_V ,   KC_B , KC_ALGR,TT(NAV),TT(_AUTO_MOUSE),KC_LALT, KC_K,   KC_M ,DE_COMM, DE_DOT ,DE_MINS, TT(NUM),
                       QK_LEAD, KC_LCTL,SFT_T(KC_ENT),TD(HA_SLA),MO(SPEC),         KC_RSFT,KC_BSPC,KC_SPC,KC_RCTL, TD(CAPS_CW)
    ),

  /*
  * 
  * SPECIAL
  *
  * ,-------------------------------------------.                              ,-------------------------------------------.
  * |  TRAN  |  @   | TRAN |  "   |  [   |  !   |                              |  ´   |  ]   |  Ù Ü | TRAN | TRAN |  TRAN  |
  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
  * |  TRAN  | À Ä  | TRAN |  ß   |  (   |  ?   |                              |  `   |  )   |   È  |   Ì  |  Ò Ö |  TRAN  |
  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
  * |  TRAN  |  |   |   ð  |   ñ  |  {   |  =   | TRAN | TRAN |  | TRAN | TRAN |  \   |  }   |   ;  |   :  | TRAN |  TRAN  |
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
       KC_TRNS, XXXXXXX, DE_7   , DE_8   , DE_9   , XXXXXXX,                                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
       KC_TRNS, DE_0   , DE_4   , DE_5   , DE_6   , DE_DOT ,                                     XXXXXXX, DE_PLUS, DE_MINS, XXXXXXX, XXXXXXX, XXXXXXX,
       KC_TRNS, XXXXXXX, DE_1   , DE_2   , DE_3   , DE_COMM, KC_TRNS,TO(CLMK),KC_TRNS,  KC_TRNS, XXXXXXX, DE_ASTR, DE_SLSH, XXXXXXX, XXXXXXX, XXXXXXX,
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
       KC_TRNS, XXXXXXX, KC_F7  , KC_F8  , KC_F9  , KC_F12 ,                                     KC_INS , KC_HOME,  KC_UP , KC_END , KC_PGUP, KC_AUDIO_VOL_UP,
       KC_TRNS, XXXXXXX, KC_F4  , KC_F5  , KC_F6  , KC_F11 ,                                     KC_DEL , KC_LEFT, KC_DOWN,KC_RIGHT, KC_PGDN, KC_AUDIO_VOL_DOWN,
       KC_TRNS, XXXXXXX, KC_F1  , KC_F2  , KC_F3  , KC_F10 , KC_TRNS,TO(CLMK),KC_TRNS,KC_TRNS,KC_PSCR,KC_CALCULATOR, XXXXXXX, XXXXXXX, XXXXXXX, KC_AUDIO_MUTE,
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
       KC_TRNS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
       KC_TRNS, XXXXXXX, MS_BTN2, MS_BTN3, MS_BTN1, XXXXXXX,                                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
       KC_TRNS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
     ),

 /*
  * Auto-Mouse layer
  *
  * ,-------------------------------------------.                              ,-------------------------------------------.
  * | TRAN   | BLITZ|      |      |      |      |                              |      |      |      |      | DPI+ |        |
  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
  * | TRAN   | SNIPE|RCLICK|MCLICK|LCLICK|      |                              |      |      |      |      |DPIres|        |
  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
  * | TRAN   |      |      |      |      |      | TRAN | TRAN |  | TRAN | TRAN |      |      |      |      | DPI- |        |
  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
  *                        | TRAN | TRAN | TRAN |SCROLL| TRAN |  | TRAN | TRAN | TRAN | TRAN | TRAN |
  *                        |      |      |      |      |      |  |      |      |      |      |      |
  *                        `----------------------------------'  `----------------------------------'
  */
     [_AUTO_MOUSE] = LAYOUT(
       KC_TRNS, M_BLITZ, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, M_DPI_INC, XXXXXXX,
       KC_TRNS, M_SNIPE,M_SCROLL, MS_BTN3, MS_BTN1, XXXXXXX,                                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, M_DPI_RESET, XXXXXXX,
       KC_TRNS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, M_DPI_DEC, XXXXXXX,
                                  KC_TRNS, KC_TRNS, KC_TRNS, M_SCROLL, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
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
// - leader key behaviour
//
