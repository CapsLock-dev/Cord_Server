#ifndef CL_CONNECTION_POOL_HPP
#define CL_CONNECTION_POOL_HPP

#include <condition_variable>
#include <memory>
#include <mutex>
#include <pqxx/connection>
#include <vector>

namespace cl::database {

class ConnectionPool {
    friend class ScopedConnection;

    public:
    ConnectionPool(std::string_view conn_str, std::size_t pool_size);
    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;

    void prepare_statements(
        const std::vector<std::pair<std::string_view, std::string_view>>&
            statements);

    private:
    std::shared_ptr<pqxx::connection> get_connection();
    void return_connection(std::shared_ptr<pqxx::connection> conn);

    std::vector<std::shared_ptr<pqxx::connection>> m_connections;
    std::vector<std::shared_ptr<pqxx::connection>> m_available;
    std::condition_variable m_cond;
    std::mutex m_mutex;
};

class ScopedConnection {
    public:
    explicit ScopedConnection(ConnectionPool& pool)
        : m_pool(pool), m_conn(pool.get_connection()) {}
    ~ScopedConnection() { m_pool.return_connection(std::move(m_conn)); }

    ScopedConnection(const ScopedConnection&) = delete;
    ScopedConnection& operator=(const ScopedConnection&) = delete;

    pqxx::connection& get() { return *m_conn; };
    pqxx::connection& operator*() { return *m_conn; }

    private:
    ConnectionPool& m_pool;
    std::shared_ptr<pqxx::connection> m_conn;
};

}  // namespace cl::database

#endif
