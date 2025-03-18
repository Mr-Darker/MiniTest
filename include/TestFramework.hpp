/**
* ==================================================
 *  @file TestFramework.hpp
 *  @brief 基础测试框架，支持测试注册、执行单个或所有测试
 *  @author mrDarker
 *  @date 2025/03/18
 *  @version 1.0
 *  @copyright Copyright (c) 2025 mrDarker. All Rights Reserved.
 * ==================================================
 */

#ifndef TEST_FRAMEWORK_HPP
#define TEST_FRAMEWORK_HPP

#include <iostream>
#include <vector>
#include <functional>
#include "TestLogger.hpp"

class TestBase {
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
    virtual ~TestBase() = default;
};

class TestFramework {
public:
    // 注册测试
    static void RegisterTest(const std::string& name, const std::function<void()> &func) {
        GetTests().push_back({name, func});
    }

    // 运行所有测试
    static void RunAllTests() {
        std::cout << "** Running All Tests **" << std::endl;
        RunFilteredTests([](const std::string&) { return true; }, "AllTests");
    }

    // 按前缀运行测试
    static void RunTestsByPrefix(const std::string& prefix) {
        std::cout << "** Running Tests with Prefix '" << prefix << "' **" << std::endl;
        RunFilteredTests([prefix](const std::string& name) {
            return name.find(prefix) == 0;
        }, "Prefix_" + prefix);
    }

    // 运行指定测试
    static void RunSingleTest(const std::string& testName) {
        std::cout << "** Running Single Tests '" << testName << "' **" << std::endl;
        RunFilteredTests([testName](const std::string& name) {
            return name == testName;
        }, "Single_" + testName);
    }

    // 运行所有被 DISABLED_ 标记的测试
    static void RunDisabledTests() {
        std::cout << "** Running Disabled Tests **" << std::endl;
        RunFilteredTests([](const std::string& name) {
            return name.find("DISABLED_") == 0;
        }, "DisabledTests");
    }

private:
    struct TestCase {
        std::string name;
        std::function<void()> func;
    };

    static std::vector<TestCase>& GetTests() {
        static std::vector<TestCase> tests;
        return tests;
    }

    // 通用测试执行
    static void RunFilteredTests(const std::function<bool(const std::string&)> &filter, const std::string& logCategory) {
        TestLogger logger(logCategory);
        int passed = 0, failed = 0;

        for (const auto&[name, func] : GetTests()) {
            if (!filter(name)) continue;
            std::cout.flush();
            std::cerr.flush();

            std::cout << "[RUNNING] " << name << std::endl;
            try {
                func();
                std::cout << "[PASS] " << name << std::endl;
                logger.LogMessage("[PASS] " + name);
                passed++;
            } catch (const std::exception& ex) {
                std::cerr << "[FAIL] " << name << " - " << ex.what() << std::endl;
                logger.LogMessage("[FAIL] " + name + " - " + ex.what());
                failed++;
            }
        }

        std::cout << "===========================================" << std::endl;
        std::cout << "Total: " << (passed + failed) << ", Passed: " << passed << ", Failed: " << failed << std::endl;
        std::cout << "===========================================" << std::endl;
        std::cout.flush();
        std::cerr.flush();
    }
};

// 适用于 `TEST_F`
template <typename T>
struct TestRegister {
    explicit TestRegister(const std::string& name) {
        TestFramework::RegisterTest(name, []() {
            T testInstance;
            testInstance.Run();
        });
    }
};

// TEST 宏
#define TEST(test_name) \
void test_name(); \
namespace { \
    struct Register_##test_name { \
        Register_##test_name() { TestFramework::RegisterTest(#test_name, test_name); } \
    }; \
    static Register_##test_name g_register_##test_name; \
} \
void test_name()

// TEST_F 宏
#define TEST_F(test_fixture, test_name) \
class test_name : public test_fixture { \
public: \
    void Run() { \
        std::cout << "[SETUP] " << #test_name << std::endl; \
        SetUp(); \
        std::cout << "[RUNNING] " << #test_name << std::endl; \
        TestBody(); \
        std::cout << "[TEARDOWN] " << #test_name << std::endl; \
        TearDown(); \
        std::cout.flush(); \
    } \
    void TestBody(); \
}; \
namespace { \
    struct Register_##test_name { \
        Register_##test_name() { \
            TestFramework::RegisterTest(#test_name, []() { \
                test_name instance; \
                instance.Run(); \
            }); \
        } \
    }; \
    static Register_##test_name g_register_##test_name; \
} \
void test_name::TestBody()

#endif // TEST_FRAMEWORK_HPP
