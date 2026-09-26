#include "network/session.hpp"
#include "core/logger.hpp"

namespace cl::network {

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

Session::Session(websocket_stream&& ws, executor_type executor)
: m_websocket{std::move(ws)}
, m_executor{executor}
, m_shutdown{false}
{

}

Session::~Session() {

}

asio::awaitable<void> Session::run() {
	try {
		co_await m_websocket.next_layer().async_handshake(asio::ssl::stream_base::server, asio::use_awaitable);

		m_websocket.set_option(beast::websocket::stream_base::timeout::suggested(beast::role_type::server));
		m_websocket.set_option(beast::websocket::stream_base::decorator(
			[](beast::websocket::response_type& res) {
				res.set(beast::http::field::server,
						std::string(BOOST_BEAST_VERSION_STRING) + " Cord-Server");
			}
		));
		co_await m_websocket.async_accept(asio::use_awaitable);

        while(!m_shutdown) {
            auto [ec, _] = co_await m_websocket.async_read(m_read_buffer, asio::as_tuple);
            if (ec == beast::websocket::error::closed) co_return;
            if (ec)
                throw boost::system::system_error{ec};
            if (m_shutdown) break;
            m_websocket.text(m_websocket.got_text());
            co_await m_websocket.async_write(m_read_buffer.data());
            m_read_buffer.consume(m_read_buffer.size());
        }

	} catch (const beast::system_error& e) {
		if (e.code() == beast::websocket::error::closed ||
            e.code() == asio::error::eof ||
            e.code() == asio::ssl::error::stream_truncated ||
            e.code() == asio::error::connection_reset) {
            
            LOG_DEBUG("Client disconnected: {}", e.code().message());
        } else {
            LOG_ERROR("Error in SSL session: {} [{}]", e.what(), e.code().message());
        }
	}	
	LOG_INFO("SSL connection closed");
    co_return;
};

} // namespace cl::network
