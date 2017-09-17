#ifndef IMAGE_H
#define IMAGE_H

#include "structs.h"
class Image {
    public:
        Image(int, int, int, unsigned char*);
        ~Image();
        void split_channels();
        unsigned char* get_pixmap();
        int get_channel();
        void set_channel(int);
        int get_width();
        int get_height();
    private:
        int width, height, displayed_channel;
        unsigned char *pixels;
        rgba_pixel *pixels_r, *pixels_g, *pixels_b;
};

#endif