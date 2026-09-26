#ifndef CL_NETWORK_SERVER_HPP
#define CL_NETWORK_SERVER_HPP

#include "network/system.hpp"
#include <set>

namespace cl::network {
class Session;
class Server {
public:	
	Server(std::shared_ptr<boost::asio::io_context> ctx, const std::string& cert_path, const std::string& key_path);
    void run(uint16_t port, std::string_view address, unsigned int threads);
    void stop();
private:
    void setup_ssl(const std::string& cert_path, const std::string& key_path);
	boost::asio::awaitable<void, executor_type> listener();
	std::shared_ptr<boost::asio::io_context> m_io_ctx;
    std::shared_ptr<acceptor_type> m_acceptor;
    executor_type m_executor;
    std::set<std::shared_ptr<Session>> m_sessions;
    boost::asio::ssl::context m_ssl_ctx;
    std::mutex m_mtx;
    bool m_shutdown;
};

} // namespace cl::network

#endif
