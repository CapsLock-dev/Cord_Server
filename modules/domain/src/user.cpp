#include "domain/user.hpp"

namespace cl::domain {

std::expected<User,DomainError> User::create(const std::string& username, 
                                             const std::string& display_name) 
{
    auto uname = Username::create(username);
    if (!uname) return std::unexpected(uname.error());
    auto dname = Username::create(display_name);
    if (!dname) return std::unexpected(dname.error());
    
    return User(std::move(*uname), std::move(*dname));
}

User::User(Username&& username, Username&& display_name)
    : m_username{std::move(username)}
    , m_display_name{std::move(display_name)} 
{

}

void User::assign_id(uint64_t id) {m_id = id;}
std::optional<uint64_t> User::get_id() const {return m_id;}
const Username& User::get_username() const {return m_username;}
const Username& User::get_display_name() const {return m_display_name;}

}; //namespace cl::domain
