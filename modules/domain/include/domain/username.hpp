#ifndef CL_DOMAIN_USERNAME_HPP
#define CL_DOMAIN_USERNAME_HPP

#include "domain/domain_error.hpp"
#include <expected>

namespace cl::domain {

class Username{
public:
    static std::expected<Username, DomainError> create(const std::string& username) {
        if (username.size() > 32) {
            return std::unexpected(
                DomainError{.ec=DomainErrorCode::UsernameTooLong, .detail=""}
            );
        } 
        if (username.size() == 0) {
            return std::unexpected(
                DomainError{.ec=DomainErrorCode::UsernameTooShort, .detail=""}
            );
        }
        return Username(username);
    }
    Username(Username&& u) {
        m_username = {u.m_username};
    }
    Username(Username&) = delete;
    std::string get_username() const {return m_username;}
protected:
    Username(const std::string& username): m_username(std::move(username)){};
    std::string m_username; 
};

}; //namespace cl::domain

#endif
