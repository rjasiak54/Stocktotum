#ifndef CROW_LOG_CUSTOM
#define CROW_LOG_CUSTOM

namespace srctools {
#define SCROW_LOG_INFO(message)                                                \
    CROW_LOG_INFO << "["                                                       \
                  << std::filesystem::path(__FILE__).filename().string()       \
                  << "." << __func__ << "(...) - line: " << __LINE__ << "] "   \
                  << message
#define SCROW_LOG_WARNING(message)                                             \
    CROW_LOG_WARNING << "["                                                    \
                     << std::filesystem::path(__FILE__).filename().string()    \
                     << "." << __func__ << "(...) - line: " << __LINE__        \
                     << "] " << message
#define SCROW_LOG_ERROR(message)                                               \
    CROW_LOG_ERROR << "["                                                      \
                   << std::filesystem::path(__FILE__).filename().string()      \
                   << "." << __func__ << "(...) - line: " << __LINE__ << "] "  \
                   << message
} // namespace srctools
#endif