#include "include/defs.hpp"

using namespace std;

vector<long long> get_sum(vector<vector<int>> data)
{
    vector<long long> res;
    for (int k = 0; k < month_of_year; k++)
    {
        int sum = 0;
        for (int j = 0; j < hour_of_day; j++)
            for (int i = 0; i < day_of_month; i++)
                sum += data[i + k * day_of_month][j];
        res.push_back(sum);
    }
    return res;
}

vector<long long> get_peaks(vector<vector<int>> data, vector<int> &hours)
{
    vector<long long> res;
    for (int k = 0; k < month_of_year; k++)
    {
        vector<long long> sums;
        for (int j = 0; j < hour_of_day; j++)
        {
            sums.push_back(0);
            for (int i = 0; i < day_of_month; i++)
                sums[j] += data[i + k * day_of_month][j];
        }
        auto max = max_element(sums.begin(), sums.end());
        res.push_back(*max);
        hours.push_back(distance(sums.begin(), max));
    }
    return res;
}

int main(int argc, char *argv[])
{
    auto data = get_csv(argv[1], 3);
    vector<int> hours;
    auto peaks = get_peaks(data, hours);
    auto sums = get_sum(data);
    string result = concat(peaks) + "|" + concat(hours) + "|" + concat(sums);
    cout << result;
    log::dbug("send data via un_named pipe:\n" + result);
}