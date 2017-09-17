#include <GL/freeglut.h>
#include <OpenImageIO/imageio.h>
#include "image.h"
#include "structs.h"

Image::Image(int w, int h, int channels, unsigned char* pixmap) {
    width = w;
    height = h;
    if(channels == 1) {
        // Grayscale
        pixels = new unsigned char[width * height * 4];
        grayscale_pixel* grayscale_pixmap = (grayscale_pixel*)(pixmap);
        rgba_pixel* rgba_pixmap = (rgba_pixel*)(pixels);
        // Convert to RGBA
        for(int i = 0; i < height; ++i) {
            for(int j = 0; j < width; ++j) {
                int index = i * width + j;
                rgba_pixmap[index].r = grayscale_pixmap[index].g;
                rgba_pixmap[index].g = grayscale_pixmap[index].g;
                rgba_pixmap[index].b = grayscale_pixmap[index].g;
                rgba_pixmap[index].a = 255;
            }
        }
    }

    else if(channels == 3) {
        // RGB
        pixels = new unsigned char[width * height * 4];
        rgb_pixel* rgb_pixmap = (rgb_pixel*)(pixmap);
        rgba_pixel* rgba_pixmap = (rgba_pixel*)(pixels);
        // Convert to RGBA
        for(int i = 0; i < height; ++i) {
            for(int j = 0; j < width; ++j) {
                int index = i * width + j;
                rgba_pixmap[index].r = rgb_pixmap[index].r;
                rgba_pixmap[index].g = rgb_pixmap[index].g;
                rgba_pixmap[index].b = rgb_pixmap[index].b;
                rgba_pixmap[index].a = 255;
            }
        }
    }

    else if(channels == 4) {
        pixels = pixmap;
    }

    pixels_r = new rgba_pixel[width * height];
    pixels_g = new rgba_pixel[width * height];
    pixels_b = new rgba_pixel[width * height];
    rgba_pixel* rgba_pixmap = (rgba_pixel*)(pixels);
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            int index = i * width + j;
            pixels_r[index].r = rgba_pixmap[index].r;
            pixels_r[index].g = 0;
            pixels_r[index].b = 0;
            pixels_r[index].a = rgba_pixmap[index].a;

            pixels_g[index].r = 0;
            pixels_g[index].g = rgba_pixmap[index].g;
            pixels_g[index].b = 0;
            pixels_g[index].a = rgba_pixmap[index].a;

            pixels_b[index].r = 0;
            pixels_b[index].g = 0;
            pixels_b[index].b = rgba_pixmap[index].b;
            pixels_b[index].a = rgba_pixmap[index].a;
        }
    } 
    displayed_channel = ALL;
}

Image::~Image() {
    delete pixels;
    delete pixels_r;
    delete pixels_g;
    delete pixels_b;
}

unsigned char* Image::get_pixmap() {
    if (displayed_channel == ALL) {
        return pixels;
    }
    if (displayed_channel == R) {
        return (unsigned char*)pixels_r;
    }
    if (displayed_channel == G) {
        return (unsigned char*)pixels_g;
    }
    if (displayed_channel = B) {
        return (unsigned char*)pixels_b;
    }
}

int Image::get_channel() {
    if (displayed_channel == ALL) return GL_RGBA;
    if (displayed_channel == R) return GL_RED;
    if (displayed_channel == G) return GL_GREEN;
    if (displayed_channel == B) return GL_BLUE;
}

void Image::set_channel(int dc) {
    displayed_channel = dc;
}

int Image::get_width() {
    return width;
}

int Image::get_height() {
    return height;
}