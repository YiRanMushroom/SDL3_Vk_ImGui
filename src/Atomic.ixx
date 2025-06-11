export module Atomic;

import std;

export template<typename T>
class Atomic {
public:
    Atomic() = default;
    Atomic(const T& value) = delete;
    Atomic(T&& value) = delete;
    Atomic(const Atomic&) = delete;
    Atomic(Atomic&&) = delete;

    struct Proxy {
        Atomic& m_Atomic;
        Proxy(Atomic& atomic) : m_Atomic(atomic) {
            m_Atomic.m_Mutex.lock();
        }

        T& operator*() {
            return m_Atomic.m_Value;
        }

        ~Proxy() {
            m_Atomic.m_Mutex.unlock();
        }

        void Set(const T& value) {
            m_Atomic.m_Value = value;
        }

        T& Get() {
            return m_Atomic.m_Value;
        }
    };

    Proxy Proxy() {
        return Proxy(*this);
    }

private:
    std::mutex m_Mutex{};
    T m_Value;
};