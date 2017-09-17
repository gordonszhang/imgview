#include <GL/freeglut.h>
#include <OpenImageIO/imageio.h>
#include <iostream>

using namespace std;
OIIO_NAMESPACE_USING

// Constants
#define WIDTH 300
#define HEIGHT 300
#define ALL 0
#define R 1
#define G 2
#define B 3

// Global variables
int width, height, display_channels;
string extension;
unsigned char *pixels, *pixels_r, *pixels_g, *pixels_b;

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

// Converts various channel formats to RGBA
void channel_convert(unsigned char* pixmap, int channels) {
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
}

int channel_to_gl(int dc) {
    if (dc == ALL) return GL_RGBA;
    if (dc == R) return GL_RED;
    if (dc == G) return GL_GREEN;
    if (dc == B) return GL_BLUE;
}

unsigned char* channel_to_pixmap(int dc) {
    if (dc == ALL) return pixels;
    if (dc == R) return pixels_r;
    if (dc == G) return pixels_g;
    if (dc == B) return pixels_b;
}

void split_channels() {
    pixels_r = new unsigned char[width * height];
    pixels_g = new unsigned char[width * height];
    pixels_b = new unsigned char[width * height];
    rgba_pixel* rgba_pixmap = (rgba_pixel*)(pixels);
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            int index = i * width + j;
            pixels_r[index] = rgba_pixmap[index].r;
            pixels_g[index] = rgba_pixmap[index].g;
            pixels_b[index] = rgba_pixmap[index].b;
        }
    } 
}

void draw() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);  
    glDrawPixels(width, height, channel_to_gl(display_channels), GL_UNSIGNED_BYTE, channel_to_pixmap(display_channels));
    glFlush();
}

void openFile(string filename) {
    string infile;
    if (filename.compare("") == 0) {
        cout << "enter input image filename: ";
        cin >> infile;
    }
    else infile = filename;
    string filetype = infile.substr(infile.find(".") + 1);
    extension = filetype;
    
    ImageInput *in = ImageInput::open(infile);
    if(!in){
        std:cerr << "Could not open " << infile << ", error = " << geterror() << endl;
        return;
    }

    const ImageSpec &spec = in->spec();
    width = spec.width;
    height = spec.height;
    int channels = spec.nchannels;
    //cerr << "channels " << channels << endl;
    unsigned char* pixmap = new unsigned char[width * height * channels];
    //pixel* array = (pixel*)(pixels);
    //cerr << array[0].r;

    int scanlinesize = width * channels * sizeof(pixmap[0]);

    //if(extension.compare("bmp") == 0) in->read_image(TypeDesc::UINT8, &pixmap[0]);
    in->read_image(TypeDesc::UINT8, (char *)pixmap + (height - 1) * scanlinesize, AutoStride, -scanlinesize, AutoStride);

    in->close();
    ImageInput::destroy(in);
    delete pixels;
    if (pixels_r) delete pixels_r;
    if (pixels_r) delete pixels_g;
    if (pixels_r) delete pixels_b;
    channel_convert(pixmap, channels);
    display_channels = ALL;
}

void writeimage(){
  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  string outfilename;

  // get a filename for the image. The file suffix should indicate the image file
  // type. For example: output.png to create a png image file named output
  cout << "enter output image filename: ";
  cin >> outfilename;
    string filetype = outfilename.substr(outfilename.find(".") + 1);
    extension = filetype;
  // create the oiio file handler for the image
  ImageOutput *outfile = ImageOutput::create(outfilename);
  if(!outfile){
    cerr << "Could not create output image for " << outfilename << ", error = " << geterror() << endl;
    return;
  }
  
  // get the current image from the OpenGL framebuffer and store in pixmap
  //glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixmap);

  // open a file for writing the image. The file header will indicate an image of
  // width w, height h, and 4 channels per pixel (RGBA). All channels will be of
  // type unsigned char
  ImageSpec spec(w, h, 4, TypeDesc::UINT8);
  if(!outfile->open(outfilename, spec)){
    cerr << "Could not open " << outfilename << ", error = " << geterror() << endl;
    delete outfile;
    return;
  }

  // write the image to the file. All channel values in the pixmap are taken to be
  // unsigned chars
  int scanlinesize = width * 4 * sizeof(pixels[0]);
  bool write_success;
  write_success = outfile->write_image(TypeDesc::UINT8,
                                            (char *)pixels + (height - 1) * scanlinesize,
                                            AutoStride,
                                            -scanlinesize,
                                            AutoStride);
  if(!write_success){
    cerr << "Could not write image to " << outfilename << ", error = " << geterror() << endl;
    delete outfile;	
    return;
  }
  else
    cout << outfilename << " saved" << endl;
  
  // close the image file after the image is written
  if(!outfile->close()){
    cerr << "Could not close " << outfilename << ", error = " << geterror() << endl;
    delete outfile;
    return;
  }

  // free up space associated with the oiio file handler
  delete outfile;
}

void input(unsigned char key, int x, int y) {
    switch(key) {
        case 'r':		// 'c' - cycle to next color
        case 'R':
            openFile("");
            display_channels = ALL;
            glutReshapeWindow(width, height);
            draw();
        break;
        
        case 'w':
        case 'W':
        writeimage();
        break;

        case '1':
            display_channels = R;
            if (!pixels_r) split_channels();
            draw();
            break;
        case '2':
            display_channels = G;
            if (!pixels_g) split_channels();
            draw();
            break;
        case '3':
            display_channels = B;
            if (!pixels_b) split_channels();
            draw();
            break;
        case 'o':
        case 'O':
            display_channels = ALL;
            draw();
            break;
        case 'q':		// q - quit
        case 'Q':
        case 27:		// esc - quit
        delete pixels;
        delete pixels_r;
        delete pixels_g;
        delete pixels_b;
        exit(0);
        
        default:		// not a valid key -- just ignore it
        return;
    }
}

int main(int argc, char* argv[]) {
    
    // Handle file input
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(WIDTH,HEIGHT);
    glutCreateWindow("imgview");
    glutKeyboardFunc(input);
    glutDisplayFunc(draw);
    
    if (argc > 1) {
        openFile(argv[1]);
        glutReshapeWindow(width, height);
        draw();
    }
    
    glutMainLoop();
    
    return 0;
}