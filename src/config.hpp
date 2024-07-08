#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <toml++/toml.hpp>
#include <string>
#include <filesystem>

namespace sconfig {

    std::string _get_env_var_or_default(const std::string &variable_name,
                                        const std::string &default_value);

    extern std::string STOCKTOTUM_ENV;

    extern std::filesystem::path TOP_DIR;
    extern std::filesystem::path CICD_PATH;
    extern std::filesystem::path CONFIG_PATH;
    extern std::filesystem::path DATA_PATH;
    extern std::filesystem::path TS_DAILY_PATH;
    extern std::filesystem::path TS_DAILY_ADJ_PATH;

    extern std::filesystem::path PUBLIC_PATH;

    extern toml::v3::ex::parse_result env;
    extern toml::v3::ex::parse_result crow;

} // namespace sconfig

#endif