/**
 * ==================================================
 *  @file TestBenchmark.hpp
 *  @brief 基准测试框架，支持多次运行、平均时间统计和参数化基准测试
 *  @author mrDarker
 *  @date 2025/03/18
 *  @version 1.2
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
    // 普通基准测试（无参数）
    static void RegisterBenchmark(const std::string& name, const std::function<void()>& func, const int iterations) {
        GetBenchmarks()[name] = {func, iterations};
    }

    // 参数化基准测试（有参数）
    template <typename Func, typename... Args>
    static void RegisterBenchmark(const std::string& name, Func func, int iterations, Args... args) {
        auto wrapper = [func, args...]() { func(args...); };
        GetBenchmarks()[name] = {wrapper, iterations};
    }

    // 运行所有基准测试
    static void RunAllBenchmarks() {
        RunFilteredBenchmarks([](const std::string&) { return true; }, "AllBenchmarks");
    }

    // 运行指定的基准测试
    static void RunBenchmark(const std::string& name) {
        RunFilteredBenchmarks([name](const std::string& testName) {
            return testName == name;
        }, "Benchmark_" + name);
    }

    // 单独测试某个函数的平均执行时间
    template <typename ReturnType, typename... Args>
    static void BenchmarkFunction(const std::string& name, ReturnType (*func)(Args...), const int iterations, Args... args) {
        std::cout << "[BENCHMARK] Running " << name << " for " << iterations << " iterations..." << std::endl;
        std::vector<double> times;

        for (int i = 0; i < iterations; ++i) {
            std::cout.flush();
            std::cerr.flush();

            auto start = std::chrono::high_resolution_clock::now();
            if constexpr (sizeof...(Args) > 0) {
                func(args...);  // 如果有参数，调用带参数的函数
            } else {
                func();         // 如果无参数，调用无参数的函数
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;
            times.push_back(duration.count());
        }

        double totalTime = 0;
        for (const double t : times) totalTime += t;
        const double avgTime = totalTime / iterations;

        std::cout << "[BENCHMARK] " << name << " Avg Time: " << avgTime << " ms" << std::endl;
    }

private:
    struct BenchmarkCase {
        std::function<void()> func;
        int iterations{};
    };

    static std::unordered_map<std::string, BenchmarkCase>& GetBenchmarks() {
        static std::unordered_map<std::string, BenchmarkCase> benchmarks;
        return benchmarks;
    }

    // 通用性能测试执行
    static void RunFilteredBenchmarks(const std::function<bool(const std::string&)>& filter, const std::string& logCategory) {
        TestLogger logger(logCategory);

        for (const auto& [fst, snd] : GetBenchmarks()) {
            if (!filter(fst)) continue;

            const auto& [func, iterations] = snd;
            std::cout << "[BENCHMARK] Running " << fst << " for " << iterations << " iterations..." << std::endl;

            std::vector<double> times;
            for (int i = 0; i < iterations; ++i) {
                std::cout.flush();
                std::cerr.flush();

                auto start = std::chrono::high_resolution_clock::now();
                func();
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

// 普通基准测试（无参数）
#define BENCHMARK(test_name, iterations) \
void test_name(); \
namespace { \
    struct Register_##test_name { \
        Register_##test_name() { \
            TestBenchmark::RegisterBenchmark(#test_name, test_name, iterations); \
        } \
    }; \
    static Register_##test_name g_register_##test_name; \
} \
void test_name()

// 参数化基准测试（有参数）
#define BENCHMARK_P(test_name, iterations, ...) \
void test_name(__VA_ARGS__); \
namespace { \
    struct Register_##test_name { \
        Register_##test_name() { \
            TestBenchmark::RegisterBenchmark(#test_name, test_name, iterations, __VA_ARGS__); \
        } \
    }; \
    static Register_##test_name g_register_##test_name; \
} \
void test_name(__VA_ARGS__)

// 指定函数基准测试
#define BENCHMARK_FUNC(func, iterations, ...) \
namespace { \
    struct Register_##func { \
        Register_##func() { \
            TestBenchmark::BenchmarkFunction(#func, func, iterations, ##__VA_ARGS__); \
        } \
    }; \
    static Register_##func g_register_##func; \
}

#endif // TEST_BENCHMARK_HPP