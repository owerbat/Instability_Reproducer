#include <iostream>
#include <iomanip>
#include <cmath>
#include <tbb/task_scheduler_init.h>
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/enumerable_thread_specific.h"
#include <tbb/mutex.h>

float get_value(const size_t i, const size_t j) { return std::exp(1.0f / ((i + 1) * (j + 1))); }

int main() {
    const size_t n = 10000, p = 10;
    float *array = new float[p];

    tbb::task_scheduler_init init;

    // tbb::mutex mutex;

    tbb::enumerable_thread_specific<float*> ets([]() -> float* {
        return new float[p];
    });

    tbb::parallel_for(tbb::blocked_range<size_t>(0, n), [&](auto& range) {
        for (auto i = range.begin(); i != range.end(); ++i) {
            auto local = ets.local();
            // mutex.lock();
            // std::cout << local << std::endl;
            // mutex.unlock();
            for (size_t j = 0; j < p; ++j) {
                const float value = get_value(i, j);
                local[j] += value;
            }
        }
    });

    bool bFirst = true;
    for (auto it = ets.begin(); it != ets.end(); ++it) {
        auto ptr = *it;
        if(!ptr)
            return 1;
        if(bFirst) {
            for(size_t i = 0; i < p; ++i)
                array[i] = ptr[i];
            bFirst = false;
        }
        else {
            for(size_t i = 0; i < p; ++i)
                array[i] += ptr[i];
        }
    }

    std::cout << "array:";
    for (size_t j = 0; j < p; ++j) {
        std::cout << " " << std::setprecision(12) << array[j];
    }
    std::cout << std::endl;

    return 0;
}
