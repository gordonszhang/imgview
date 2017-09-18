/* Gordon Zhang
 * CPSC 6040
 * 09.18.2017
 * image.h
 * Image class header. Class contains descriptive information, 
 * separated RGB channel pixmaps, as well as an RGBA pixmap
 */

#ifndef IMAGE_H
#define IMAGE_H

#include "structs.h"
class Image {
    public:
        Image(int, int, int, unsigned char*);
        ~Image();
        void split_channels();
        void set_channel(int);
        unsigned char* get_pixmap();
        int get_width();
        int get_height();
    private:
        int width, height, displayed_channel;
        unsigned char *pixels;
        rgba_pixel *pixels_r, *pixels_g, *pixels_b;
};

#endif