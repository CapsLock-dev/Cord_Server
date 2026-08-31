#ifndef CL_DATABASE_HPP
#define CL_DATABASE_HPP

#include <expected>
#include <pqxx/transaction>
#include <vector>

#include "conn_pool.hpp"
#include "core/logger.hpp"
#include "database/database_error.hpp"

namespace cl::database {

template <typename F>
concept ValidTransaction = std::invocable<F, pqxx::work&>;

class Database {
    public:
    Database(std::string_view conn_str, std::size_t conn_pool_size);
    void prepare_statements(
        const std::vector<std::pair<std::string_view, std::string_view>>&
            statements);

    template <ValidTransaction F>
    auto transaction(F&& func)
        -> std::expected<std::invoke_result_t<F, pqxx::work&>, DatabaseError> {
        DatabaseError error = {};
        ScopedConnection conn(*m_pool);
        try {
            pqxx::work tx(*conn);
            auto res = func(tx);
            return res;
        } catch (const pqxx::unique_violation& e) {
            error.detail = e.what();
            error.ec = DatabaseErrorCode::ConstraintViolation;
        } catch (const pqxx::unexpected_rows& e) {
            error.detail = e.what();
            error.ec = DatabaseErrorCode::NoRows;
        } catch (const pqxx::sql_error& e) {
            LOG_ERROR("[DB] sql error: {}", e.what());
            error.detail = e.what();
            error.ec = DatabaseErrorCode::QueryFailed;
        } catch (const std::exception& e) {
            LOG_ERROR("[DB] transaction error: {}", e.what());
            error.detail = e.what();
            error.ec = DatabaseErrorCode::TransactionFailed;
        }
        return std::unexpected(error);
    }

    private:
    std::unique_ptr<ConnectionPool> m_pool;
};

}  // namespace cl::database

#endif
