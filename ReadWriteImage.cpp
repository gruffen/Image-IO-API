#include "ReadWriteImage.h"

/* Read single image */
/*
auto* readImage(char * filename) {
  if (strcmp(get_file_extension(filename), "png") == 0)
    return readPNGfile(filename);
  else if (strcmp(get_file_extension(filename), "jpeg") == 0)
    return readJPEGfile(filename);
}*/

/* Returns png array */
png_bytep* readPNGfile(char* filename) {
  FILE* file = fopen(filename, "rb");
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png)
    abort();
  png_infop info = png_create_info_struct(png);
  if(!info)
    abort();
  if(setjmp(png_jmpbuf(png))) abort();
  png_init_io(png, file);
  png_read_info(png, info);
  impng.width      = png_get_image_width(png, info);
  impng.height     = png_get_image_height(png, info);
  impng.color_type = png_get_color_type(png, info);
  impng.bit_depth  = png_get_bit_depth(png, info);

  if (impng.bit_depth == 16)
    png_set_strip_16(png);
  if (impng.color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);
  if (impng.color_type == PNG_COLOR_TYPE_GRAY && impng.bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);
  if (png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  if(impng.color_type == PNG_COLOR_TYPE_RGB ||
    impng.color_type == PNG_COLOR_TYPE_GRAY ||
    impng.color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(impng.color_type == PNG_COLOR_TYPE_GRAY ||
    impng.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  impng.row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * impng.height);
  for(int y = 0; y < impng.height; y++) {
    impng.row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, impng.row_pointers);

  fclose(file);

  return impng.row_pointers;
}

/* Writes PNG file */
int writePNGfile(char* filename, PNGimage_str strpng) {
  int y;

  FILE *file = fopen(filename, "wb");
  if(!file)
    abort();
  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png)
    abort();
  png_infop info = png_create_info_struct(png);
  if (!info) abort();
  if (setjmp(png_jmpbuf(png))) abort();
  png_init_io(png, file);
  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    strpng.width, strpng.height,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  png_write_image(png, strpng.row_pointers);
  png_write_end(png, NULL);

 /*
  for(int y = 0; y < height; y++) {
    free(strpng.row_pointers[y]);
  }
  free(strpng.row_pointers);*/

  fclose(file);

  return 0;
}

/* Returns jpeg byte array */
unsigned char* readJPEGfile(char* filename) {
  unsigned long x, y;
  unsigned int texture_id;
  unsigned long data_size;     // length of the file
  int channels;               //  3 =>RGB   4 =>RGBA
  unsigned int type;
  unsigned char * rowptr[1];    // pointer to an array
  unsigned char * jdata;        // data for the image
  struct jpeg_decompress_struct info; //for our jpeg info
  struct jpeg_error_mgr err;          //the error handler

  FILE* file = fopen(filename, "rb");
  info.err = jpeg_std_error(& err);
  jpeg_create_decompress(& info);   //fills info structure

  //if the jpeg file doesn't load
  if(!file) {
     fprintf(stderr, "Error reading JPEG file %s!", filename);
     return 0;
  }

  jpeg_stdio_src(&info, file);
  jpeg_read_header(&info, TRUE);   // read jpeg file header

  jpeg_start_decompress(&info);    // decompress the file

  //set width and height
  x = info.output_width; im.width = info.output_width;
  y = info.output_height; im.height = info.output_height;
  channels = info.num_components;

  data_size = x * y * 3; im.data_sz = data_size;

  //--------------------------------------------
  // read scanlines one at a time & put bytes
  //    in jdata[] array. Assumes an RGB image
  //--------------------------------------------
  jdata = (unsigned char *)malloc(data_size);
  while (info.output_scanline < info.output_height) // loop
  {
    // Enable jpeg_read_scanlines() to fill our jdata array
    rowptr[0] = (unsigned char *)jdata +  // secret to method
            3* info.output_width * info.output_scanline;

    jpeg_read_scanlines(&info, rowptr, 1);
  }
  //---------------------------------------------------

  jpeg_finish_decompress(&info);   //finish decompressing

  jpeg_destroy_decompress(&info);
  fclose(file);                    //close the file

  im.image_buffer = jdata;
  return jdata;
}

int writeJPEGfile(char * filename, int quality, JPEGimage_str strjpeg) {
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  FILE* file;
  JSAMPROW row_pointer[1];
  int row_stride;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  if ((file = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 1;
  }
  jpeg_stdio_dest(&cinfo, file);
  // CHANGE THIS PART
  cinfo.image_width = strjpeg.width; 	/* image width and height, in pixels */
  cinfo.image_height = strjpeg.height;
  cinfo.input_components = 3;		/* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);
  jpeg_start_compress(&cinfo, TRUE);

  row_stride = cinfo.image_width * 3;

  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = & im.image_buffer[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  fclose(file);

  jpeg_destroy_compress(&cinfo);

  return 1;
}

/* Return array of TIFF data */
uint32* readTIFFfile(char* filename) {
  TIFF* tif = TIFFOpen(filename, "r");
  if (tif) {
    uint32 width, height, row, scanLineSize, bitspersample, photo;
    tdata_t buf;
    int sampleperpixel;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);   imtiff.width = width;
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height); imtiff.height = height;
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &sampleperpixel);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo); imtiff.photo = photo;
      imtiff.bitspersample = bitspersample;
    scanLineSize = TIFFScanlineSize(tif);
      imtiff.sampleperpixel = sampleperpixel;

    buf = _TIFFmalloc(scanLineSize);

    uint32* imageMatrix = new uint32[height * width];

    for (uint32 row = 0; row < height; row++) {
      if (TIFFReadScanline(tif, buf, row, 0) != -1) {
        memcpy(&imageMatrix[row * width], buf, scanLineSize);
        cout << imageMatrix[row * width] << endl;
      } else {
        std::cout << "ERROR READING SCANLINE" << std::endl;
        exit(-1);
      }
    }
    imtiff.data = imageMatrix;
    _TIFFfree(buf);
    TIFFClose(tif);
  }
  return imtiff.data;
}

int writeTIFFfile(char* filename, TIFFImage_str strtiff) {
  TIFF *out = TIFFOpen(filename, "w");
  if (out) {
    uint32 imagelength, imagewidth;
    uint8 * buf;
    uint32 row, col, n, bitspersample, photo;
    uint16 config, nsamples;

    imagelength = strtiff.height;
    imagewidth = strtiff.width;
    config = PLANARCONFIG_CONTIG;
    nsamples = strtiff.sampleperpixel;
    bitspersample = strtiff.bitspersample;
    photo = strtiff.photo;

    uint32* imageMatrix = new uint32[imagelength*imagewidth];
    imageMatrix = strtiff.data;

    TIFFSetField(out, TIFFTAG_IMAGELENGTH, imagelength);
    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, imagewidth);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, nsamples);
  //  TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_LZW) ;
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bitspersample) ;
    //TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, 1);
  //  TIFFSetField(out, TIFFTAG_ORIENTATION, (int)ORIENTATION_TOPLEFT);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, photo);

    buf = new uint8 [imagewidth*nsamples] ;
    for (uint32 row = 0; row < imagelength; row++){
       if (TIFFWriteScanline(out, imageMatrix, row, 0) != 1 )
       {
           std::cout << "Unable to write a row." <<std::endl ;
           break ;
       }
     }
     //_TIFFfree(buf);
     TIFFClose(out);
  }
  return 0;
}

const char* get_file_extension(const char* filename) {
  const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

void print_array(unsigned char* a) {
  for (int i = 0; i < im.data_sz; i++) {
    cout << a[i];
  }
}

int testTIFF(char* filename) {
  TIFF* tif = TIFFOpen(filename, "r");
  uint32 width, height, row, scanLineSize, bitspersample, photo, planar;
  tdata_t buf;
  int sampleperpixel;
  uint32* imageMatrix;

  if (tif) {
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &sampleperpixel);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo);
    TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar);
    scanLineSize = TIFFScanlineSize(tif);

    buf = _TIFFmalloc(scanLineSize);

    imageMatrix = new uint32[height * width];

    for (uint32 row = 0; row < height; row++) {
      if (TIFFReadScanline(tif, buf, row, 0) != -1) {
        memcpy(&imageMatrix[row * width], buf, scanLineSize);
        //cout << imageMatrix[row * width] << endl;
      } else {
        std::cout << "ERROR READING SCANLINE" << std::endl;
        exit(-1);
      }
    }
    _TIFFfree(buf);
    TIFFClose(tif);
  }

  TIFF *out = TIFFOpen("test4.tiff", "w");
  if (out) {
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bitspersample);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, photo);
    for (uint32 row = 0; row < height; ++row){
       if (TIFFWriteScanline(out, imageMatrix, row, 0) != 1 )
       {
           std::cout << "Unable to write a row." <<std::endl ;
           break ;
       }
     }
     //_TIFFfree(buf);
     TIFFClose(out);
  }
  delete imageMatrix;
  return 0;
}

/*
int main() {
  //readJPEGfile("icecream.jpg");
  //writeJPEGfile("test.jpg", 50);
  //readPNGfile("fish.png");
  //writePNGfile("test2.png", impng);
  //cout << readTIFFfile("glow.tiff") << endl;
  //writeTIFFfile("test3.tiff", imtiff);
  //cout << readTIFFfile("test3.tiff") << endl;
  testTIFF("lena_color.tiff");

  return EXIT_SUCCESS;
}*/
