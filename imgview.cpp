/* Gordon Zhang
 * CPSC 6040
 * 09.18.2017
 * imgview.cpp
 * Command-line based image viewing application made using OIIO and GLUT.
 */

#include <GL/freeglut.h>
#include <OpenImageIO/imageio.h>
#include <iostream>
#include "image.h"

using namespace std;
OIIO_NAMESPACE_USING

// Constants
#define WIDTH 300
#define HEIGHT 300

// Global variables
int x, y;
Image *image;

// Draw selected pixmap to screen
void draw()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    // Get image paramters and pixmap from class object
    glDrawPixels(image->get_width(), image->get_height(), GL_RGBA, GL_UNSIGNED_BYTE, image->get_pixmap());
    glFlush();
}


// Open image file with OIIO and instantiate global Image pointer
void open_file(string filename)
{
    // Get filename
    string infile;
    if (filename.compare("") == 0)
    {
        cout << "enter input image filename: ";
        cin >> infile;
    }
    else
        infile = filename;
    string filetype = infile.substr(infile.find(".") + 1);

    ImageInput *in = ImageInput::open(infile);
    if (!in)
    {
    std:
        cerr << "Could not open " << infile << ", error = " << geterror() << endl;
        return;
    }

    const ImageSpec &spec = in->spec();
    int width = spec.width;
    int height = spec.height;
    int channels = spec.nchannels;

    unsigned char *pixmap = new unsigned char[width * height * channels];

    int scanlinesize = width * channels * sizeof(pixmap[0]);

    in->read_image(TypeDesc::UINT8, (char *)pixmap + (height - 1) * scanlinesize, AutoStride, -scanlinesize, AutoStride);

    in->close();
    ImageInput::destroy(in);
    delete image;
    image = new Image(width, height, channels, pixmap);
}

// Write pixmap to file defined by user. Code adapted from Ioannis' example
void write_file() 
{
    int w = image->get_width();
    int h = image->get_height();
    string outfilename;

    // get a filename for the image. The file suffix should indicate the image file
    // type. For example: output.png to create a png image file named output
    cout << "enter output image filename: ";
    cin >> outfilename;
    string filetype = outfilename.substr(outfilename.find(".") + 1);

    // create the oiio file handler for the image
    ImageOutput *outfile = ImageOutput::create(outfilename);
    if (!outfile)
    {
        cerr << "Could not create output image for " << outfilename << ", error = " << geterror() << endl;
        return;
    }
    unsigned char *pixels = image->get_pixmap();

    // open a file for writing the image. The file header will indicate an image of
    // width w, height h, and 4 channels per pixel (RGBA). All channels will be of
    // type unsigned char
    ImageSpec spec(w, h, 4, TypeDesc::UINT8);
    if (!outfile->open(outfilename, spec))
    {
        cerr << "Could not open " << outfilename << ", error = " << geterror() << endl;
        delete outfile;
        return;
    }

    // write the image to the file. All channel values in the pixmap are taken to be
    // unsigned chars, and assume RGBA encoding
    int scanlinesize = w * 4 * sizeof(pixels[0]);
    bool write_success;
    write_success = outfile->write_image(TypeDesc::UINT8,
                                         (char *)pixels + (h - 1) * scanlinesize,
                                         AutoStride,
                                         -scanlinesize,
                                         AutoStride);
    if (!write_success)
    {
        cerr << "Could not write image to " << outfilename << ", error = " << geterror() << endl;
        delete outfile;
        return;
    }
    else
        cout << outfilename << " saved" << endl;

    // close the image file after the image is written
    if (!outfile->close())
    {
        cerr << "Could not close " << outfilename << ", error = " << geterror() << endl;
        delete outfile;
        return;
    }

}

// Handle GL window input. Most behavior manipulates Image object
void input(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'r': // 'c' - cycle to next color
    case 'R':
        open_file("");
        glutReshapeWindow(image->get_width(), image->get_height());
        draw();
        break;

    case 'w':
    case 'W':
        write_file();
        break;

    // Image channel selection
    case '1': // Red
        image->set_channel(R);
        draw();
        break;

    case '2': // Green
        image->set_channel(G);
        draw();
        break;

    case '3': // Blue
        image->set_channel(B);
        draw();
        break;

    case 'o':
    case 'O': // All channels
        image->set_channel(ALL);
        draw();
        break;

    case 'q': // q - quit
    case 'Q':
    case 27: // esc - quit
        delete image;
        exit(0);

    default: // not a valid key -- just ignore it
        return;
    }
}

// Handle GL window resizing
void resize(int w, int h)
{
    // set the viewport to be the entire window
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    // Calculate render origin, attempting to center image
    x = (w - image->get_width()) / 2;
    y = (h - image->get_height()) / 2;

    // Get actual raster render origin, commented out 
    // int array[4];
    // glGetIntegerv(GL_CURRENT_RASTER_POSITION, array);

    // Resize image if window height or width is less than that of image
    double x_factor, y_factor;
    x_factor = 1.0;
    y_factor = 1.0;

    // If window width is less than image width and ratio is less than
    // window height / image height, scale based on width
    if (x < y && x < 0)
    {
        x_factor = double(w) / image->get_width();
        y_factor = x_factor;
    }

    // Otherwise, scale by height
    else if (y < x && y < 0)
    {
        y_factor = double(h) / image->get_height();
        x_factor = y_factor;
    }

    // Perform pixel zoom to scale image
    glPixelZoom(x_factor, y_factor);

    x = (w - image->get_width() * x_factor) / 2;
    y = (h - image->get_height() * y_factor) / 2;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    glRasterPos2i(x, y); // Set raster render position, accounting for scale
}

int main(int argc, char *argv[])
{
    // Initialize a blank, black image with default width/height
    image = new Image(WIDTH, HEIGHT, 4, new unsigned char[WIDTH * HEIGHT * 4]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("imgview");
    glutKeyboardFunc(input);
    glutDisplayFunc(draw);
    glutReshapeFunc(resize);

    // Handle command line argument as filename to open
    if (argc > 1)
    {
        open_file(argv[1]);
        glutReshapeWindow(image->get_width(), image->get_height());
        draw();
    }

    glutMainLoop();

    return 0;
}