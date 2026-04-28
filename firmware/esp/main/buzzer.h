#pragma once
#include <driver/gpio.h>

enum Tone
{
    T_REST = 0, // No sound

    // Octave 0
    T_B0 = 31,

    // Octave 1
    T_C1 = 33,
    T_CS1 = 35,
    T_D1 = 37,
    T_DS1 = 39,
    T_E1 = 41,
    T_F1 = 44,
    T_FS1 = 46,
    T_G1 = 49,
    T_GS1 = 52,
    T_A1 = 55,
    T_AS1 = 58,
    T_B1 = 62,

    // Octave 2
    T_C2 = 65,
    T_CS2 = 69,
    T_D2 = 73,
    T_DS2 = 78,
    T_E2 = 82,
    T_F2 = 87,
    T_FS2 = 93,
    T_G2 = 98,
    T_GS2 = 104,
    T_A2 = 110,
    T_AS2 = 117,
    T_B2 = 123,

    // Octave 3
    T_C3 = 131,
    T_CS3 = 139,
    T_D3 = 147,
    T_DS3 = 156,
    T_E3 = 165,
    T_F3 = 175,
    T_FS3 = 185,
    T_G3 = 196,
    T_GS3 = 208,
    T_A3 = 220,
    T_AS3 = 233,
    T_B3 = 247,

    // Octave 4 (Middle C)
    T_C4 = 262,
    T_CS4 = 277,
    T_D4 = 294,
    T_DS4 = 311,
    T_E4 = 330,
    T_F4 = 349,
    T_FS4 = 370,
    T_G4 = 392,
    T_GS4 = 415,
    T_A4 = 440,
    T_AS4 = 466,
    T_B4 = 494,

    // Octave 5
    T_C5 = 523,
    T_CS5 = 554,
    T_D5 = 587,
    T_DS5 = 622,
    T_E5 = 659,
    T_F5 = 698,
    T_FS5 = 740,
    T_G5 = 784,
    T_GS5 = 831,
    T_A5 = 880,
    T_AS5 = 932,
    T_B5 = 988,

    // Octave 6
    T_C6 = 1047,
    T_CS6 = 1109,
    T_D6 = 1175,
    T_DS6 = 1245,
    T_E6 = 1319,
    T_F6 = 1397,
    T_FS6 = 1480,
    T_G6 = 1568,
    T_GS6 = 1661,
    T_A6 = 1760,
    T_AS6 = 1865,
    T_B6 = 1976,

    // Octave 7
    T_C7 = 2093,
    T_CS7 = 2217,
    T_D7 = 2349,
    T_DS7 = 2489,
    T_E7 = 2637,
    T_F7 = 2794,
    T_FS7 = 2960,
    T_G7 = 3136,
    T_GS7 = 3322,
    T_A7 = 3520,
    T_AS7 = 3729,
    T_B7 = 3951,

    // Octave 8
    T_C8 = 4186,
    T_CS8 = 4435,
    T_D8 = 4699,
    T_DS8 = 4978
};

typedef struct
{
    uint32_t tone;
    uint32_t duration; // in milliseconds
} Note;

void buzzer_init(gpio_num_t pin);
void buzzer_play(Note *note);
void buzzer_play_melody(Note *melody, uint8_t length);