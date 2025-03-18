/**
 * ==================================================
 *  @file TestSuite.hpp
 *  @brief 测试分组管理，支持运行指定测试套件
 *  @author mrDarker
 *  @date 2025/03/18
 *  @version 1.0
 *  @copyright Copyright (c) 2025 mrDarker. All Rights Reserved.
 * ==================================================
 */

#ifndef TEST_SUITE_HPP
#define TEST_SUITE_HPP

#include <vector>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <ranges>
#include "TestLogger.hpp"

class TestSuite {
public:
    static void RegisterTest(const std::string& suite, const std::string& test, const std::function<void()> &func) {
        GetSuites()[suite].push_back({test, func});
    }

    static void RunSuite(const std::string& suite) {
        auto& suites = GetSuites();
        if (!suites.contains(suite)) {
            std::cerr << "[ERROR] Test suite '" << suite << "' not found.\n";
            return;
        }

        TestLogger logger(suite);
        std::cout << "[RUNNING SUITE] " << suite << std::endl;
        int passed = 0, failed = 0;

        for (const auto&[name, func] : suites[suite]) {
            std::cout << "[RUNNING] " << name << std::endl;
            try {
                func();
                std::cout << "[PASS] " << name << std::endl;
                logger.LogTestResult(name, true);
                ++passed;
            } catch (const std::exception& ex) {
                std::cerr << "[FAIL] " << name << " - " << ex.what() << std::endl;
                logger.LogTestResult(name, false);
                ++failed;
            }
        }
        std::cout << "[SUITE] " << suite << " - Passed: " << passed << ", Failed: " << failed << std::endl;
    }

    static void RunAllSuites() {
        for (const auto &key: GetSuites() | std::views::keys) {
            RunSuite(key);
        }
    }

private:
    struct TestCase {
        std::string name;
        std::function<void()> func;
    };

    static std::unordered_map<std::string, std::vector<TestCase>>& GetSuites() {
        static std::unordered_map<std::string, std::vector<TestCase>> suites;
        return suites;
    }
};

#define TEST_SUITE(suite_name, test_name) \
void test_name(); \
namespace { \
    struct Register_##test_name { \
        Register_##test_name() { TestSuite::RegisterTest(#suite_name, #test_name, test_name); } \
    }; \
    static Register_##test_name g_register_##test_name; \
} \
void test_name()

#endif // TEST_SUITE_HPP
