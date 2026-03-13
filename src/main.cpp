#include <core/logger.hpp>

int main() {
    auto& logger = cl::core::Logger::get_instance();

    cl::core::LogConfig default_config = {
        .min_level = cl::core::LogLevel::TRACE,
        .queue_size = 2048,
        .colored_output = true,
    };
    logger.init(default_config);

    LOG_ERROR("Error");
    LOG_TRACE("Trace");

    logger.shutdown();
    return 0;
}
