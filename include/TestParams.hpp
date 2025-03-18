/**
* ==================================================
 *  @file TestParams.hpp
 *  @brief 参数化测试框架，支持测试数据集驱动测试
 *  @author mrDarker
 *  @date 2025/03/18
 *  @version 1.0
 *  @copyright Copyright (c) 2025 mrDarker. All Rights Reserved.
 * ==================================================
 */

#ifndef TEST_PARAMS_HPP
#define TEST_PARAMS_HPP

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include "TestLogger.hpp"

using ParamTestFunc = std::function<void(const std::vector<std::string>&)>;

class TestParams {
public:
    static void RegisterParamTest(const std::string& name, const ParamTestFunc& func, const std::vector<std::vector<std::string>>& params) {
        for (const auto& paramSet : params) {
            std::string fullName = name + "(";
            for (const auto& p : paramSet) fullName += p + ",";
            fullName.pop_back();
            fullName += ")";

            GetParamTests().push_back({fullName, func, paramSet});
        }
    }

    static void RunAllParamTests() {
        TestLogger logger("ParamTests");

        for (const auto&[name, func, params] : GetParamTests()) {
            std::cout << "[RUNNING] " << name << std::endl;
            try {
                func(params);
                std::cout << "[PASS] " << name << std::endl;
                logger.LogTestResult(name, true);
            } catch (const std::exception& ex) {
                std::cerr << "[FAIL] " << name << " - " << ex.what() << std::endl;
                logger.LogTestResult(name, false);
            }
        }

        // 确保立即输出
        std::cout.flush();
        std::cerr.flush();
    }

private:
    struct ParamTestCase {
        std::string name;
        ParamTestFunc func;
        std::vector<std::string> params;
    };

    static std::vector<ParamTestCase>& GetParamTests() {
        static std::vector<ParamTestCase> paramTests;
        return paramTests;
    }
};

// 参数化测试宏
#define TEST_P(test_name, param_data) \
void test_name(const std::vector<std::string>& params); \
namespace { \
    struct Register_##test_name { \
        Register_##test_name() { \
            static const std::vector<std::vector<std::string>> _param_data = param_data; \
            TestParams::RegisterParamTest(#test_name, [](const std::vector<std::string>& params) { test_name(params); }, _param_data); \
        } \
    }; \
    static Register_##test_name g_register_##test_name; \
} \
void test_name(const std::vector<std::string>& params)

#endif // TEST_PARAMS_HPP
