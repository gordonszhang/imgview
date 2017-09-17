#include <GL/freeglut.h>
#include <OpenImageIO/imageio.h>
#include <iostream>
#include "image.h"

using namespace std;
OIIO_NAMESPACE_USING

// Constants
#define WIDTH 200
#define HEIGHT 280

int x, y;
// Global variabless
//int width, height, display_channels;
string extension;
//unsigned char *pixels, *pixels_r, *pixels_g, *pixels_b;
Image* image;

void draw() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);  
    //glDrawPixels(width, height, channel_to_gl(display_channels), GL_UNSIGNED_BYTE, channel_to_pixmap(display_channels));
    glDrawPixels(image->get_width(), image->get_height(), GL_RGBA, GL_UNSIGNED_BYTE, image->get_pixmap());
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
    int width = spec.width;
    int height = spec.height;
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
    delete image;
    image = new Image(width, height, channels, pixmap);
}

void writeimage(){
  //int w = glutGet(GLUT_WINDOW_WIDTH);
  //int h = glutGet(GLUT_WINDOW_HEIGHT);
  int w = image->get_width();
  int h = image->get_height();
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
  unsigned char* pixels = image->get_pixmap();
  // get the current image from the OpenGL framebuffer and store in pixmap
  //glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

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
  //unsigned char* pixels = image->get_pixmap();
  int scanlinesize = w * 4 * sizeof(pixels[0]);
  bool write_success;
  write_success = outfile->write_image(TypeDesc::UINT8,
                                            (char *)pixels + (h - 1) * scanlinesize,
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
  //delete outfile;
  delete pixels;
}

void input(unsigned char key, int x, int y) {
    switch(key) {
        case 'r':		// 'c' - cycle to next color
        case 'R':
            openFile("");
            glutReshapeWindow(image->get_width(), image->get_height());
            draw();
        break;
        
        case 'w':
        case 'W':
        writeimage();
        break;

        case '1':
            image->set_channel(R);
            draw();
            break;
        case '2':
            image->set_channel(G);
            draw();
            break;
        case '3':
            image->set_channel(B);
            draw();
            break;
        case 'o':
        case 'O':
            image->set_channel(ALL);
            draw();
            break;
        case 'q':		// q - quit
        case 'Q':
        case 27:		// esc - quit
        delete image;
        exit(0);
        
        default:		// not a valid key -- just ignore it
        return;
    }
}

void resize(int w, int h) {
	// set the viewport to be the entire window
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    x = (w - image->get_width()) / 2;
    y = (h - image->get_height()) / 2;
    
    //int array[4];
    //glGetIntegerv(GL_CURRENT_RASTER_POSITION, array);
    
    double x_factor, y_factor;
    x_factor = 1.0;
    y_factor = 1.0;
    if (x < y && x < 0) {
        x_factor = double(w) / image->get_width();
        y_factor = x_factor;
    }
    else if (y < x && y < 0) {
        y_factor = double(h) / image->get_height();
        x_factor = y_factor;
    }
    glPixelZoom(x_factor, y_factor);
    x = (w - image->get_width() * x_factor) / 2;
    y = (h - image->get_height() * y_factor) / 2;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    glRasterPos2i(x, y);
}

int main(int argc, char* argv[]) {
    image = new Image(WIDTH, HEIGHT, 4, new unsigned char[WIDTH * HEIGHT * 4]);
    // Handle file input
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(WIDTH,HEIGHT);
    glutCreateWindow("imgview");
    glutKeyboardFunc(input);
    glutDisplayFunc(draw);
    glutReshapeFunc(resize);
    
    if (argc > 1) {
        openFile(argv[1]);
        glutReshapeWindow(image->get_width(), image->get_height());
        draw();
    }
    
    glutMainLoop();
    
    return 0;
}