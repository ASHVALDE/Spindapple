#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0

typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

#include <fstream>
#include <sstream>
#include <iomanip>
//***Inputs*****
// fileName: The name of the file to open
//***Outputs****
// pixels: A pointer to a byte array. This will contain the pixel data
// width: An int pointer to store the width of the image in pixels
// height: An int pointer to store the height of the image in pixels
// bytesPerPixel: An int pointer to store the number of bytes per pixel that are used in the image
void ReadImage(const char *fileName, byte **pixels, int32 *width, int32 *height, int32 *bytesPerPixel)
{
  // Open the file for reading in binary mode
  FILE *imageFile = fopen(fileName, "rb");
  // Read data offset
  int32 dataOffset;
  fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
  fread(&dataOffset, 4, 1, imageFile);
  // Read width
  fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
  fread(width, 4, 1, imageFile);
  // Read height
  fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
  fread(height, 4, 1, imageFile);
  // Read bits per pixel
  int16 bitsPerPixel;
  fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
  fread(&bitsPerPixel, 2, 1, imageFile);
  // Allocate a pixel array
  *bytesPerPixel = ((int32)bitsPerPixel) / 8;

  // Rows are stored bottom-up
  // Each row is padded to be a multiple of 4 bytes.
  // We calculate the padded row size in bytes
  int paddedRowSize = (int)(4 * ceil((float)(*width) / 4.0f)) * (*bytesPerPixel);
  // We are not interested in the padded bytes, so we allocate memory just for
  // the pixel data
  int unpaddedRowSize = (*width) * (*bytesPerPixel);
  // Total size of the pixel data in bytes
  int totalSize = unpaddedRowSize * (*height);
  *pixels = (byte *)malloc(totalSize);
  // Read the pixel data Row by Row.
  // Data is padded and stored bottom-up
  int i = 0;
  // point to the last row of our pixel array (unpadded)
  byte *currentRowPointer = *pixels + ((*height - 1) * unpaddedRowSize);
  for (i = 0; i < *height; i++)
  {
    // put file cursor in the next row from top to bottom
    fseek(imageFile, dataOffset + (i * paddedRowSize), SEEK_SET);
    // read only unpaddedRowSize bytes (we can ignore the padding bytes)
    fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
    // point to the next row (from bottom to top)
    currentRowPointer -= unpaddedRowSize;
  }

  fclose(imageFile);
}

//***Inputs*****
// fileName: The name of the file to save
// pixels: Pointer to the pixel data array
// width: The width of the image in pixels
// height: The height of the image in pixels
// bytesPerPixel: The number of bytes per pixel that are used in the image
void WriteImage(const char *fileName, byte *pixels, int32 width, int32 height, int32 bytesPerPixel)
{
  // Open file in binary mode
  FILE *outputFile = fopen(fileName, "wb");
  //*****HEADER************//
  // write signature
  const char *BM = "BM";
  fwrite(&BM[0], 1, 1, outputFile);
  fwrite(&BM[1], 1, 1, outputFile);
  // Write file size considering padded bytes
  int paddedRowSize = (int)(4 * ceil((float)width / 4.0f)) * bytesPerPixel;
  int32 fileSize = paddedRowSize * height + HEADER_SIZE + INFO_HEADER_SIZE;
  fwrite(&fileSize, 4, 1, outputFile);
  // Write reserved
  int32 reserved = 0x0000;
  fwrite(&reserved, 4, 1, outputFile);
  // Write data offset
  int32 dataOffset = HEADER_SIZE + INFO_HEADER_SIZE;
  fwrite(&dataOffset, 4, 1, outputFile);

  //*******INFO*HEADER******//
  // Write size
  int32 infoHeaderSize = INFO_HEADER_SIZE;
  fwrite(&infoHeaderSize, 4, 1, outputFile);
  // Write width and height
  fwrite(&width, 4, 1, outputFile);
  fwrite(&height, 4, 1, outputFile);
  // Write planes
  int16 planes = 1; // always 1
  fwrite(&planes, 2, 1, outputFile);
  // write bits per pixel
  int16 bitsPerPixel = bytesPerPixel * 8;
  fwrite(&bitsPerPixel, 2, 1, outputFile);
  // write compression
  int32 compression = NO_COMPRESION;
  fwrite(&compression, 4, 1, outputFile);
  // write image size (in bytes)
  int32 imageSize = width * height * bytesPerPixel;
  fwrite(&imageSize, 4, 1, outputFile);
  // write resolution (in pixels per meter)
  int32 resolutionX = 11811; // 300 dpi
  int32 resolutionY = 11811; // 300 dpi
  fwrite(&resolutionX, 4, 1, outputFile);
  fwrite(&resolutionY, 4, 1, outputFile);
  // write colors used
  int32 colorsUsed = MAX_NUMBER_OF_COLORS;
  fwrite(&colorsUsed, 4, 1, outputFile);
  // Write important colors
  int32 importantColors = ALL_COLORS_REQUIRED;
  fwrite(&importantColors, 4, 1, outputFile);
  // write data
  int i = 0;
  int unpaddedRowSize = width * bytesPerPixel;
  for (i = 0; i < height; i++)
  {
    // start writing from the beginning of last row in the pixel array
    int pixelOffset = ((height - i) - 1) * unpaddedRowSize;
    fwrite(&pixels[pixelOffset], 1, paddedRowSize, outputFile);
  }
  fclose(outputFile);
}

const int Divitions = 50;

int main()
{
  byte *pixels;
  int32 width;
  int32 height;
  int32 bytesPerPixel;
  for (int bpmImages = 0; bpmImages <= 2000; bpmImages++)
  {
      std::stringstream ss;
      ss << std::setw(5) << std::setfill('0') << bpmImages;
      std::string filename = ss.str();
      std::cout<<filename;




  }
  /*
  ReadImage("./frames/01948.bmp", &pixels, &width, &height, &bytesPerPixel);
  std::ofstream myfile;
  myfile.open("prueba.txt");

  //El CONVOLUCIONADOR :00000

  for (int divY = 0; divY < Divitions; divY++)
  {
    for (int divX = 0; divX < Divitions; divX++)
    {

      float values = 0;
      float mesurements = 0;

      int initialXScanPos = divY * (height / Divitions);
      for (int yScan = initialXScanPos; yScan < initialXScanPos +(height / Divitions) ; yScan++)
      {
        int initialXScanPos = divX * (width * 3 / Divitions);
        for (int xScan = initialXScanPos; xScan < initialXScanPos + (width * 3 / Divitions); xScan = xScan + 3)
        {
          byte red = *(&pixels[xScan +(width*yScan*3)]);
          byte green = *(&pixels[xScan+1 +(width*yScan*3)]);
          byte blue = *(&pixels[xScan+2 +(width*yScan*3)]);
          byte alpha = *(&pixels[xScan+3 +(width*yScan*3)]);
          values = values + ((red+green+blue)/3);
          mesurements++;

        }
      }

    float valuePerBlock = values/mesurements;
    if(valuePerBlock>100){
        myfile<<"x";
      }else{
        myfile<<".";
      }
    }
    myfile<<"\n";
  }
  /*

  for (int j = 0; j < height; j++)
  {
    for (int i = 0; i < width*3; i=i+3)
    {
      byte red = *(&pixels[i +(width*j*3)]);
      byte green = *(&pixels[i+1 +(width*j*3)]);
      byte blue = *(&pixels[i+2 +(width*j*3)]);
      byte alpha = *(&pixels[i+3 +(width*j*3)]);

      float b = (red+green+blue)/3;


      if(b>100){
        myfile<<"x";
      }else{
        myfile<<".";
      }


    }
    myfile<<"\n";
  }
  
  myfile.close();

  WriteImage("./frames/prueba.bmp", pixels, width, height, bytesPerPixel);
  free(pixels);
  */
  return 0;
}