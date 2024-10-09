#pragma once

#include <vector>

#include <crow.h>

#include "views.h"
#include "calc_ema.h"
#include "calc_sma.h"
#include "../config.hpp"

void init_routes(crow::SimpleApp &app);

namespace stocktotum {

    class server : public crow::SimpleApp {

      public:
        server() : crow::SimpleApp() {
            init_routes(*this);
        }
    };

} // namespace stocktotum

void init_routes(crow::SimpleApp &app) {

    CROW_ROUTE(app, "/")
    ([](const crow::request &req) {
        return stocktotum::view::load_public_view_file("index.html");
    });

    CROW_ROUTE(app, "/sma")
    ([]() { return stocktotum::view::load_public_view_file("sma.html"); });
    CROW_ROUTE(app, "/ema")
    ([]() { return stocktotum::view::load_public_view_file("ema.html"); });

    CROW_ROUTE(app, "/lapi/sma/<int>/<int>/<str>")
    ([](int n_day_watch, int n_day_baseline, std::string symbol) {
        return stocktotum::calc::sma_bsp(n_day_watch, n_day_baseline, symbol);
    });
    CROW_ROUTE(app, "/lapi/ema/<int>/<int>/<str>")
    ([](int n_day_watch, int n_day_baseline, std::string symbol) {
        return stocktotum::calc::ema_bsp(n_day_watch, n_day_baseline, symbol);
    });
}