#ifndef CL_NETWORK_TASK_GROUP_HPP
#define CL_NETWORK_TASK_GROUP_HPP

#include "network/system.hpp"
#include <list>
#include <mutex>

namespace cl::network {

class TaskGroup {
public:
    TaskGroup(boost::asio::any_io_executor executor);
    TaskGroup(TaskGroup const& ) = delete;
    TaskGroup(TaskGroup&&) = delete;
    template<typename CompletionToken>
    auto adapt(CompletionToken&& completion_token) {
        std::unique_lock lock{m_mtx};
        auto new_signal = m_signal_list.emplace(m_signal_list.end());

        class remover{
            public:
                remover(TaskGroup* tg, decltype(new_signal) cs): m_tg{tg}, m_cs{cs} {};
                remover(remover&& other) noexcept : m_tg{std::exchange(other.m_tg, nullptr)}, m_cs{ other.m_cs}{}
                ~remover() {
                    if (m_tg) {
                        std::unique_lock lock{m_tg->m_mtx};
                        if (m_tg->m_signal_list.erase(m_cs) == m_tg->m_signal_list.end()) {
                            m_tg->m_cv.cancel();
                        }
                    }
                }
            private:
                TaskGroup* m_tg;
                decltype(new_signal) m_cs;
        };
        
        return boost::asio::bind_cancellation_slot(
            new_signal->slot(),
            boost::asio::consign(completion_token, remover{this, new_signal})
        );
    }
    void emit(boost::asio::cancellation_type type) {
        std::unique_lock lock{m_mtx};
        for (auto& cs : m_signal_list) {
            cs.emit(type);
        }
    }
    boost::asio::awaitable<void> async_wait() {
        co_await m_cv.async_wait();
        co_return;
    }
private:
    std::mutex m_mtx;
    boost::asio::steady_timer m_cv;
    std::list<boost::asio::cancellation_signal> m_signal_list;
};

}; //namespace cl::network;

#endif
