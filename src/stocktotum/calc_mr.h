#pragma once

#include <vector>
#include <numeric>

#include "structs.h"
#include "calc_sma.h"
#include "../alphvant/ts_daily_adj.h"
#include "../srctools/enumerate.hpp"

namespace stocktotum {
    namespace calc {
        std::vector<tmstp_dbl> mr(std::string symbol) {
            auto ts = alphvant::ts::ts_daily_adj(symbol);
            auto smas = sma(ts.intvs);
            auto smas = align(smas, ts.intvs);
        }
    } // namespace calc
} // namespace stocktotum