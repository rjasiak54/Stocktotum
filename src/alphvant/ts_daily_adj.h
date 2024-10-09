#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <iostream>

#include <cpr/cpr.h>

#include "log.h"
#include "api.h"

#include "../config.hpp"

namespace alphvant {

    namespace intv {

        struct intv_daily_adj {
            std::string timestamp;
            double open;
            double high;
            double low;
            double close;
            double adj_close;
            unsigned long volume;
            double div_amt;
            double split_coef;

            /*
            Construct from a csv row
            */
            intv_daily_adj(std::string line) {
                std::istringstream lineStream(line);
                std::string col;

                if (std::getline(lineStream, col, ',')) {
                    timestamp = col;
                }
                if (std::getline(lineStream, col, ',')) {
                    open = std::stod(col);
                }
                if (std::getline(lineStream, col, ',')) {
                    high = std::stod(col);
                }
                if (std::getline(lineStream, col, ',')) {
                    low = std::stod(col);
                }
                if (std::getline(lineStream, col, ',')) {
                    close = std::stod(col);
                }
                if (std::getline(lineStream, col, ',')) {
                    adj_close = std::stod(col);
                }
                if (std::getline(lineStream, col, ',')) {
                    volume = std::stoul(col);
                }
                if (std::getline(lineStream, col, ',')) {
                    div_amt = std::stod(col);
                }
                if (std::getline(lineStream, col, ',')) {
                    split_coef = std::stod(col);
                }
            }
        };
    } // namespace intv

    namespace ts {

        class ts_daily_adj {
          public:
            std::string symbol;
            std::vector<intv::intv_daily_adj> intvs;
            std::map<std::string, intv::intv_daily_adj> intvs_map;
            std::string base_url = api::domain +
                                   "query?function=TIME_SERIES_DAILY_ADJUSTED&"
                                   "outputsize=full&datatype=csv";

          private:
            void __read_csv_intvs(std::istream &f) noexcept {

                std::string line;

                getline(f, line);
                std::string s = "timestamp";
                if (line.compare(0, s.length(), s) != 0) {
                    ALPHVANT_LOG_ERROR << "csv file has no header";
                    return;
                }

                while (getline(f, line)) {
                    auto i = intv::intv_daily_adj(line);

                    this->intvs.push_back(i);
                    // this->intvs_map[i.timestamp] = i;
                }
            }

            std::string download() noexcept {
                std::string url = base_url + "&symbol=" + this->symbol +
                                  "&apikey=" + api::GLOB_KEY;

                ALPHVANT_LOG_INFO << "URL: " << url;
                cpr::Response r = cpr::Get(cpr::Url{url});

                if (r.status_code != 200) {
                    std::string msg = "Failed to download : " + r.error.message;
                    ALPHVANT_LOG_ERROR << msg;
                    return "";
                }
                return r.text;
            }

            bool check_cache() {
                namespace fs = std::filesystem;
                auto fp = sconfig::TS_DAILY_ADJ_PATH /
                          fs::path(this->symbol + ".csv");

                if (!fs::exists(fp)) {
                    ALPHVANT_LOG_INFO << "file '" << fp
                                      << "' doesn't exist in cache";
                    return false;
                }

                auto ftime = fs::last_write_time(fp.string());
                auto sctp = std::chrono::time_point_cast<
                    std::chrono::system_clock::duration>(
                    ftime - fs::file_time_type::clock::now() +
                    std::chrono::system_clock::now());

                std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);

                // Get today's date
                auto today = std::chrono::system_clock::now();
                std::time_t ctoday =
                    std::chrono::system_clock::to_time_t(today);

                std::tm *lt = std::localtime(&ctoday);
                lt->tm_hour = 0;
                lt->tm_min = 0;
                lt->tm_sec = 0;
                std::time_t midnight = std::mktime(lt);

                if (std::difftime(cftime, midnight) >= 0) {
                    ALPHVANT_LOG_INFO << "file '" << fp
                                      << "' found in cache and up-to-date";
                    return true;
                } else {
                    ALPHVANT_LOG_INFO << "file '" << fp
                                      << "' found in cache, but NOT up-to-date";
                    return false;
                }
            }

            void cache(std::string s) {
                namespace fs = std::filesystem;
                auto fp = sconfig::TS_DAILY_ADJ_PATH /
                          fs::path(this->symbol + ".csv");
                ALPHVANT_LOG_INFO << "Caching file '" << fp << "'";

                std::ofstream outfile(fp.string());
                outfile << s;
            }

            void load_from_cache() {
                namespace fs = std::filesystem;
                auto fp = sconfig::TS_DAILY_ADJ_PATH /
                          fs::path(this->symbol + ".csv");

                std::ifstream infile(fp.string());

                this->__read_csv_intvs(infile);
            }

            void load_data() {
                std::string s;
                if (this->check_cache()) {
                    this->load_from_cache();
                } else {
                    s = this->download();
                    this->cache(s);
                    std::istringstream ss(s);
                    this->__read_csv_intvs(ss);
                }
            }

          public:
            ts_daily_adj(const std::string &symbol) {
                this->symbol = symbol;

                this->load_data();

                std::sort(this->intvs.begin(), this->intvs.end(),
                          [&](const intv::intv_daily_adj &a,
                              const intv::intv_daily_adj &b) {
                              return a.timestamp < b.timestamp;
                          });
            }
        };

    } // namespace ts

} // namespace alphvant