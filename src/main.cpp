#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>

std::mutex mtx;
uint64_t total;

uint64_t maxDivisiblePerfectSquare(uint64_t n, const std::vector<uint64_t>& squares) {
    for (uint64_t i : squares) {
        if (n % i == 0) {
            return i;
        }
    }

    return 1;
}

void tWorker(uint64_t start, uint64_t end, const std::vector<uint64_t>& squares) {
    for (uint64_t i = start; i <= end; i++) {
        uint64_t val = maxDivisiblePerfectSquare(i, squares);
        
        mtx.lock();
        total += val;
        mtx.unlock();
    }
}

int main() {

    uint64_t input = 100;
    std::vector<uint64_t> perfectSquares;

    //generate perfect squares

    //could use SIMD here but cba
    for (uint64_t i = 0; i * i <= input; i ++) {
        perfectSquares.push_back(i * i);
    }

    std::reverse(perfectSquares.begin(), perfectSquares.end());

    uint64_t workerMultiplier = input / 12;
    uint8_t extra = input % 12;

    std::thread workers[12];

    for (unsigned i = 0; i < 12; i++) {
        workers[i] = std::thread(tWorker, workerMultiplier * i + 1, workerMultiplier * (i + 1), perfectSquares);
    }

    std::thread extraWorker(tWorker, workerMultiplier * 12 + 1, workerMultiplier * 12 + extra, perfectSquares);

    for (unsigned i = 0; i < 12; i += 4) {
        workers[i].join();
        workers[i + 1].join();
        workers[i + 2].join();
        workers[i + 3].join();
    }

    extraWorker.join();

    std::cout << "total: " << total << std::endl;

    return 0;
}