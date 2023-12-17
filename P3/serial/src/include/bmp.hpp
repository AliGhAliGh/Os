#ifndef __BMP__
#define __BMP__

#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <string>

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct rgb
{
    unsigned char r, g, b;
} Rgb;

constexpr Rgb LINE_COLOR = {255, 255, 255};

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)

class Image
{
public:
    Image(char *buffer);
    void get(char *&buffer, int &buff_size);
    void flip_vert();
    void blur(int *kernel, int coef);
    void purple(float *filter);
    void add_line();

private:
    void set_pixel(unsigned char *pixel, int row, int col);
    Rgb *get_pixel(int row, int col);
    void set_pixel(Rgb rgb, int row, int col);
    Rgb *get_matrix(int row, int col);
    std::vector<std::vector<Rgb> *> data;
    int cols, rows;
    DWORD bf_size;
    char *old_data;
};

Image *create(const char *fileName);

void set_bmp(Image *image, const char *nameOfFileToCreate);

#endif