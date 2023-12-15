#include "include/bmp.hpp"
#include <pthread.h>

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

void Image::set_pixel(unsigned char *pixel, int row)
{
    data[row]->push_back({pixel[0], pixel[1], pixel[2]});
}

void Image::set_pixel(Rgb rgb, int row, int col)
{
    (*data[row + 1])[col + 1] = rgb;
}

void *flip_vert(void *arg)
{
    auto param = (Param *)arg;
    auto data = *((vector<vector<Rgb> *> *)param->buffer);
    int rows = param->image->rows;
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
    int rows = param->image->rows;
    int cols = param->image->cols;
    int turn = param->id * rows / THREAD_COUNT;
    int turn2 = (param->id + 1) * rows / THREAD_COUNT;
    for (int i = turn; i < turn2; i++)
        for (int j = 0; j < cols; j++)
        {
            auto pixel = param->image->get_pixel(i, j);
            float r = filter[0] * pixel->r + filter[1] * pixel->g + filter[2] * pixel->b;
            float g = filter[3] * pixel->r + filter[4] * pixel->g + filter[5] * pixel->b;
            float b = filter[6] * pixel->r + filter[7] * pixel->g + filter[8] * pixel->b;
            int r2 = r;
            int g2 = g;
            int b2 = b;
            limit_int(r2);
            limit_int(g2);
            limit_int(b2);
            param->image->set_pixel({(unsigned char)r2, (unsigned char)g2, (unsigned char)b2}, i, j);
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
    int rows = param->image->rows;
    int cols = param->image->cols;
    int turn = param->id * rows / THREAD_COUNT;
    int turn2 = (param->id + 1) * rows / THREAD_COUNT;
    for (int i = turn; i < turn2; i++)
    {
        param->image->set_pixel(LINE_COLOR, i, rows - 1 - i);
        for (int j = 0; j < cols; j++)
            if (i + j == cols / 2 || i + j - rows == cols / 2)
                param->image->set_pixel(LINE_COLOR, i, j);
    }
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
    if (row == rows)
        cout << "row bug" << endl;
    if (col == cols)
        cout << "col bug" << endl;
    return res;
}

void *blur(void *arg)
{
    auto param = (Param *)arg;
    auto data = *((ConvBuffer *)param->buffer);
    int rows = param->image->rows;
    int cols = param->image->cols;
    int turn = param->id * rows / THREAD_COUNT;
    int turn2 = (param->id + 1) * rows / THREAD_COUNT;

    for (int i = turn; i < turn2; i++)
    {
        auto temp2 = new vector<Rgb>;
        for (int j = 0; j < cols; j++)
            temp2->push_back(calculate(data.kernel, param->image->get_matrix(i, j), data.coef));
        (*data.temp)[i] = *temp2;
    }

    return NULL;
}

void Image::blur(int *kernel, int coef)
{
    vector<vector<Rgb>> temp(rows, vector<Rgb>());
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

void *set_image(void *arg)
{
    auto param = (Param *)arg;
    auto buffer = (char *)param->buffer;
    int id = param->id;
    auto data = param->image->data;
    int real_cols = param->image->real_cols;
    int cols = param->image->cols;
    int rows = param->image->rows;
    int turn = id * rows / THREAD_COUNT;
    int turn2 = (id + 1) * rows / THREAD_COUNT;

    for (int i = turn; i < turn2; i++)
    {
        data[i + 1]->at(0) = LINE_COLOR;
        for (int j = 0; j < cols; j++)
        {
            int index = sizeof(Rgb) * (i * real_cols + j);
            data[i + 1]->at(cols - j) =
                {(unsigned char)buffer[index], (unsigned char)buffer[index + 1], (unsigned char)buffer[index + 2]};
        }
        data[i + 1]->at(cols + 1) = LINE_COLOR;
    }

    return NULL;
}

void Image::init(char *buffer)
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

    for (int i = 0; i < rows + 2; i++)
    {
        vector<Rgb> *temp = new vector<Rgb>(cols + 2);
        data.push_back(temp);
    }

    create_thread(buffer, set_image);
    if (data.size() != (long unsigned int)rows + 2)
        cout << "bug row" << endl;
    for (int i = 0; i < rows + 2; i++)
        if (data[i]->size() != (long unsigned int)cols + 2)
            cout << "bug col" << endl;
}

void Image::create_thread(void *buffer, void *(func)(void *))
{
    pthread_t threads[THREAD_COUNT];
    Param params[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        // cout << "created : " << i << endl;
        params[i].buffer = buffer;
        params[i].id = i;
        params[i].image = this;
        pthread_create(&threads[i], NULL, func, &params[i]);
    }

    // cout << "joining..." << endl;

    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    // cout << "finish" << endl;
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