#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "smip_export.h"

namespace smip::log {

// clang-format off
enum Level : std::uint8_t {
    Shutdown    = 0b0000'0001,
    Emergency   = Shutdown | 0b0000'0010,
    Alert       = Shutdown | 0b0000'0100,
    Critical    = Shutdown | 0b0000'0110,
    Error       = 0b0001'0000,
    Warning     = 0b0010'0000,
    Notice      = 0b0011'0000,
    Info        = 0b0100'0000,
    Debug       = 0b1000'0000
};
//clang-format on

/**
 * @brief The system class
 * Initialisation and logging system configuration
 */
class SMIP_EXPORT system {
public:
    /**
     * @brief
     * @param l Maximum Log level to show
     */
    static void setup(
        Level l,
        std::function<void(int)> callback = [](int c) { exit(c); },
        std::ostream& str = std::cerr);

    system(Level l, std::function<void(int)> cb, std::ostream& str);

    [[nodiscard]] static auto level() -> Level&;
    [[nodiscard]] static auto stream() -> std::ostream&;
    static void callback(int exit_code);

private:
    static std::unique_ptr<system> s_singleton;

    Level m_level {};
    std::function<void(int)> m_callback {};
    std::ostream& m_stream;
};

template <Level L>
class SMIP_EXPORT logger {
public:
    template <typename T>
    auto operator<<(T content) -> logger<L>&
    {
        m_stream << content;
        return *this;
    }

    logger(const std::string& component, int exit_code = 0)
        : m_exit_code { exit_code }
    {
        m_stream << to_string();
        if (!component.empty()) {
            m_stream << " (" << component << ')';
        }
        if (m_stream.tellp() > 0) {
            m_stream << ": ";
        }
    }

    logger() = default;

    ~logger()
    {
        if (/*((L & Level::Info) > 0) ||*/ (L <= system::level())) {
            m_stream << '\n';
            system::stream() << m_stream.rdbuf() << std::flush;
        }
        if constexpr ((L & Level::Shutdown) > 0) {
            system::callback(m_exit_code);
        }
    }

private:
    std::stringstream m_stream {};
    int m_exit_code { 0 };

    [[nodiscard]] constexpr static auto to_string() -> const char*
    {
        switch (L) {
        case Level::Debug:
            return "Debug";
        case Level::Info:
            return "";
        case Level::Notice:
            return "Notice";
        case Level::Warning:
            return "Warning";
        case Level::Error:
            return "Error";
        case Level::Critical:
            return "Critical";
        case Level::Alert:
            return "Alert";
        case Level::Emergency:
            return "Emergency";
        }
        return "";
    }
};

[[nodiscard]] auto SMIP_EXPORT debug(const std::string& component = {})
    -> logger<Level::Debug>;
[[nodiscard]] auto SMIP_EXPORT info(const std::string& component = {}) -> logger<Level::Info>;
[[nodiscard]] auto SMIP_EXPORT notice(const std::string& component = {})
    -> logger<Level::Notice>;
[[nodiscard]] auto SMIP_EXPORT warning(const std::string& component = {})
    -> logger<Level::Warning>;
[[nodiscard]] auto SMIP_EXPORT error(const std::string& component = {})
    -> logger<Level::Error>;
[[nodiscard]] auto SMIP_EXPORT critical(int exit_code = 1, const std::string& component = {})
    -> logger<Level::Critical>;
[[nodiscard]] auto SMIP_EXPORT alert(int exit_code = 1, const std::string& component = {})
    -> logger<Level::Alert>;
[[nodiscard]] auto SMIP_EXPORT emergency(int exit_code = 1, const std::string& component = {})
    -> logger<Level::Emergency>;

} // namespace smip::log
