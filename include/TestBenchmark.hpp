/**
 * ==================================================
 *  @file TestBenchmark.hpp
 *  @brief 基准测试框架，支持多次运行和平均时间统计
 *  @author mrDarker
 *  @date 2025/03/18
 *  @version 1.1
 *  @copyright Copyright (c) 2025 mrDarker. All Rights Reserved.
 * ==================================================
 */

#ifndef TEST_BENCHMARK_HPP
#define TEST_BENCHMARK_HPP

#include <chrono>
#include <functional>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "TestLogger.hpp"

class TestBenchmark {
public:
    // 注册基准测试
    static void RegisterBenchmark(const std::string& name, const std::function<void()> &func, const int iterations) {
        GetBenchmarks()[name] = {func, iterations};
    }

    // 运行所有基准测试
    static void RunAllBenchmarks() {
        RunFilteredBenchmarks([](const std::string&) { return true; }, "AllBenchmarks");
    }

    // 运行指定基准测试
    static void RunBenchmark(const std::string& name) {
        RunFilteredBenchmarks([name](const std::string& testName) {
            return testName == name;
        }, "Benchmark_" + name);
    }

private:
    struct BenchmarkCase {
        std::function<void()> func;
        int iterations;
    };

    static std::unordered_map<std::string, BenchmarkCase>& GetBenchmarks() {
        static std::unordered_map<std::string, BenchmarkCase> benchmarks;
        return benchmarks;
    }

    // 通用性能测试执行
    static void RunFilteredBenchmarks(const std::function<bool(const std::string&)> &filter, const std::string& logCategory) {
        TestLogger logger(logCategory);

        for (const auto&[fst, snd] : GetBenchmarks()) {
            if (!filter(fst)) continue;
            std::cout.flush();
            std::cerr.flush();

            const auto& [func, iterations] = snd;
            std::cout << "[BENCHMARK] Running " << fst << " for " << iterations << " iterations..." << std::endl;

            std::vector<double> times;
            for (int i = 0; i < iterations; ++i) {
                auto start = std::chrono::high_resolution_clock::now();
                func(); // 执行测试代码
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> duration = end - start;
                times.push_back(duration.count());
            }

            double totalTime = 0;
            for (const double t : times) totalTime += t;
            const double avgTime = totalTime / iterations;

            std::cout << "[BENCHMARK] " << fst << " Avg Time: " << avgTime << " ms" << std::endl;
            logger.LogMessage("[BENCHMARK] " + fst + " Avg Time: " + std::to_string(avgTime) + " ms");
        }
    }
};

// BENCHMARK 宏
#define BENCHMARK(test_name, iterations) \
void test_name(); \
namespace { \
    struct Register_##test_name { \
        Register_##test_name() { TestBenchmark::RegisterBenchmark(#test_name, test_name, iterations); } \
    }; \
    static Register_##test_name g_register_##test_name; \
} \
void test_name()

#endif // TEST_BENCHMARK_HPP
