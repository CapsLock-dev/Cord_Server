#ifndef CL_DOMAIN_USER_HPP
#define CL_DOMAIN_USER_HPP

#include "domain/domain_error.hpp"
#include "domain/username.hpp"
#include <expected>
#include <optional>
#include <cstdint>

namespace cl::domain {
class User{
public:
    static std::expected<User,DomainError> create(const std::string& username, 
                                                      const std::string& display_name);
    void assign_id(uint64_t id);
    std::optional<uint64_t> get_id() const;
    const Username& get_username() const;
    const Username& get_display_name() const;
private:
    User(Username&& username, Username&& display_name);
    std::optional<uint64_t> m_id;
    Username m_username;
    Username m_display_name;
};

}; //namespace cl::domain

#endif
