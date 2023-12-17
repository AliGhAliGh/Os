#ifndef __BMP__
#define __BMP__

#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <string>

typedef struct rgb
{
    unsigned char r, g, b;
} Rgb;

constexpr Rgb LINE_COLOR = {255, 255, 255};
constexpr int THREAD_COUNT = 4;

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#define CLOCK_PER_SEC 2600000

typedef struct convolution_buffer
{
    std::vector<std::vector<Rgb>> *temp;
    int *kernel;
    int coef;
} ConvBuffer;

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
    void init(char *buffer);
    void get(char *&buffer, int &buff_size);
    void flip_vert();
    void blur(int *kernel, int coef);
    void purple(float *filter);
    void add_line();
    void set_empty(int row);
    void set_pixel(unsigned char *pixel, int row, int col);
    Rgb *get_matrix(int row, int col);
    void set_pixel(Rgb rgb, int row, int col);
    Rgb *get_pixel(int row, int col);

    int cols, rows;
    DWORD bf_size;
    std::vector<std::vector<Rgb> *> data;

private:
    void create_thread(void *buffer, void *(func)(void *));

    char *old_data;
};

typedef struct thread_param
{
    void *buffer;
    int id;
    // Image *image;
} Param;

Image *create(const char *fileName);

void set_bmp(Image *image, const char *nameOfFileToCreate);

#endif