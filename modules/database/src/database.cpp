#include "database/database.hpp"

namespace cl::database {

Database::Database(std::string_view conn_str, std::size_t conn_pool_size)
    : m_pool(std::make_unique<ConnectionPool>(conn_str, conn_pool_size)) {}

void Database::prepare_statements(
    const std::vector<std::pair<std::string_view, std::string_view>>&
        statements) {
    m_pool->prepare_statements(statements);
}

}  // namespace cl::database
