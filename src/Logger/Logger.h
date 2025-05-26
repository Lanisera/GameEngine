#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

class Logger {
public:
    enum class Level {
        Info,
        Error,
        Debug,
        Warning
    };
    
    static void Info(const std::string& message) { Log(Level::Info, message); }
    static void Error(const std::string& message) { Log(Level::Error, message); }
    static void Debug(const std::string& message) { Log(Level::Debug, message); }
    static void Warning(const std::string& message) { Log(Level::Warning, message); }

private:
    static std::string GetCurrentTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        
        std::tm local_time;
        #ifdef _WIN32
            localtime_s(&local_time, &now_c);
        #else
            localtime_r(&now_c, &local_time);
        #endif
        
        std::ostringstream oss;
        oss << "[" << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S") << "] ";
        return oss.str();
    }

    static void Log(Level level, const std::string& message) {
        
        std::string cur_time = GetCurrentTime();

        switch (level)
        {
            case Level::Info:
                std::cout << GREEN << cur_time << "[INFO] " << RESET << std::endl;
                break;
            case Level::Debug:
                std::cout << BLUE << cur_time << "[DEBUG] " << RESET << std::endl;
                break;
            case Level::Error:
                std::cout << RED << cur_time << "[ERROR] " << RESET << std::endl;
                break;
            case Level::Warning:
                std::cout << YELLOW << cur_time << "[WARNING] " << RESET << std::endl;
                break;
        
            default:
                break;
        }
    }

    static constexpr const char* RESET  = "\033[0m";
    static constexpr const char* RED    = "\033[31m";
    static constexpr const char* GREEN  = "\033[32m";
    static constexpr const char* YELLOW = "\033[33m";
    static constexpr const char* BLUE   = "\033[34m";
};