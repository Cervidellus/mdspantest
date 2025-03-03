
#define _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS
#include <chrono>
#include <iostream>
#include <mdspan>
#include <random>
#include <print>
#include <vector>

#include <Eigen/Dense>

class ScopedTimer {
public:
    ScopedTimer(std::string name = "unnamed_timer") {
        name_ = name;
        startTime_ = std::chrono::high_resolution_clock::now();
    }

    ~ScopedTimer() {
        std::chrono::duration<double> elapsedTime = std::chrono::high_resolution_clock::now() - startTime_;
        auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime);
        std::cout << "Elapsed time for " << name_ << ":" << elapsedMilliseconds.count() << "\n";
    }

private:
    std::string name_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
};

//The write function just adds randomly a 1 or 0 to each cell in the dataset. 
// Changing the frequency parameter allows you to control the sparseness of the dataset.
//The internal test counts the neighbors in a moore neighborhood. This is done so that memory access is not always sequential.
//For convenience, we skip checking neighborhoods of border cells so we don't have to think about wrapping. 
//Rather than iterating, we explicitely define the neighborhood so we don't need to check every cell to see if it is the center cell, which would be skipped. 
//I increment count if the cell neighborhood is exactly 3, as this is one of the checks you would use for Conway's Game of Life. 
//If I don't print or otherwise do something with count, it will get optimized away by the compiler.
int main()
{
    std::vector<std::vector<int>> vectorvector(500, std::vector<int>(500, 0));
    std::vector<int> mdspanvector(250000, 0);
    auto mySpan = std::mdspan(mdspanvector.data(), 500, 500);
    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> eigenMatrix(500, 500);
    
    std::random_device randomDevice;
    std::mt19937 rng(randomDevice());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    const int reps = 2000;
    const double frequency = 0.5;

    int count = 0;
    int neighborCount = 0;

    //Vector of Vectors
    std::cout << "Testing std::vector<std::vector<int>\n";
    {
        ScopedTimer timer("Write Vector of Vectors");
        for (int a = 0; a < reps; a++)
        {
            for (auto& row : vectorvector)
            {
                for (int x = 0; x < 500; x++)
                {
                    row[x] = distribution(rng) < frequency ? 1 : 0;
                }
            }
        }
    }

    {
        ScopedTimer timer("Read Vector of Vectors");
        count = 0;
        for (int a = 0; a < reps; a++)
        {
            for (int x = 1; x < 499; x++)
            {
                for (int y = 1; y < 499; y++)
                {
                    neighborCount = vectorvector[y - 1][x - 1] +
                        vectorvector[y - 1][x] +
                        vectorvector[y - 1][x + 1] +
                        vectorvector[y][x - 1] +
                        vectorvector[y][x + 1] +
                        vectorvector[y + 1][x - 1] +
                        vectorvector[y + 1][x] +
                        vectorvector[y + 1][x + 1];
                    if (neighborCount == 3) count++;
                }
            }
        }
    }
    std::println("Count:{}", count);

    //std::mdspan
    std::cout << "\nTesting std::mdspan.\n";
    count = 0;
    {
        ScopedTimer timer("write std::mdspan");
        for (int a = 0; a < reps; a++)
        {
            for (int x = 0; x < 500; x++)
            {
                for (int y = 0; y < 500; y++)
                {
                    mySpan[std::array{ x, y }] = distribution(rng) < frequency ? 1 : 0;
                }
            }
        }
    }

    {
        ScopedTimer timer("read std::mdspan");
        count = 0;
        for (int a = 0; a < reps; a++)
        {
            for (int x = 1; x < 499; x++)
            {
                for (int y = 1; y < 499; y++)
                {
                    neighborCount = mySpan[std::array{ x - 1, y + 1 }] +
                        mySpan[std::array{ x, y + 1 }] +
                        mySpan[std::array{ x + 1, y + 1 }] +
                        mySpan[std::array{ x - 1, y }] +
                        mySpan[std::array{ x + 1, y }] +
                        mySpan[std::array{ x - 1, y - 1 }] +
                        mySpan[std::array{ x, y - 1 }] +
                        mySpan[std::array{ x + 1, y - 1 }];
                    if (neighborCount == 3) count++;
                }
            }
        }
    }
    std::println("Count:{}", count);

    //std::vector
    std::cout << "\nTesting std::vector.\n";
    count = 0;
    {
        ScopedTimer timer("write std::vector");
        for (int a = 0; a < reps; a++)
        {
            for (auto& cell : mdspanvector)
            {
                cell = distribution(rng) < frequency ? 1 : 0;
            }
        }
    }

    {
        ScopedTimer timer("Read std::vector");
        count = 0;
        for (int a = 0; a < reps; a++)
        {
            for (int x = 1; x < 499; x++)
            {
                for (int y = 1; y < 499; y++)
                {
                    neighborCount = mdspanvector[(y - 1) * 500 + (x - 1)] +
                        mdspanvector[(y - 1) * 500 + x] +
                        mdspanvector[(y - 1) * 500 + (x + 1)] +
                        mdspanvector[y * 500 + (x - 1)] +
                        mdspanvector[y * 500 + (x + 1)] +
                        mdspanvector[(y + 1) * 500 + (x - 1)] +
                        mdspanvector[(y + 1) * 500 + x] +
                        mdspanvector[(y + 1) * 500 + (x + 1)];
                    if (neighborCount == 3) count++;
                }
            }
        }
    }
    std::println("Count:{}", count);

    // Eigen::Matrix
    std::cout << "\nTesting Eigen::Matrix.\n";
    count = 0;
    {
        ScopedTimer timer("Write Eigen::Matrix");
        for (int a = 0; a < reps; a++) {
            for (int x = 0; x < 500; x++) {
                for (int y = 0; y < 500; y++) {
                    eigenMatrix(x, y) = distribution(rng) < frequency ? 1 : 0;
                }
            }
        }
    }

    {
        ScopedTimer timer("Read Eigen::Matrix");
        count = 0;
        for (int a = 0; a < reps; a++) {
            for (int x = 1; x < 499; x++) {
                for (int y = 1; y < 499; y++) {
                    neighborCount = eigenMatrix(x - 1, y + 1) +
                        eigenMatrix(x, y + 1) +
                        eigenMatrix(x + 1, y + 1) +
                        eigenMatrix(x - 1, y) +
                        eigenMatrix(x + 1, y) +
                        eigenMatrix(x - 1, y - 1) +
                        eigenMatrix(x, y - 1) +
                        eigenMatrix(x + 1, y - 1);
                    if (neighborCount == 3) count++;
                }
            }
        }
    }
    std::println("Count: {}", count);

    return 0;
}
