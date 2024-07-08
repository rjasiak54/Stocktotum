#pragma once

#include <crow.h>

#include "calc_sma.h"
#include "../config.hpp"

namespace stocktotum {

    namespace view {

        std::string load_public_view_file(const std::string &pub_fname);

        crow::mustache::rendered_template sma(std::string &symbol, int n_days) {

            auto v = calc::sma(symbol, n_days);

            std::vector<crow::json::wvalue> timestamps;
            crow::json::wvalue::list avg_list;

            for (const auto &i : v) {
                timestamps.push_back(i.tmstp);
                avg_list.push_back(i.dbl);
            }

            crow::mustache::context ctx;

            ctx["symbol"] = symbol;
            ctx["n_day"] = n_days;
            ctx["timestamps"] = std::move(timestamps);
            ctx["avgs"] = std::move(avg_list);

            std::string f = load_public_view_file("moving-average.stache");
            auto page = crow::mustache::compile(f).render(ctx);
            return page;
        }

        std::string load_public_view_file(const std::string &pub_fname) {
            std::filesystem::path p =
                sconfig::PUBLIC_PATH / std::filesystem::path(pub_fname);
            std::ifstream file(p);
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }
    } // namespace view
} // namespace stocktotum