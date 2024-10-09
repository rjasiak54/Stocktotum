#pragma once

#include <vector>
#include <map>

#include "structs.h"
#include "../alphvant/ts_daily_adj.h"
#include "../srctools/enumerate.hpp"

namespace stocktotum {
    namespace calc {

        std::vector<tmstp_dbl>
        sma(std::vector<alphvant::intv::intv_daily_adj> &intvs, int n_days) {

            std::vector<tmstp_dbl> avgs;
            double sum = 0;
            int num_days_summed = 0;
            int skip_count = 0;
            int empty_count = 0;
            for (auto ii : srctools::enumerate(intvs)) {
                if (ii.item.timestamp == "") {
                    skip_count++;
                    auto to_remove = intvs[ii.index - (n_days - 1)];
                    //
                    // "Pop" last item in "queue"
                    if (to_remove.timestamp != "") {
                        num_days_summed--;
                        sum -= to_remove.adj_close;
                    }
                    continue;
                }
                auto item = ii.item;
                sum += item.adj_close;
                num_days_summed++;
                if (ii.index >= n_days) {
                    avgs.push_back(
                        tmstp_dbl{sum / num_days_summed, item.timestamp});
                    auto to_remove = intvs[ii.index - (n_days - 1)];
                    //
                    // "Pop" last item in "queue"
                    if (to_remove.timestamp != "") {
                        num_days_summed--;
                        sum -= to_remove.adj_close;
                    }
                }
            }
            return avgs;
        }
        std::vector<tmstp_dbl> sma(const std::string &symbol, int n_days) {
            alphvant::ts::ts_daily_adj t(symbol);
            return sma(t.intvs, n_days);
        }

        std::vector<tmstp_dbl> sma(alphvant::ts::ts_daily_adj &ts, int n_days) {
            return sma(ts.intvs, n_days);
        }
        std::vector<tmstp_dbl>
        align(const std::vector<tmstp_dbl> &dataset_short,
              const std::vector<alphvant::intv::intv_daily_adj> &dataset_long) {
            std::vector<tmstp_dbl> new_short;

            for (const auto &l : dataset_long) {
                if (dataset_short[0].tmstp == l.timestamp) {
                    break;
                }
                new_short.push_back(tmstp_dbl{-1, ""});
            }

            for (const auto &s : dataset_short) {
                new_short.push_back(s);
            }
            return new_short;
        }
        std::vector<tmstp_dbl>
        align(const std::vector<tmstp_dbl> &dataset_short,
              const std::vector<tmstp_dbl> &dataset_long) {
            std::map<std::string, tmstp_dbl> std::vector<tmstp_dbl> new_short;

            for (const auto &l : dataset_long) {
                if (dataset_short[0].tmstp == l.tmstp) {
                    break;
                }
                new_short.push_back(tmstp_dbl{-1, ""});
            }

            for (const auto &s : dataset_short) {
                new_short.push_back(s);
            }
            return new_short;
        }
        std::vector<tmstp_dbl>
        align_to_short(const std::vector<tmstp_dbl> &dataset_long,
                       const std::vector<tmstp_dbl> &dataset_short) {
            std::vector<tmstp_dbl> new_short;

            for (const auto &l : dataset_long) {
                if (dataset_short[0].tmstp == l.tmstp) {
                    break;
                }
                new_short.push_back(tmstp_dbl{-1, ""});
            }

            for (const auto &s : dataset_short) {
                new_short.push_back(s);
            }
            return new_short;
        }

        crow::json::wvalue sma_bsp(int n_day_watch, int n_day_baseline,
                                   std::string symbol) {

            auto ts = alphvant::ts::ts_daily_adj(symbol);
            auto sma_intvs_watch = sma(ts, n_day_watch);
            auto sma_intvs_baseline = sma(ts, n_day_baseline);

            auto intvs = ts.intvs;

            int baseline_num_points = sma_intvs_baseline.size();
            sma_intvs_watch = calc::align(sma_intvs_watch, intvs);
            sma_intvs_baseline = calc::align(sma_intvs_baseline, intvs);

            //
            // Calculate buy-sell points
            int start = sma_intvs_baseline.size() - baseline_num_points;
            int n = sma_intvs_baseline.size();
            double totalProfit = 0;
            double buyPrice = 0;
            bool bought = false;
            crow::json::wvalue::list buys;
            crow::json::wvalue::list sells;
            for (int i = start; i < n; ++i) {
                // Look for buy signal: 50-day SMA crosses above 200-day SMA
                if (sma_intvs_watch[i].dbl > sma_intvs_baseline[i].dbl &&
                    sma_intvs_watch[i - 1].dbl <=
                        sma_intvs_baseline[i - 1].dbl) {
                    buyPrice = ts.intvs[i].adj_close;
                    bought = true;
                    buys.push_back(sma_intvs_watch[i].tmstp);
                    std::cout << "Buy at " << buyPrice << " on day " << i
                              << std::endl;
                }

                // Look for sell signal: 50-day SMA crosses below 200-day SMA
                if (sma_intvs_watch[i].dbl < sma_intvs_baseline[i].dbl &&
                    sma_intvs_watch[i - 1].dbl >=
                        sma_intvs_baseline[i - 1].dbl &&
                    bought) {
                    double sellPrice = ts.intvs[i].adj_close;
                    double profit = sellPrice - buyPrice;
                    totalProfit += profit;
                    bought = false;
                    sells.push_back(sma_intvs_watch[i].tmstp);
                    std::cout << "Sell at " << sellPrice << " on day " << i
                              << std::endl;
                    std::cout << "Profit from this trade: " << profit
                              << std::endl;
                }
            }

            crow::json::wvalue response;
            crow::json::wvalue::list watch_points;
            crow::json::wvalue::list baseline_points;
            crow::json::wvalue::list adj_close_points;
            for (int i = start - 1; i < n; i++) {
                crow::json::wvalue::list wp;
                wp.push_back(sma_intvs_watch[i].dbl);
                wp.push_back(sma_intvs_watch[i].tmstp);
                watch_points.push_back(wp);
                crow::json::wvalue::list bp;
                bp.push_back(sma_intvs_baseline[i].dbl);
                bp.push_back(sma_intvs_baseline[i].tmstp);
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