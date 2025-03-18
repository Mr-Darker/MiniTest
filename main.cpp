#include "TestAssert.hpp"       // 断言宏
#include "TestFramework.hpp"    // 基础测试
#include "TestParams.hpp"       // 参数测试
#include "TestSuite.hpp"        // 分组测试
#include "TestBenchmark.hpp"    // 性能测试
#include "TestMock.hpp"         // 模拟对象

// ======================================================
// 基础测试
TEST(TestAddition) {
    ASSERT_EQ(2 + 3, 5);
}

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
std::vector<std::vector<std::string>> paramData = {
    {"2", "3", "5"},
    {"4", "1", "5"},
    {"6", "3", "8"}
};

TEST_P(TestParamAddition, paramData) {
    int a = std::stoi(params[0]);
    int b = std::stoi(params[1]);
    int expected = std::stoi(params[2]);
    ASSERT_EQ(a + b, expected);
}
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