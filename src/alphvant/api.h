#pragma once

#include <string>

namespace alphvant {

    namespace api {

        const std::string domain = "https://www.alphavantage.co/";
        std::string GLOB_KEY = "demo";
        std::string DATA_PATH = "avdata";

        void set_key(std::string k) {
            GLOB_KEY = k;
        }
    } // namespace api

} // namespace alphvant