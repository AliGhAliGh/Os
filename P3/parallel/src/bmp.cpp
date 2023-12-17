#include "include/bmp.hpp"
#include <pthread.h>

using namespace std;

Image *image;

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
    delete matrix;
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

void Image::set_pixel(unsigned char *pixel, int row, int col)
{
    set_pixel({pixel[0], pixel[1], pixel[2]}, row, col);
}

void Image::set_pixel(Rgb rgb, int row, int col)
{
    (*data[row + 1])[col + 1] = rgb;
}

void *flip_vert(void *arg)
{
    auto param = (Param *)arg;
    auto data = *((vector<vector<Rgb> *> *)param->buffer);
    int rows = image->rows;
    int turn = param->id * rows / (THREAD_COUNT * 2);
    int turn2 = (param->id + 1) * rows / (THREAD_COUNT * 2);

    for (int i = turn; i < turn2; i++)
        data[i + 1]->swap(*data[rows - i]);

    return NULL;
}

void Image::flip_vert()
{
    create_thread(&data, ::flip_vert);
}

void *purple(void *arg)
{
    auto param = (Param *)arg;
    auto filter = (float *)param->buffer;
    int rows = image->rows;
    int cols = image->cols;
    int turn = param->id * rows / THREAD_COUNT;
    int turn2 = (param->id + 1) * rows / THREAD_COUNT;

    for (int i = turn; i < turn2; i++)
        for (int j = 0; j < cols; j++)
        {
            auto pixel = image->get_pixel(i, j);
            float r = filter[0] * pixel->r + filter[1] * pixel->g + filter[2] * pixel->b;
            float g = filter[3] * pixel->r + filter[4] * pixel->g + filter[5] * pixel->b;
            float b = filter[6] * pixel->r + filter[7] * pixel->g + filter[8] * pixel->b;
            int r2 = r;
            int g2 = g;
            int b2 = b;
            limit_int(r2);
            limit_int(g2);
            limit_int(b2);
            image->set_pixel({(unsigned char)r2, (unsigned char)g2, (unsigned char)b2}, i, j);
        }

    return NULL;
}

void Image::purple(float *filter)
{
    create_thread(filter, ::purple);
}

void *add_line(void *arg)
{
    auto param = (Param *)arg;
    int rows = image->rows;
    int cols = image->cols;
    int turn = param->id * rows / THREAD_COUNT;
    int turn2 = (param->id + 1) * rows / THREAD_COUNT;
    int min = rows < cols ? rows : cols;

    // for (int i = turn; i < turn2; i++)
    // {
    //     image->set_pixel(LINE_COLOR, i, i);
    //     for (int j = 0; j < cols; j++)
    //         if (i + (cols - 1 - j) == cols / 2 || i + (cols - 1 - j) - rows == cols / 2)
    //             image->set_pixel(LINE_COLOR, i, j);
    // }

    for (int i = turn; i < turn2; i++)
        for (int j = 0; j < cols; j++)
            for (int k = 1; k < 5; k++)
                if (i + j == k * min / 2)
                    image->set_pixel(LINE_COLOR, i, j);

    return NULL;
}

void Image::add_line()
{
    create_thread(NULL, ::add_line);
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

void *blur(void *arg)
{
    auto param = (Param *)arg;
    auto data = *((ConvBuffer *)param->buffer);
    int rows = image->rows;
    int cols = image->cols;
    int turn = param->id * rows / THREAD_COUNT;
    int turn2 = (param->id + 1) * rows / THREAD_COUNT;

    for (int i = turn; i < turn2; i++)
    {
        vector<Rgb> temp2(cols);
        for (int j = 0; j < cols; j++)
            temp2[j] = calculate(data.kernel, image->get_matrix(i, j), data.coef);
        data.temp->at(i) = temp2;
    }

    return NULL;
}

void Image::blur(int *kernel, int coef)
{
    vector<vector<Rgb>> temp(rows);
    ConvBuffer data = {&temp, kernel, coef};

    create_thread(&data, ::blur);

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
    buffer = old_data;

    int count = 0;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
        {
            count += 3;
            auto rgb = get_pixel(i, j);
            buffer[buff_size - count] = char(rgb->r);
            buffer[buff_size - count - 1] = char(rgb->g);
            buffer[buff_size - count - 2] = char(rgb->b);
        }
    }
}

void *set_image(void *arg)
{
    auto param = (Param *)arg;
    auto buffer = (char *)param->buffer;
    int id = param->id;
    auto data = image->data;
    int cols = image->cols;
    int rows = image->rows;
    int bf_size = image->bf_size;
    int turn = id * rows / THREAD_COUNT;
    int turn2 = (id + 1) * rows / THREAD_COUNT;

    int extra = cols % 4;
    int count = 1 + turn * (3 * cols + extra);
    for (int i = turn; i < turn2; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
        {
            image->set_pixel({(unsigned char)buffer[bf_size - count], (unsigned char)buffer[bf_size - count - 1], (unsigned char)buffer[bf_size - count - 2]}, i, j);
            count += 3;
        }
    }
    // for (int i = turn; i < turn2; i++)
    //     for (int j = 0; j < cols; j++)
    //         image->set_pixel((unsigned char *)&buffer[sizeof(Rgb) * (i * real_cols + j)], i, j);

    return NULL;
}

void Image::init(char *buffer)
{
    image = this;
    old_data = buffer;
    auto file_header = (PBITMAPFILEHEADER)(buffer);
    auto info_header = (PBITMAPINFOHEADER)(buffer + sizeof(BITMAPFILEHEADER));
    bf_size = file_header->bfSize;
    rows = info_header->biHeight;
    cols = info_header->biWidth;

    data = vector<vector<Rgb> *>(rows + 2);
    for (int i = 0; i < rows + 2; i++)
    {
        vector<Rgb> *temp = new vector<Rgb>(cols + 2);
        data[i] = temp;
    }

    create_thread(buffer, set_image);
}

void Image::create_thread(void *buffer, void *(func)(void *))
{
    pthread_t threads[THREAD_COUNT];
    Param params[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        params[i].buffer = buffer;
        params[i].id = i;
        // params[i].image = this;
        pthread_create(&threads[i], NULL, func, &params[i]);
    }

    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);
}

void Image::set_empty(int row)
{
    data[row]->push_back(LINE_COLOR);
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

    Image *res = new Image();
    res->init(buffer);
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