#include "database/conn_pool.hpp"

#include "core/logger.hpp"

namespace cl::database {

ConnectionPool::ConnectionPool(std::string_view conn_str,
                               std::size_t pool_size) {
    std::unique_lock lock(m_mutex);
    m_connections.reserve(pool_size);
    m_available.reserve(pool_size);

    for (std::size_t i = 0; i < pool_size; ++i) {
        auto conn = std::make_shared<pqxx::connection>(std::string(conn_str));
        m_connections.emplace_back(conn);
        m_available.emplace_back(conn);
    }

    LOG_INFO("[DB] ConnectionPool created with {} connections", pool_size);
}

void ConnectionPool::prepare_statements(
    const std::vector<std::pair<std::string_view, std::string_view>>&
        statements) {
    std::unique_lock lock(m_mutex);
    for (auto& statement : statements) {
        std::string first = std::string(statement.first);
        std::string second = std::string(statement.second);
        for (auto& conn : m_connections) {
            conn->prepare(first, second);
        }
    }
}

std::shared_ptr<pqxx::connection> ConnectionPool::get_connection() {
    std::unique_lock lock(m_mutex);
    m_cond.wait(lock, [this]() { return !m_available.empty(); });
    auto c = std::move(m_available.back());
    m_available.pop_back();
    return c;
}

void ConnectionPool::return_connection(std::shared_ptr<pqxx::connection> conn) {
    {
        std::unique_lock lock(m_mutex);
        m_available.push_back(std::move(conn));
    }
    m_cond.notify_one();
}

}  // namespace cl::database
