#pragma once

#include <cinttypes>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <deque>
#include <vector>

namespace DIGITAL_CONTROL
{
    const double lpf_30_b[] = {0.294199221645671, 0.588398443291342, 0.294199221645671};
    const double lpf_30_a[] = {1.000000000000000, -0.074015770272151, 0.250821930289187};

    const double lpf_10_b[] = {0.066876672401120, 0.133753344802241, 0.066876672401121};
    const double lpf_10_a[] = {1.000000000000000, -1.221537977599278, 0.489063633504805};

    const double lpf_3_b[] = {0.007916873853898, 0.015833747707795, 0.007916873853898};
    const double lpf_3_a[] = {1.000000000000000, -1.766729129140695, 0.798396874103547};

    const double bldiff_30_b[] = {58.839844329134230, 0.000000000000011, -58.839844329134309};
    const double bldiff_30_a[] = {1.000000000000000, -0.074015770272151, 0.250821930289187};
}

template <typename T>
class DF2_IIR
{
public:
    DF2_IIR(const T *_b, const T *_a, uint8_t _order) : order(_order)
    {
        b.resize(order + 1);
        for (int ii = 0; ii < order + 1; ii++)
        {
            b.at(ii) = _b[ii];
        }

        a.resize(order + 1);
        if (_a[0] != 1)
            throw std::runtime_error("invalid poles");

        for (int ii = 0; ii < order + 1; ii++)
        {
            a.at(ii) = _a[ii];
        }

        v.resize(order + 1);
        std::fill(v.begin(), v.end(), 0);
    }

    virtual ~DF2_IIR() {}

    T update(T x_n);

private:
    uint8_t order;
    std::vector<T> a;
    std::vector<T> b;
    std::deque<T> v;
};

template <typename T>
T DF2_IIR<T>::update(T x_n)
{
    v.push_front(0);
    v.pop_back();

    T &v_n = v.front();
    v_n = x_n;

    for (int ii = 1; ii < (int)a.size(); ii++)
    {
        v_n -= a.at(ii) * v.at(ii);
    }

    T y_n = 0;
    for (int ii = 0; ii < (int)b.size(); ii++)
    {
        T b_ii = b.at(ii);
        T v_cur = v.at(ii);
        y_n += b_ii * v_cur;
    }

    return y_n;
}