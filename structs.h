/* Gordon Zhang
 * CPSC 6040
 * 09.18.2017
 * structs.h
 * Various constants and C-style structs used for convenience
 */

#ifndef STRUCTS_H
#define STRUCTS_H

// Constants to define selected channel (int displayed_channel)
#define ALL 0
#define R 1
#define G 2
#define B 3

// C-syntax structs for accessing pixmap data
struct grayscale_pixel { // Grayscale encoding
    unsigned char g;
};

struct rgb_pixel { // RGB encoding
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct rgba_pixel { // RGBA encoding
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

#endif