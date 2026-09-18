#ifndef CL_DOMAIN_ERROR_HPP
#define CL_DOMAIN_ERROR_HPP

#include <source_location>
#include <string>

namespace cl::domain {

enum class DomainErrorCode {
    Undefined,
    UsernameTooLong,
    UsernameTooShort,
    Ok,
};

struct DomainError {
    DomainErrorCode ec;
    std::source_location loc = std::source_location::current();
    std::string detail;
};

}  // namespace cl::domain

#endif
