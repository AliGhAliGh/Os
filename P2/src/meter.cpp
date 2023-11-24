#include "include/defs.hpp"

using namespace std;

vector<int> get_sum(vector<vector<int>> data)
{
    vector<int> res;
    for (int k = 0; k < month_of_year; k++)
    {
        int sum;
        for (int j = 0; j < hour_of_day; j++)
            for (int i = 0; i < day_of_month; i++)
                sum += data[i][j];
        res.push_back(sum);
    }
    return res;
}

vector<int> get_peaks(vector<vector<int>> data, vector<int> &hours)
{
    vector<int> res;
    for (int k = 0; k < month_of_year; k++)
    {
        vector<int> sums;
        for (int j = 0; j < hour_of_day; j++)
        {
            sums.push_back(0);
            for (int i = 0; i < day_of_month; i++)
            {
                sums[j] += data[i][j];
            }
        }
        auto max = max_element(sums.begin(), sums.end());
        res.push_back(*max);
        res.push_back(distance(sums.begin(), max));
    }
    return res;
}

int main(int argc, char *argv[])
{
    auto data = get_csv(argv[1]);
    vector<int> hours;
    auto peaks = get_peaks(data, hours);
    auto sums = get_sum(data);
    cout << concat(peaks) + "|" + concat(hours) + "|" + concat(sums);
}