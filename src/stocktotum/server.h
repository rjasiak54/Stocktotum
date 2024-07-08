#pragma once

#include <vector>

#include <crow.h>

#include "views.h"
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
    ([]() { return stocktotum::view::load_public_view_file("index.html"); });

    CROW_ROUTE(app, "/sma/<int>/<string>")
    ([](int n_day, std::string symbol) {
        return stocktotum::view::sma(symbol, n_day);
    });
}