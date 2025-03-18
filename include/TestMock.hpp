/**
 * ==================================================
 *  @file TestMock.hpp
 *  @brief 模拟对象框架，支持 Mock 方法和返回值设置
 *  @author mrDarker
 *  @date 2025/03/18
 *  @version 1.0
 *  @copyright Copyright (c) 2025 mrDarker. All Rights Reserved.
 * ==================================================
 */

#ifndef TEST_MOCK_HPP
#define TEST_MOCK_HPP

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <map>

class Mock {
public:
    // 设置 Mock 方法的返回值
    template <typename ReturnType>
    static void SetReturn(ReturnType (*func)(), ReturnType returnValue) {
        auto wrapper = [returnValue]() -> ReturnType { return returnValue; };
        GetMockFunctions<ReturnType>()[reinterpret_cast<void*>(func)] = wrapper;
    }

    // 调用 Mock 方法
    template <typename ReturnType>
    static ReturnType Invoke(ReturnType (*func)()) {
        auto& mockMap = GetMockFunctions<ReturnType>();
        auto it = mockMap.find(reinterpret_cast<void*>(func));
        if (it != mockMap.end()) {
            return it->second();
        }
        // std::cerr << "[MOCK ERROR] Function '" << typeid(func).name() << "' not mocked!\n";
        return ReturnType();  // 默认返回 0 / 空值
    }

    // 清除 Mock
    template <typename ReturnType>
    static void Reset() {
        GetMockFunctions<ReturnType>().clear();
    }

    // 清空所有 Mock 方法
    static void ResetAll() {
        mockStorage().clear();
    }

private:
    // 存储不同类型的 Mock 方法
    template <typename ReturnType>
    static std::unordered_map<void*, std::function<ReturnType()>>& GetMockFunctions() {
        static std::unordered_map<void*, std::function<ReturnType()>> mockFunctions;
        return mockFunctions;
    }

    static std::map<std::type_index, void*>& mockStorage() {
        static std::map<std::type_index, void*> storage;
        return storage;
    }
};

// MOCK_METHOD 宏
#define MOCK_METHOD(returnType, functionName, params) \
    returnType functionName params; \
    namespace { \
        struct RegisterMock_##functionName { \
            RegisterMock_##functionName() { \
                Mock::SetReturn(functionName, returnType()); \
            } \
        }; \
        static RegisterMock_##functionName g_register_mock_##functionName; \
    } \
    returnType functionName params { return Mock::Invoke(functionName); }

#endif // TEST_MOCK_HPP