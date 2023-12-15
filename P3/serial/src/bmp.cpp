#include "include/bmp.hpp"

using namespace std;

void limit_int(int &val)
{
    if (val > 255)
        val = 255;
    else if (val < 0)
        val = 0;
}

Rgb calculate(int *kernel, Rgb *matrix, int coef)
{
    int r = 0, g = 0, b = 0;
    for (int i = 0; i < 9; i++)
    {
        r += kernel[i] * matrix[i].r / coef;
        g += kernel[i] * matrix[i].g / coef;
        b += kernel[i] * matrix[i].b / coef;
    }
    limit_int(r);
    limit_int(g);
    limit_int(b);
    Rgb res = {(unsigned char)r, (unsigned char)g, (unsigned char)b};
    return res;
}

void print_pixel(Rgb rgb)
{
    cout << "r:" << (int)rgb.r << ",g:" << (int)rgb.g << ",b:" << (int)rgb.b << endl;
}

void Image::set_pixel(unsigned char *pixel)
{
    auto vec = data.back();
    vec->push_back({pixel[0], pixel[1], pixel[2]});
}

void Image::set_pixel(Rgb rgb, int row, int col)
{
    (*data[row + 1])[col + 1] = rgb;
}

void Image::flip_vert()
{
    for (int i = 1; i < rows / 2 + 1; i++)
    {
        auto tmp = data[i];
        data[i] = data[rows + 1 - i];
        data[rows + 1 - i] = tmp;
    }
}

void Image::purple(float *filter)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            auto pixel = get_pixel(i, j);
            float r = filter[0] * pixel->r + filter[1] * pixel->g + filter[2] * pixel->b;
            float g = filter[3] * pixel->r + filter[4] * pixel->g + filter[5] * pixel->b;
            float b = filter[6] * pixel->r + filter[7] * pixel->g + filter[8] * pixel->b;
            int r2 = r;
            int g2 = g;
            int b2 = b;
            limit_int(r2);
            limit_int(g2);
            limit_int(b2);
            set_pixel({(unsigned char)r2, (unsigned char)g2, (unsigned char)b2}, i, j);
        }
}

void Image::add_line()
{
    for (int i = 0; i < rows; i++)
    {
        set_pixel({255, 255, 255}, i, rows - 1 - i);
        for (int j = 0; j < cols; j++)
            if (i + j == cols / 2 || i + j - rows == cols / 2)
                set_pixel({255, 255, 255}, i, j);
    }
}

Rgb *Image::get_matrix(int row, int col)
{
    auto res = new Rgb[9];
    res[0] = *get_pixel(row - 1, col - 1);
    res[1] = *get_pixel(row - 1, col);
    res[2] = *get_pixel(row - 1, col + 1);
    res[3] = *get_pixel(row, col - 1);
    res[4] = *get_pixel(row, col);
    res[5] = *get_pixel(row, col + 1);
    res[6] = *get_pixel(row + 1, col - 1);
    res[7] = *get_pixel(row + 1, col);
    res[8] = *get_pixel(row + 1, col + 1);
    return res;
}

void Image::blur(int *kernel, int coef)
{
    vector<vector<Rgb>> temp;
    for (int i = 0; i < rows; i++)
    {
        vector<Rgb> temp2;
        for (int j = 0; j < cols; j++)
            temp2.push_back(calculate(kernel, get_matrix(i, j), coef));
        temp.push_back(temp2);
    }
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            set_pixel(temp[i][j], i, j);
}

Rgb *Image::get_pixel(int row, int col)
{
    return &((*data[row + 1])[col + 1]);
}

void Image::get(char *&buffer, int &buff_size)
{
    buff_size = bf_size;
    buffer = old_data + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    for (int i = 0; i < rows; i++)
    {
        for (int j = cols - 1; j >= 0; j--)
        {
            memcpy(buffer, get_pixel(i, j), sizeof(Rgb));
            buffer += sizeof(Rgb);
        }
        buffer += (real_cols - cols) * sizeof(Rgb);
    }
    buffer = old_data;
}

Image::Image(char *buffer)
{
    old_data = buffer;
    auto file_header = (PBITMAPFILEHEADER)(buffer);
    buffer += sizeof(BITMAPFILEHEADER);
    auto info_header = (PBITMAPINFOHEADER)(buffer);
    buffer += sizeof(BITMAPINFOHEADER);
    bf_size = file_header->bfSize;
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    real_cols = 4 * (cols / 4) + (cols % 4 ? 4 : 0);
    set_row(false);
    for (int j = cols; j >= 0; j--)
        set_row(true);
    for (int i = 0; i < rows; i++)
    {
        set_row(false);
        for (int j = cols - 1; j >= 0; j--)
            set_pixel((unsigned char *)&buffer[sizeof(Rgb) * (i * real_cols + j)]);
        set_row(true);
    }
    set_row(false);
    for (int j = cols; j >= 0; j--)
        set_row(true);
}

void Image::set_row(bool is_end)
{
    if (is_end)
    {
        data.back()->push_back({0, 0, 0});
    }
    else
    {
        vector<Rgb> *temp = new vector<Rgb>;
        temp->push_back({0, 0, 0});
        data.push_back(temp);
    }
}

Image *create(const char *fileName)
{
    ifstream file(fileName);
    if (!file)
    {
        cout << "File" << fileName << " doesn't exist!" << endl;
        return NULL;
    }

    file.seekg(0, ios::end);
    streampos length = file.tellg();
    file.seekg(0, ios::beg);

    auto buffer = new char[length];
    file.read(buffer, length);

    Image *res = new Image(buffer);
    return res;
}

void set_bmp(Image *image, const char *nameOfFileToCreate)
{
    ofstream write(nameOfFileToCreate);
    if (!write)
    {
        cout << "Failed to write " << nameOfFileToCreate << endl;
        return;
    }
    char *buffer;
    int size;
    image->get(buffer, size);
    write.write(buffer, size);
}
