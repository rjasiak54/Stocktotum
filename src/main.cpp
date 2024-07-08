#include <stdio.h>
#include <iostream>
#include <format>

#include <boost/json.hpp>

#include "alphvant/api.h"
#include "stocktotum/server.h"
#include "config.hpp"

int main(int argc, char **argv) {

    stocktotum::server srv;
    int port = sconfig::crow["port"].value_or<int>(5038);
    std::string host = sconfig::crow["host"].value_or<std::string>("0.0.0.0");
    int num_threads = sconfig::crow["num_threads"].value_or<int>(4);

    std::string_view api_key =
        sconfig::env["alpha_vantage"]["key"].value_or("demo");
    alphvant::api::set_key(std::string(api_key));

    srv.concurrency(num_threads);
    srv.port(port).multithreaded().run();

    return 0;
}
