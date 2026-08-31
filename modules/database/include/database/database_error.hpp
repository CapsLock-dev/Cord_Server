#ifndef CL_DATABASE_ERROR_HPP
#define CL_DATABASE_ERROR_HPP

#include <source_location>
#include <string>

namespace cl::database {

enum class DatabaseErrorCode {
    Undefined,
    ConnectionFailed,
    QueryFailed,
    ConstraintViolation,
    NoRows,
    TransactionFailed,
    Ok,
};

struct DatabaseError {
    DatabaseErrorCode ec;
    std::source_location loc = std::source_location::current();
    std::string detail;
};

}  // namespace cl::database

#endif
