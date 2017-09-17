#ifndef STRUCTS_H
#define STRUCTS_H

#define ALL 0
#define R 1
#define G 2
#define B 3

// C-syntax structs for accessing pixmap data
struct grayscale_pixel {
    unsigned char g;
};

struct rgb_pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct rgba_pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

#endif