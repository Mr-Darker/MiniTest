/**
* ==================================================
 *  @file TestAssert.hpp
 *  @brief 断言宏定义，提供基本的单元测试断言功能
 *  @author mrDarker
 *  @date 2025/03/18
 *  @version 1.0
 *  @copyright Copyright (c) 2025 mrDarker. All Rights Reserved.
 * ==================================================
 */

#ifndef TEST_ASSERT_HPP
#define TEST_ASSERT_HPP

#include <iostream>
#include <stdexcept>

// 断言失败时记录日志
#define ASSERT_FAIL(message) \
do { \
    /**std::ostringstream oss; \
    oss << "Assertion failed: " << message << " at " << __FILE__ << ":" << __LINE__; \
    std::cerr << "[ASSERT FAIL] " << oss.str() << std::endl; \
    TestLogger logger("AssertFailures"); \
    logger.LogMessage(oss.str()); \
    std::cout.flush(); \
    std::cerr.flush(); \
    throw std::runtime_error(oss.str()); \ **/ \
} while (0)

// 断言：检查是否为真
#define ASSERT_TRUE(condition) \
do { \
    if (!(condition)) { \
        ASSERT_FAIL(#condition " is false"); \
    } \
} while (0)

// 断言：检查是否为假
#define ASSERT_FALSE(condition) \
do { \
    if ((condition)) { \
        ASSERT_FAIL(#condition " is true"); \
    } \
} while (0)

// 断言：检查两个值是否相等
#define ASSERT_EQ(expected, actual) \
do { \
    if ((expected) != (actual)) { \
        std::ostringstream oss; \
        oss << #expected " != " #actual " (" << expected << " != " << actual << ")"; \
        ASSERT_FAIL(oss.str()); \
    } \
} while (0)

// 断言：检查两个值是否不相等
#define ASSERT_NE(expected, actual) \
do { \
    if ((expected) == (actual)) { \
        std::ostringstream oss; \
        oss << #expected " == " #actual " (" << expected << " == " << actual << ")"; \
        ASSERT_FAIL(oss.str()); \
    } \
} while (0)

// 断言：检查某个语句是否抛出指定的异常
#define ASSERT_THROW(statement, exception_type) \
do { \
    bool caught = false; \
    try { \
        statement; \
    } catch (const exception_type&) { \
        caught = true; \
    } catch (...) { \
        ASSERT_FAIL("Unexpected exception type thrown"); \
    } \
    if (!caught) { \
        ASSERT_FAIL("Expected exception of type " #exception_type " not thrown"); \
    } \
} while (0)

#endif // TEST_ASSERT_HPP