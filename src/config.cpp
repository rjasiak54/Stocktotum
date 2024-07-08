#include "config.hpp"

#include <stdlib.h>
#include <filesystem>

#include <toml++/toml.hpp>

namespace sconfig {

    std::string _get_env_var_or_default(const std::string &variable_name,
                                        const std::string &default_value) {
        const char *value = getenv(variable_name.c_str());
        return value ? value : default_value;
    }

    using __fs_path = std::filesystem::path;

    std::string STOCKTOTUM_ENV =
        _get_env_var_or_default("STOCKTOTUM_ENV", "local");

    __fs_path _TOP_DIR = __fs_path("../../../");
    __fs_path TOP_DIR = _TOP_DIR;

    __fs_path _CICD_PATH =
        _TOP_DIR / __fs_path("cicd") / __fs_path(STOCKTOTUM_ENV);
    __fs_path CICD_PATH = _CICD_PATH;
    __fs_path _CONFIG_PATH = _TOP_DIR / __fs_path("config");
    __fs_path CONFIG_PATH = _CONFIG_PATH;
    __fs_path _DATA_PATH = _TOP_DIR / __fs_path("avdata");
    __fs_path DATA_PATH = _DATA_PATH;
    __fs_path _TS_DAILY_PATH = _DATA_PATH / __fs_path("ts-daily");
    __fs_path TS_DAILY_PATH = _TS_DAILY_PATH;
    __fs_path _TS_DAILY_ADJ_PATH = _DATA_PATH / __fs_path("ts-daily-adj");
    __fs_path TS_DAILY_ADJ_PATH = _TS_DAILY_ADJ_PATH;

    __fs_path _PUBLIC_PATH = _TOP_DIR / __fs_path("public");
    __fs_path PUBLIC_PATH = _PUBLIC_PATH;

    __fs_path _env_file = _CICD_PATH / __fs_path("env.toml");
    __fs_path _crow_file = _CONFIG_PATH / __fs_path("crow.toml");

    toml::v3::ex::parse_result env = toml::parse_file(_env_file.string());
    toml::v3::ex::parse_result crow = toml::parse_file(_crow_file.string());

} // namespace sconfig
