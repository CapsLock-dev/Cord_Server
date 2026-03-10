#ifndef CL_CORE_LOGGER_HPP
#define CL_CORE_LOGGER_HPP
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <string>
#include <thread>
#include <vector>

namespace cl::core {

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
};

struct LogConfig {
    LogLevel min_level;
    std::size_t queue_size;
    bool colored_output;
};

class Logger {
    public:
    static Logger& get_instance();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    void log(LogLevel level, const std::string& msg);
    void init(const LogConfig& cfg);
    void shutdown();

    private:
    struct LogDecorators {
        std::string color;
        std::string tag;
    };
    struct LogEntry {
        std::string msg;
        LogLevel level;
        std::chrono::system_clock::time_point timestamp;
    };

    Logger() = default;
    ~Logger() = default;

    std::vector<LogEntry> m_queue;
    std::condition_variable m_cond;
    std::atomic<bool> m_shutdown;
    std::atomic<bool> m_flushed;
    std::thread m_worker;
    LogConfig m_config;
    std::mutex m_mutex;

    void run();
    void write(const LogEntry& entry);
    LogDecorators get_decorator(LogLevel lvl);
};

};  // namespace cl::core

#define LOG_TRACE(msg) cl::core::Logger::get_instance().log(cl::core::LogLevel::TRACE, msg);
#define LOG_DEBUG(msg) cl::core::Logger::get_instance().log(cl::core::LogLevel::DEBUG, msg);
#define LOG_INFO(msg) cl::core::Logger::get_instance().log(cl::core::LogLevel::INFO, msg);
#define LOG_WARNING(msg) cl::core::Logger::get_instance().log(cl::core::LogLevel::WARNING, msg);
#define LOG_ERROR(msg) cl::core::Logger::get_instance().log(cl::core::LogLevel::ERROR, msg);
#define LOG_CRITICAL(msg) cl::core::Logger::get_instance().log(cl::core::LogLevel::CRITICAL, msg);

#endif
