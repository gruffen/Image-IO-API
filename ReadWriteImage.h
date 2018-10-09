#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <png.h>
#include <jpeglib.h>
#include <tiffio.h>
#include <jerror.h>
#include <cstdlib>

using namespace std;
using std::string;

struct JPEGimage_str {
  int width, height, data_sz;
  JSAMPLE * image_buffer;
} im;

struct PNGimage_str {
  int width, height;
  png_byte color_type;
  png_byte bit_depth;
  png_bytep *row_pointers;
} impng;

struct TIFFImage_str {
  int width, height;
  tstrip_t numberOfStrips;
  uint32 bitspersample, photo;
  uint32* data;
  int sampleperpixel;
} imtiff;

/* Main API functions */

/*
auto* readImage(char * filename); // Read single image
auto* readImage(dir); // Read whole directory of images
auto* writeImage(); */

/* PNG */
png_bytep* readPNGfile(char* filename);
int writePNGfile(char * filename);

/* JPEG */
unsigned char* readJPEGfile(char * filename);
int writeJPEGfile(char * filename, int quality);

/* GIF */
//unsigned char* readGIFfile(char * filename);
//int writeGIFfile(char * filename, int quality);

/* TIFF */
uint32* readTIFFfile(char* filename);
int writeTIFFfile();

/* RAW */
unsigned char* readRAWfile(char * filename);
int writeRAWfile(char * filename, int quality);

/* METADATA*/
int getMetadata(char* filename);

void print_array(unsigned char* a);
