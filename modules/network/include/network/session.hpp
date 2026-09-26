#ifndef CL_NETWORK_SESSION_HPP
#define CL_NETWORK_SESSION_HPP

#include "network/system.hpp"

namespace cl::network {

class Session : std::enable_shared_from_this<Session> {
public:	
	Session(websocket_stream&& ws, executor_type executor);
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    ~Session();
    boost::asio::awaitable<void> run();
private:
    boost::beast::flat_buffer m_read_buffer;
    boost::beast::flat_buffer m_write_buffer;
    websocket_stream m_websocket;
    executor_type m_executor;
    bool m_shutdown;
};

} // namespace cl::network

#endif
