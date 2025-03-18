/**
 * ==================================================
 *  @file TestLogger.hpp
 *  @brief 测试日志管理，支持日志级别、自动打开文件
 *  @author mrDarker
 *  @date 2025/03/18
 *  @version 1.3
 *  @copyright Copyright (c) 2025 mrDarker. All Rights Reserved.
 * ==================================================
 */

#ifndef TEST_LOGGER_HPP
#define TEST_LOGGER_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

class TestLogger {
public:
    enum class LogLevel { INFO, WARNING, ERROR };

    // 初始化日志
    explicit TestLogger(const std::string& suiteName = "General", const bool isEnable = true) {
        loggingEnabled = isEnable;
        OpenLogFile(suiteName);
    }

    // 启用/禁用日志
    void EnableLogging(const bool status) {
        loggingEnabled = status;
    }

    // 记录测试结果
    void LogTestResult(const std::string& name, const bool result) {
        if (!loggingEnabled) return;

        // 如果日志文件未打开，尝试打开
        if (!logFile.is_open()) {
            OpenLogFile(name);
        }

        if (logFile.is_open()) {
            logFile << "[TEST] " << name << " - " << (result ? "PASS" : "FAIL") << std::endl;
            logFile.flush();
        } else {
            std::cerr << "[LOG ERROR] Log file is not open!" << std::endl;
        }
    }

    // 记录日志
    void LogMessage(const std::string& message, const LogLevel level = LogLevel::INFO) {
        if (!loggingEnabled) return;

        // 如果日志文件未打开，尝试打开
        if (!logFile.is_open()) {
            OpenLogFile("AutoRecovery");
        }

        if (logFile.is_open()) {
            logFile << GetLogPrefix(level) << message << "\n";
            logFile.flush();
        } else {
            std::cerr << "[LOG ERROR] Failed to write log: " << message << std::endl;
        }
    }

    // 关闭日志
    ~TestLogger() {
        if (logFile.is_open()) {
            logFile << "[LOG END] " << GetTimestamp() << "\n";
            logFile.close();
        }
    }

private:
    std::ofstream logFile;
    std::string logFileName;
    bool loggingEnabled;

    // 获取当前时间字符串
    static std::string GetTimestamp() {
        std::time_t t = std::time(nullptr);
        std::tm tm{};
    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return oss.str();
    }

    // 获取日志级别前缀
    static std::string GetLogPrefix(const LogLevel level) {
        switch (level) {
            case LogLevel::INFO: return "[INFO] ";
            case LogLevel::WARNING: return "[WARNING] ";
            case LogLevel::ERROR: return "[ERROR] ";
        }
        return "[UNKNOWN] ";
    }

    // 打开日志文件
    void OpenLogFile(const std::string& suiteName) {
        if (!loggingEnabled) return;  // 日志未启用，不创建文件

        std::filesystem::create_directory("Logs");  // 确保目录存在
        logFileName = "Logs/test_log_" + suiteName + "_" + GetTimestamp() + ".log";
        logFile.open(logFileName, std::ios::out);

        if (logFile) {
            logFile << "[LOG START] Suite: " << suiteName << "\n";
            logFile << "[TEST START] " << GetTimestamp() << "\n";
            logFile.flush();
        } else {
            std::cerr << "[LOG ERROR] Failed to open log file: " << logFileName << std::endl;
        }
    }
};

#endif // TEST_LOGGER_HPP