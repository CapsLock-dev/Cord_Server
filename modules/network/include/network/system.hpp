#ifndef CL_NETWORK_SYSTEM_HPP
#define CL_NETWORK_SYSTEM_HPP

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ssl.hpp>

namespace cl::network {

using executor_type = boost::asio::strand<boost::asio::io_context::executor_type>;
using acceptor_type = typename boost::asio::ip::tcp::acceptor::rebind_executor<executor_type>::other;
using websocket_stream = boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>>;

}; //namespace cl::network;

#endif
