
#include <chrono>
#include <iostream>
#include <mdspan>
#include <print>
#include <random>
#include <vector>

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

int main()
{
    std::vector<std::vector<int>> vectorvector(500, std::vector<int>(500, 0));

    std::vector<int> mdspanvector(250000, 0);

    auto mySpan = std::mdspan(mdspanvector.data(), 500, 500);

    std::random_device randomDevice;
    std::mt19937 rng(randomDevice());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    const int reps = 1000;

    {
        ScopedTimer timer("Vector of Vectors");
        for (int a = 0; a < reps; a++)
        {
            for (auto row : vectorvector)
            {
                for (int i = 0; i < 500; i++)
                {
                    row[i] = distribution(rng) < .5 ? 1 : 0;
                }
            }
        }
        int count = 0;
        for (int a = 0; a < reps; a++)
        {
            for (auto row : vectorvector)
            {
                for (int i = 0; i < 500; i++)
                {
                    if (row[i] == 1) count++;
                }
            }
        }
        std::println("Count:{}", count);
    }

    {
        ScopedTimer timer("std::mdspan");
        for (int a = 0; a < reps; a++)
        {
            for (int i = 0; i < 500; i++)
            {
                for (int j = 0; j < 500; j++)
                {
                    mySpan[std::array{ i,j }] = distribution(rng) < .5 ? 1 : 0;
                }
            }
        }
        int count = 0;
        for (int a = 0; a < reps; a++)
        {
            for (int i = 0; i < 500; i++)
            {
                for (int j = 0; j < 500; j++)
                {
                    if (mySpan[std::array{ i,j }] == 1) count++;
                }
            }
        }
        std::println("Count:{}", count);
    }

    {
        ScopedTimer timer("std::vector");
        for (int a = 0; a < reps; a++)
        {
            for (auto& cell : mdspanvector)
            {
                cell = distribution(rng) < .5 ? 1 : 0;
            }
        }
    }
        int count = 0;
    for (int a = 0; a < reps; a++)
    {
        for (auto& cell : mdspanvector)
        {
            if (cell == 1) count++;
        }
    }
    std::println("Count:{}", count);
    
}

