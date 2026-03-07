#include <core/logger.hpp>

int main() {
    auto& logger = cl::core::Logger::get_instance();

    cl::core::LogConfig default_config = {
        .min_level = cl::core::LogLevel::INFO,
        .queue_size = 2048,
        .colored_output = true,
    };
    logger.init(default_config);
    logger.log(cl::core::LogLevel::INFO, "Test");

    logger.shutdown();
    return 0;
}
