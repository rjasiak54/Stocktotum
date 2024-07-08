#pragma once

#include <vector>

#include "structs.h"
#include "../alphvant/ts_daily_adj.h"
#include "../srctools/enumerate.hpp"

namespace stocktotum {
    namespace calc {

        std::vector<tmstp_dbl> sma(const std::string &symbol, int n_days) {

            alphvant::ts::ts_daily_adj t(symbol);

            std::vector<tmstp_dbl> avgs;
            double sum = 0;
            int num_days_summed = 0;
            int skip_count = 0;
            int empty_count = 0;
            for (auto ii : srctools::enumerate(t.intvs)) {
                if (ii.item.timestamp == "") {
                    skip_count++;
                    auto to_remove = t.intvs[ii.index - (n_days - 1)];
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
                    auto to_remove = t.intvs[ii.index - (n_days - 1)];
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

    } // namespace calc
} // namespace stocktotum