#include "TestAssert.hpp"       // 断言宏
#include "TestFramework.hpp"    // 基础测试
#include "TestParams.hpp"       // 参数测试
#include "TestSuite.hpp"        // 分组测试
#include "TestBenchmark.hpp"    // 性能测试
#include "TestMock.hpp"         // 模拟对象

// ======================================================
// 基础测试
// TEST(TestAddition) {
//     ASSERT_EQ(2 + 3, 5);
// }

TEST(TestBoolean) {
    ASSERT_TRUE(true);
}

TEST(TestFailure) {
    ASSERT_TRUE(false);
}

TEST(TestException) {
    ASSERT_THROW(throw std::runtime_error("error"), std::runtime_error);
}

TEST(TestSubtraction) {
    ASSERT_EQ(5 - 3, 2);
}

TEST(DISABLED_TestIgnored) {
    ASSERT_EQ(1 + 1, 3);  // 这个测试会失败，但不会被默认运行
}
// ======================================================

// ======================================================
// 预处理测试
class MyTestFixture : public TestBase {
public:
    void SetUp() override {
        std::cout << "Setup before test\n";
    }
    void TearDown() override {
        std::cout << "Cleanup after test\n";
    }
};

TEST_F(MyTestFixture, TestWithSetup) {
    ASSERT_TRUE(1 + 1 == 2);
}
// ======================================================

// ======================================================
// 参数化测试
std::vector<std::tuple<int, int, int>> additionParams = {
    {1, 2, 3},
    {4, 5, 9},
    {3, 7, 10},
    {6, -2, 4},
    {8, 3, 12}
};

// 参数化测试
TEST_P(TestParamsAddition, additionParams, int a, int b, int expected) {
    ASSERT_EQ(a + b, expected);
}

std::vector<std::tuple<std::string, std::string, std::string>> stringParams = {
    {"Hello", "World", "HelloWorld"},
    {"Mini", "Test", "MiniTest"},
    {"Unit", "Test", "UnitTest"}
};

// 参数化测试
TEST_P(TestStringConcat, stringParams, const std::string& s1, const std::string& s2, const std::string& expected) {
    ASSERT_EQ(s1 + s2, expected);
}

std::vector<std::tuple<int, int>> exceptionParams = {
    {10, 0},
    {-4, 2}
};

TEST_P(TestExceptionHandling, exceptionParams, int a, int b) {
    if (b == 0) {
        ASSERT_THROW(throw std::runtime_error("Divide by zero"), std::runtime_error);
    } else if (a < 0) {
        ASSERT_THROW(throw std::domain_error("Negative square root"), std::domain_error);
    }
}

std::vector<std::tuple<int, int, int>> boundaryParams = {
    {INT_MAX, 1, INT_MIN}, // 溢出
    {INT_MIN, -1, INT_MAX},
    {0, 0, 0},
    {-1, -1, -2}
};

TEST_P(TestBoundaryCases, boundaryParams, int a, int b, int expected) {
    ASSERT_EQ(a + b, expected);
}

std::vector<std::tuple<int>> perfParams = {
    {1000},
    {5000},
    {10000}
};

// TEST_P(TestPerformanceLoop, perfParams, int count) {
//     BENCHMARK_P(TestLoopBenchmark, 5, count) {
//         int sum = 0;
//         for (int i = 0; i < count; ++i) {
//             sum += i;
//         }
//     };
// }
// ======================================================

// ======================================================
// 分组测试
TEST_SUITE(MathTests, TestSuiteAddition) {
    ASSERT_EQ(2 + 3, 5);
}

TEST_SUITE(MathTests, TestMultiplication) {
    ASSERT_EQ(4 * 2, 8);
}

TEST_SUITE(StringTests, TestStringLength) {
    ASSERT_EQ(std::string("hello").size(), 5);
}

struct MySuiteTestFixture {
    MySuiteTestFixture() { std::cout << "[SETUP] MyTestFixture\n"; }
    ~MySuiteTestFixture() { std::cout << "[TEARDOWN] MyTestFixture\n"; }
};

// 测试代码
TEST_SUITE_F(MySuiteTestFixture, MathTests, TestAddition) {
    ASSERT_EQ(1 + 1, 2);
}
// ======================================================

// ======================================================
// 性能测试
BENCHMARK(TestSortingPerformance, 5) {
    std::vector<int> data(1000000);
    std::ranges::generate(data, rand);
    std::ranges::sort(data);
}

BENCHMARK(TestLoopPerformance, 10) {
    int sum = 0;
    for (int i = 0; i < 1000000; ++i) {
        sum += i;
    }
}

// 无参数函数性能测试
void MyTestFunction() {
    std::vector<int> data(100000);
    std::ranges::generate(data.begin(), data.end(), rand);
    std::ranges::sort(data.begin(), data.end());
}

BENCHMARK_FUNC(MyTestFunction, 5);

// 有参数函数性能测试
int MyTestAdd(const int a, const int b) {
    return a + b;
}

BENCHMARK_FUNC(MyTestAdd, 10, 10, 20);

// ======================================================
// 模拟对象
// Mock 方法
MOCK_METHOD(int, GetRandomNumber, ());

TEST(TestMockExample) {
    Mock::SetReturn(GetRandomNumber, 42);
    ASSERT_EQ(GetRandomNumber(), 42);
}

TEST(TestMockReset) {
    Mock::Reset<int>();
    ASSERT_EQ(GetRandomNumber(), 0);
}

TEST(TestMockResetAll) {
    Mock::SetReturn(GetRandomNumber, 100);
    Mock::ResetAll();
    ASSERT_EQ(GetRandomNumber(), 0);
}
// ======================================================

int main() {
    // 运行基础测试和模拟对象测试
    // std::cout << "================= TestFramework ==================" << std::endl;
    TestFramework::RunAllTests();
    TestFramework::RunTestsByPrefix("Test");
    TestFramework::RunDisabledTests();

    // 运行所有参数测试
    // std::cout << "=================== TestParams ===================" << std::endl;
    TestParams::RunAllParamTests();

    // 运行所有性能测试
    // std::cout << "================== TestBenchmark =================" << std::endl;
    TestBenchmark::RunAllBenchmarks();

    // 分组测试
    // 只运行 `MathTests`
    // std::cout << "==================== TestSuite ===================" << std::endl;
    TestSuite::RunSuite("MathTests");
    // 运行所有 Suite
    // std::cout << "================== TestSuite All =================" << std::endl;
    TestSuite::RunAllSuites();

    return 0;
}