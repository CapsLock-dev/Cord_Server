#include "network/server.hpp"
#include "network/session.hpp"
#include "core/logger.hpp"

namespace cl::network {

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

Server::Server(std::shared_ptr<boost::asio::io_context> ctx, const std::string& cert_path, const std::string& key_path)
	: m_io_ctx{ctx}
    , m_executor{asio::make_strand(*ctx)}
    , m_ssl_ctx{ssl::context{ssl::context::tlsv13}}
    , m_shutdown{false}
{
    setup_ssl(cert_path, key_path);
}

void Server::run(uint16_t port, std::string_view address, unsigned int threads) {
    LOG_INFO("Server is starting with {} threads", threads);
    m_acceptor = std::make_shared<acceptor_type>(m_executor, asio::ip::tcp::endpoint{asio::ip::make_address(address), port});
    asio::co_spawn(m_executor, listener(), asio::detached);
    std::vector<std::thread> v;
    v.reserve(threads-1);
    for(auto i=threads-1; i>0; --i) {
        v.emplace_back([this] { m_io_ctx->run(); });
    }
    m_io_ctx->run();
    for(auto& t : v) {
        t.join();
    }
}

void Server::setup_ssl(const std::string& cert_path, const std::string& key_path) {
	try {
		m_ssl_ctx.set_options(
            ssl::context::default_workarounds |
            ssl::context::no_sslv2 |
            ssl::context::no_sslv3 |
            ssl::context::single_dh_use);
        
        m_ssl_ctx.use_certificate_chain_file(cert_path);
        m_ssl_ctx.use_private_key_file(key_path, ssl::context::pem);
        
        LOG_INFO("SSL enabled with certificate: ", cert_path);
	} catch (const std::exception& e) {
		throw std::runtime_error("Setup SSL failed: " + std::string(e.what()));
	}
}

asio::awaitable<void, executor_type> Server::listener() {  
    auto executor = co_await asio::this_coro::executor;
    while (!m_shutdown) {
        auto socket_executor = asio::make_strand(executor.get_inner_executor());
        auto [ec, socket] = co_await m_acceptor->async_accept(socket_executor, asio::as_tuple);

        if(ec == asio::error::operation_aborted) co_return;
        if(ec) throw boost::system::system_error{ ec };

        LOG_INFO("Connection accepted from {}", socket.remote_endpoint().address().to_v4().to_string());

        auto ws = websocket_stream{std::move(socket), m_ssl_ctx};

        std::shared_ptr<Session> session = std::make_shared<Session>(std::move(ws), socket_executor);
        std::unique_lock lock{m_mtx};
        auto iter = m_sessions.insert(m_sessions.end(), session);

        asio::co_spawn(socket_executor, session->run(), asio::bind_executor(socket_executor, [this, iter](std::exception_ptr ep) {
            std::unique_lock ulock{m_mtx};
            if (ep) {
                try {
                    std::rethrow_exception(ep);
                } catch (const std::exception& e) {
                    LOG_ERROR("Session failed: {}", e.what());
                }
            }
            m_sessions.erase(iter);
            LOG_INFO("Session erased");
        }));
    }
    co_return;
}

void Server::stop() {
    m_io_ctx->stop();
}

} // namespace cl::network
