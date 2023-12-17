#include "include/bmp.hpp"
#include <bits/stdc++.h>

using namespace std;

void print_time(string title, clock_t &start)
{
    double timer = double(clock() - start) / double(CLOCK_PER_SEC);
    cout << title << ": " << timer * 1000 << setprecision(3) << " ms" << endl;
    start = clock();
}

int main(int argc, char *argv[])
{
    clock_t exec_timer = clock();
    Image *image;
    int kernel[] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
    float filter[] = {.5, .3, .5, .16, .5, .16, .6, .2, .8};

    clock_t start = clock();

    if ((image = create(argv[1])) == NULL)
    {
        cout << "File read error" << endl;
        return 1;
    }
    if (argc == 2)
        print_time("Read", start);

    image->flip_vert();
    if (argc == 2)
        print_time("Flip", start);

    image->blur(kernel, 16);
    if (argc == 2)
        print_time("Blur", start);

    image->purple(filter);
    if (argc == 2)
        print_time("Purple", start);

    image->add_line();
    if (argc == 2)
        print_time("Lines", start);

    set_bmp(image, "output.bmp");
    if (argc == 2)
        print_time("Execution", exec_timer);
    if (argc != 2)
        cout << int(double(clock() - start) * 1000 / double(CLOCK_PER_SEC)) << endl;
    return 0;
}