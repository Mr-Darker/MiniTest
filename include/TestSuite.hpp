/**
 * ==================================================
 *  @file TestSuite.hpp
 *  @brief 测试分组管理，支持运行指定测试套件
 *  @author mrDarker
 *  @date 2025/03/18
 *  @version 1.1
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
#include <thread>
#include "TestLogger.hpp"

class TestSuite {
public:
    // 结构体: 测试用例
    struct TestCase {
        std::string name;           // 测试名称
        std::function<void()> func; // 测试执行函数
    };

    /**
     * @brief 注册测试到指定的 Suite
     * @param suite 测试套件名称
     * @param test 测试名称
     * @param func 测试函数
     */
    static void RegisterTest(const std::string& suite, const std::string& test, const std::function<void()> &func) {
        GetSuites()[suite].push_back({test, func});
    }

    /**
     * @brief 打印所有已注册的测试套件
     */
    static void ListSuites() {
        std::cout << "[AVAILABLE TEST SUITES]" << std::endl;
        for (const auto &suite: GetSuites() | std::views::keys) {
            std::cout << "- " << suite << std::endl;
        }
    }

    /**
     * @brief 执行指定测试套件的 Setup
     */
    static void SetupSuite(const std::string& suite) {
        if (auto& setupFuncs = GetSetupFuncs(); setupFuncs.contains(suite)) setupFuncs[suite]();
    }

    /**
     * @brief 执行指定测试套件的 Teardown
     */
    static void TeardownSuite(const std::string& suite) {
        if (auto& teardownFuncs = GetTeardownFuncs(); teardownFuncs.contains(suite)) teardownFuncs[suite]();
    }

    /**
     * @brief 获取所有测试套件的 Setup 函数映射
     * @return std::unordered_map<std::string, std::function<void()>>&
     */
    static std::unordered_map<std::string, std::function<void()>>& GetSetupFuncs() {
        static std::unordered_map<std::string, std::function<void()>> setupFuncs;
        return setupFuncs;
    }

    /**
     * @brief 获取所有测试套件的 Teardown 函数映射
     * @return std::unordered_map<std::string, std::function<void()>>&
     */
    static std::unordered_map<std::string, std::function<void()>>& GetTeardownFuncs() {
        static std::unordered_map<std::string, std::function<void()>> teardownFuncs;
        return teardownFuncs;
    }

    /**
     * @brief 运行指定测试套件，可选过滤单个测试
     * @param suite 测试套件名称
     * @param testFilter 需要执行的具体测试（默认为空，执行整个套件）
     */
    static void RunSuite(const std::string& suite, const std::string& testFilter = "") {
        auto& suites = GetSuites();
        if (!suites.contains(suite)) {
            std::cerr << "[ERROR] Test suite '" << suite << "' not found.\n";
            return;
        }

        auto& tests = suites[suite];

        // 如果 testFilter 为空，执行整个 Suite
        if (testFilter.empty()) {
            std::cout << "[RUNNING SUITE] " << suite << std::endl;
            RunTests(tests, suite);
            return;
        }

        // 查找是否存在该 Test
        auto it = std::ranges::find_if(tests, [&](const TestCase& test) {
            return test.name == testFilter;
        });

        // 如果存在，执行该 Test
        if (it != tests.end()) {
            std::cout << "[RUNNING SINGLE TEST] " << suite << "::" << testFilter << std::endl;
            RunTests({*it}, suite);
        } else {
            std::cerr << "[ERROR] Test '" << testFilter << "' not found in suite '" << suite << "'.\n";
        }
    }

    /**
     * @brief 运行测试列表
     * @param tests 需要执行的测试列表
     * @param suite 所属的测试套件名称
     */
    static void RunTests(const std::vector<TestCase>& tests, const std::string& suite) {
        TestLogger logger(suite);
        int passed = 0, failed = 0;

        // 执行 Setup
        SetupSuite(suite);

        for (const auto& [name, func] : tests) {
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

        // 执行 Teardown
        TeardownSuite(suite);

        std::cout << "[SUITE] " << suite << " - Passed: " << passed << ", Failed: " << failed << std::endl;
    }

    /**
    * @brief 运行所有测试套件
    */
    static void RunAllSuites() {
        std::vector<std::thread> threads;
        for (const auto &key: GetSuites() | std::views::keys) {
            threads.emplace_back([key]() { RunSuite(key); });
        }
        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }
    }

private:
    /**
    * @brief 获取所有已注册的测试套件
    */
    static std::unordered_map<std::string, std::vector<TestCase>>& GetSuites() {
        static std::unordered_map<std::string, std::vector<TestCase>> suites;
        return suites;
    }
};

// **注册 Setup**
#define TEST_SUITE_SETUP(suite_name, func) \
namespace { struct RegisterSetup_##suite_name { \
    RegisterSetup_##suite_name() { TestSuite::GetSetupFuncs()[#suite_name] = func; } \
}; static RegisterSetup_##suite_name g_registerSetup_##suite_name; }

// **注册 Teardown**
#define TEST_SUITE_TEARDOWN(suite_name, func) \
namespace { struct RegisterTeardown_##suite_name { \
    RegisterTeardown_##suite_name() { TestSuite::GetTeardownFuncs()[#suite_name] = func; } \
}; static RegisterTeardown_##suite_name g_registerTeardown_##suite_name; }

// **注册普通测试**
#define TEST_SUITE(suite_name, test_name) \
void test_name(); \
namespace { \
    struct Register_##test_name { \
        Register_##test_name() { TestSuite::RegisterTest(#suite_name, #test_name, test_name); } \
    }; \
    static Register_##test_name g_register_##test_name; \
} \
void test_name()

// **注册带 Fixture 的测试**
#define TEST_SUITE_F(fixture_name, suite_name, test_name) \
class test_name : public fixture_name { \
public: \
    void Run(); \
}; \
namespace { \
    struct Register_##test_name { \
        Register_##test_name() { \
            TestSuite::RegisterTest(#suite_name, #test_name, []() { \
                test_name instance; \
                instance.Run(); \
            }); \
        } \
    }; \
    static Register_##test_name g_register_##test_name; \
} \
void test_name::Run()

#endif // TEST_SUITE_HPP
