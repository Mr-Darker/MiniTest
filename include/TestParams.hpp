/**
 * ==================================================
 *  @file TestParams.hpp
 *  @brief 参数化测试框架，支持数据集驱动测试
 *  @author mrDarker
 *  @date 2025/03/18
 *  @version 1.1
 *  @copyright Copyright (c) 2025 mrDarker. All Rights Reserved.
 * ==================================================
 */

#ifndef TEST_PARAMS_HPP
#define TEST_PARAMS_HPP

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <sstream>
#include <tuple>
#include "TestLogger.hpp"

// 参数化测试的存储结构
class TestParams {
public:
    // 注册参数化测试
    template <typename Func, typename... Args>
    static void RegisterParamTest(const std::string& name, Func func, const std::vector<std::tuple<Args...>>& params) {
        for (const auto& paramSet : params) {
            std::ostringstream oss;
            oss << name << FormatParams(paramSet);
            GetParamTests().push_back({oss.str(), [func, paramSet]() { std::apply(func, paramSet); }});
        }
    }

    // 运行所有参数化测试
    static void RunAllParamTests() {
        TestLogger logger("ParamTests");

        int passed = 0, failed = 0;
        for (const auto&[name, func] : GetParamTests()) {
            std::cout.flush();
            std::cerr.flush();
            std::cout << "[RUNNING] " << name << std::endl;
            try {
                func();  // 无参数调用
                std::cout << "[PASS] " << name << std::endl;
                logger.LogTestResult(name, true);
                ++passed;
            } catch (const std::exception& ex) {
                std::cerr << "[FAIL] " << name << " - " << ex.what() << std::endl;
                logger.LogTestResult(name, false);
                ++failed;
            }
        }

        // 打印测试统计信息
        std::cout << "===========================================" << std::endl;
        std::cout << "Total: " << (passed + failed) << ", Passed: " << passed << ", Failed: " << failed << std::endl;
        std::cout << "===========================================" << std::endl;
        std::cout.flush();
        std::cerr.flush();
    }

private:
    struct ParamTestCase {
        std::string name;
        std::function<void()> func;
    };

    static std::vector<ParamTestCase>& GetParamTests() {
        static std::vector<ParamTestCase> paramTests;
        return paramTests;
    }

    // 格式化参数列表
    template <typename Tuple, size_t... Index>
    static std::string FormatTupleImpl(const Tuple& tuple, std::index_sequence<Index...>) {
        std::ostringstream oss;
        ((oss << (Index == 0 ? "" : ", ") << std::get<Index>(tuple)), ...);
        return "(" + oss.str() + ")";
    }

    template <typename... Args>
    static std::string FormatParams(const std::tuple<Args...>& params) {
        return FormatTupleImpl(params, std::index_sequence_for<Args...>{});
    }
};

// 参数化测试宏
#define TEST_P(test_name, param_data, ...) \
void test_name(__VA_ARGS__); \
namespace { \
    struct Register_##test_name { \
        Register_##test_name() { \
            static const auto _param_data = param_data; \
            TestParams::RegisterParamTest(#test_name, test_name, _param_data); \
        } \
    }; \
    static Register_##test_name g_register_##test_name; \
} \
void test_name(__VA_ARGS__)

#endif // TEST_PARAMS_HPP