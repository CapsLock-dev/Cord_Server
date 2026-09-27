#include "core/logger.hpp"
#include "network/server.hpp"

#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

int main(int argc, char** argv) {
	po::options_description desc("Options");
	desc.add_options()
    	("help,h", "Display help message")
    	("address,a", po::value<std::string>(), "Set server address")
    	("port,p", po::value<unsigned short>(), "Set server port")
    	("cert,c", po::value<std::string>(), "Set SSL Certificate path")
    	("key,k", po::value<std::string>(), "Set SSL Certificate key path")
	;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

	if (!vm.count("cert") || !vm.count("key")) {
        std::cout << "Both SSL certificate and key must be provided.\n";
        std::cout << desc << "\n";
        return 1;
    }

    auto& logger = cl::core::Logger::get_instance();

    cl::core::LogConfig default_config = {
        .min_level = cl::core::LogLevel::TRACE,
        .queue_size = 2048,
        .colored_output = true,
    };
    logger.init(default_config);
    try {
		uint16_t port = vm.count("port") ? vm["port"].as<unsigned short>() : 9191;
        std::string address = vm.count("address") ? vm["address"].as<std::string>() : "0.0.0.0";
		std::string cert_path = vm["cert"].as<std::string>();
		std::string key_path = vm["key"].as<std::string>();

        unsigned int threads = std::thread::hardware_concurrency();

        auto io_ctx = std::make_shared<boost::asio::io_context>(threads);
        cl::network::Server server{io_ctx, cert_path, key_path};

        boost::asio::signal_set signals(*io_ctx, SIGINT, SIGTERM);
        signals.async_wait([&server](boost::beast::error_code const&, int) {
            server.stop();
        });
        server.run(port, address, threads);
    } catch (const std::exception& e) {
        LOG_CRITICAL("Error in main: {}", e.what());
    }

    logger.shutdown();
    return 0;
}
