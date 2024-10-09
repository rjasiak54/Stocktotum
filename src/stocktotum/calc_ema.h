#pragma once

#include <vector>
#include <numeric>

#include "structs.h"
#include "../alphvant/ts_daily_adj.h"
#include "../srctools/enumerate.hpp"

namespace stocktotum {
    namespace calc {

        std::vector<tmstp_dbl>
        ema(std::vector<alphvant::intv::intv_daily_adj> &intvs, int n_days) {

            std::vector<tmstp_dbl> avgs;
            /*
            Calculate initial sma
            */
            double initial_sma = 0.0;
            for (int i = 0; i < n_days && i < intvs.size(); i++) {
                initial_sma += intvs[i].adj_close;
            }
            initial_sma /= n_days;
            tmstp_dbl initial_sma_td{initial_sma, intvs[n_days - 1].timestamp};
            avgs.push_back(initial_sma_td);
            double multiplier = 2.0 / (n_days + 1);
            for (int i = n_days; i < intvs.size(); i++) {
                double e = ((intvs[i].adj_close * multiplier +
                             avgs.back().dbl * (1 - multiplier)));
                tmstp_dbl td{e, intvs[i].timestamp};
                avgs.push_back(td);
            }
            return avgs;
        }
        std::vector<tmstp_dbl> ema(const std::string &symbol, int n_days) {
            alphvant::ts::ts_daily_adj t(symbol);
            return ema(t.intvs, n_days);
        }

        std::vector<tmstp_dbl> ema(alphvant::ts::ts_daily_adj &ts, int n_days) {
            return ema(ts.intvs, n_days);
        }

        crow::json::wvalue ema_bsp(int n_day_watch, int n_day_baseline,
                                   std::string symbol) {

            auto ts = alphvant::ts::ts_daily_adj(symbol);
            auto ema_intvs_watch = ema(ts, n_day_watch);
            auto ema_intvs_baseline = ema(ts, n_day_baseline);

            auto intvs = ts.intvs;

            int baseline_num_points = ema_intvs_baseline.size();
            ema_intvs_watch = calc::align(ema_intvs_watch, intvs);
            ema_intvs_baseline = calc::align(ema_intvs_baseline, intvs);

            //
            // Calculate buy-sell points
            int start = ema_intvs_baseline.size() - baseline_num_points;
            int n = ema_intvs_baseline.size();
            double totalProfit = 0;
            double buyPrice = 0;
            bool bought = false;
            crow::json::wvalue::list buys;
            crow::json::wvalue::list sells;
            for (int i = start; i < n; ++i) {
                // Look for buy signal: 50-day SMA crosses above 200-day SMA
                if (ema_intvs_watch[i].dbl > ema_intvs_baseline[i].dbl &&
                    ema_intvs_watch[i - 1].dbl <=
                        ema_intvs_baseline[i - 1].dbl) {
                    buyPrice = ts.intvs[i].adj_close;
                    bought = true;
                    buys.push_back(ema_intvs_watch[i].tmstp);
                    // std::cout << "Buy at " << buyPrice << " on day " << i
                    //   << std::endl;
                }

                // Look for sell signal: 50-day SMA crosses below 200-day SMA
                if (ema_intvs_watch[i].dbl < ema_intvs_baseline[i].dbl &&
                    ema_intvs_watch[i - 1].dbl >=
                        ema_intvs_baseline[i - 1].dbl &&
                    bought) {
                    double sellPrice = ts.intvs[i].adj_close;
                    double profit = sellPrice - buyPrice;
                    totalProfit += profit;
                    bought = false;
                    sells.push_back(ema_intvs_watch[i].tmstp);
                    // std::cout << "Sell at " << sellPrice << " on day " << i
                    //           << std::endl;
                    // std::cout << "Profit from this trade: " << profit
                    //           << std::endl;
                }
            }

            crow::json::wvalue response;
            crow::json::wvalue::list watch_points;
            crow::json::wvalue::list baseline_points;
            crow::json::wvalue::list adj_close_points;
            for (int i = start - 1; i < n; i++) {
                crow::json::wvalue::list wp;
                wp.push_back(ema_intvs_watch[i].dbl);
                wp.push_back(ema_intvs_watch[i].tmstp);
                watch_points.push_back(wp);
                crow::json::wvalue::list bp;
                bp.push_back(ema_intvs_baseline[i].dbl);
                bp.push_back(ema_intvs_baseline[i].tmstp);
                baseline_points.push_back(bp);
                crow::json::wvalue::list cp;
                cp.push_back(intvs[i].adj_close);
                cp.push_back(intvs[i].timestamp);
                adj_close_points.push_back(cp);
            }
            response["watch_points"] = std::move(watch_points);
            response["baseline_points"] = std::move(baseline_points);
            response["adj_close_points"] = std::move(adj_close_points);
            response["buys"] = std::move(buys);
            response["sells"] = std::move(sells);
            return response;
        }

    } // namespace calc
} // namespace stocktotum