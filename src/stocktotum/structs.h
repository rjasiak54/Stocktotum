#pragma once

#include <crow.h>

namespace stocktotum {

    struct tmstp_dbl {
        double dbl;
        std::string tmstp;
        crow::json::wvalue to_json(std::string dbl_key = "val",
                                   std::string tmstp_key = "timestamp") const {
            crow::json::wvalue j;
            j[dbl_key] = dbl;
            j[tmstp_key] = tmstp;
            return j;
        }
    };
} // namespace stocktotum