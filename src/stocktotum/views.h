#pragma once

#include <crow.h>

#include "calc_sma.h"
#include "../alphvant/ts_daily_adj.h"
#include "../config.hpp"

namespace stocktotum {

    namespace view {

        std::string load_public_view_file(const std::string &pub_fname);

        crow::json::wvalue sma(std::string &symbol, int n_days) {

            auto ts = alphvant::ts::ts_daily_adj(symbol);
            CROW_LOG_INFO << "got data for symbol " << symbol;
            auto sma_intvs = calc::sma(ts, n_days);
            auto sma_intvs2 = calc::sma(ts, 200);
            auto intvs = ts.intvs;

            int sma_intvs2_num_points = sma_intvs2.size();
            sma_intvs = calc::align(sma_intvs, intvs);
            sma_intvs2 = calc::align(sma_intvs2, sma_intvs);

            //
            // Calculate buy-sell points
            int start = sma_intvs2.size() - sma_intvs2_num_points;
            int n = sma_intvs2.size();
            double totalProfit = 0;
            double buyPrice = 0;
            bool bought = false;

            crow::json::wvalue::list buy_timestamps;
            crow::json::wvalue::list sell_timestamps;
            for (int i = 1; i < n; ++i) {
                // Look for buy signal: 50-day SMA crosses above 200-day SMA
                if (sma_intvs[i].dbl > sma_intvs2[i].dbl &&
                    sma_intvs[i - 1].dbl <= sma_intvs2[i - 1].dbl) {
                    buyPrice = ts.intvs[i].adj_close;
                    bought = true;
                    std::cout << "Buy at " << buyPrice << " on day " << i
                              << std::endl;
                }

                // Look for sell signal: 50-day SMA crosses below 200-day SMA
                if (sma_intvs[i].dbl < sma_intvs2[i].dbl &&
                    sma_intvs[i - 1].dbl >= sma_intvs2[i - 1].dbl && bought) {
                    double sellPrice = ts.intvs[i].adj_close;
                    double profit = sellPrice - buyPrice;
                    totalProfit += profit;
                    bought = false;
                    std::cout << "Sell at " << sellPrice << " on day " << i
                              << std::endl;
                    std::cout << "Profit from this trade: " << profit
                              << std::endl;
                }
            }

            std::vector<crow::json::wvalue> timestamps;
            crow::json::wvalue::list intvs_list;
            crow::json::wvalue::list sma_intvs_list;
            crow::json::wvalue::list sma_intvs2_list;

            for (const auto &i : intvs) {
                timestamps.push_back(i.timestamp);
                intvs_list.push_back(i.adj_close);
            }
            for (const auto &i : sma_intvs) {
                sma_intvs_list.push_back(i.dbl);
            }
            for (const auto &i : sma_intvs2) {
                sma_intvs2_list.push_back(i.dbl);
            }
            crow::json::wvalue response;

            return response;
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