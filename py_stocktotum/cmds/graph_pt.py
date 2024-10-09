import datetime
import itertools
import os
import typing

import alphvant
import alphvant.browse_symbols
import alphvant.time_series
import dateutil  # type: ignore
import matplotlib.pyplot as plt
import pandas as pd
import statsmodels.tsa.stattools as st
import stocktotum
import tqdm


def main() -> None:
    startdate = "2022-01-01"
    print("startdate: ", startdate)
    #
    # Get coint pairs:
    find_corrs_on_all_symbols()
    return
    df = pd.read_csv("avdata/pt/coint_passes.csv")
    # df.sort_values)

    for i, s1, s2, pv in df.itertuples():
        print(s1, s2, pv)
        stk1 = alphvant.time_series.daily(s1)
        stk2 = alphvant.time_series.daily(s2)
        spread = stk1.adjusted_close - stk2.adjusted_close
        spm = spread.mean()
        sps = spread.std()
        # Simulate trading
        positions = []
        # Define entry and exit thresholds
        entry_threshold = 2 * sps
        exit_threshold = 0.5 * sps
        for i in range(len(spread)):
            if spread[i] > spm + entry_threshold:
                positions.append(("Sell Stock1, Buy Stock2", stk1.index[i]))
            elif spread[i] < spm - entry_threshold:
                positions.append(("Buy Stock1, Sell Stock2", stk1.index[i]))
            elif abs(spread[i] - spm) < exit_threshold and positions:
                positions.append(("Close Position", stk1.index[i]))

    print(df)


# def main()


def find_corrs_on_all_symbols(
    startdate: str = "2023-01-01",
    enddate: typing.Optional[str] = None,
) -> pd.DataFrame:
    symbols = alphvant.browse_symbols.get_volumous_symbols()

    pairs = itertools.combinations(symbols, 2)

    coint_pass = []
    for s1, s2 in tqdm.tqdm(list(pairs)):
        try:
            stk1 = alphvant.time_series.daily(s1, startdate, enddate=enddate)
            stk2 = alphvant.time_series.daily(s2, startdate, enddate=enddate)

            if len(stk1) != len(stk2):
                continue
            if len(stk1) == 0:
                continue
            if stk1.tail(100).adjusted_close.mean() < 10:
                continue
            if stk2.tail(100).adjusted_close.mean() < 10:
                continue

            coint_test = st.coint(stk1.adjusted_close, stk2.adjusted_close)
            tstat, pv, cr = coint_test
            if pv < 0.01:
                coint_pass.append([s1, s2, pv])
        except Exception as e:
            print(e)
            print(f"problem at {s1}, {s2}")

    coint_pass_df = pd.DataFrame(coint_pass)

    coint_pass_df.to_csv("avdata/pt/coint_passes.csv", index=False)
    return coint_pass_df


def run_corr() -> None:
    startdate = "2023-01-01"
    print("startdate: ", startdate)

    symbols = _load_already_used_symbols()
    closes_columns = []

    for s in symbols:
        ts = alphvant.time_series.daily(s, startdate)
        closes_columns.append(ts.adjusted_close.rename(s))

    closes_df = pd.concat(closes_columns, axis=1)
    corrs = closes_df.corr()
    corrs.to_csv("avdata/pt/corrs.csv")
    print(closes_df.corr())

    corr_unstacked = corrs.unstack()
    corr_unstacked = corr_unstacked[corr_unstacked != 1.0]
    corr_unstacked = corr_unstacked.sort_values()
    corr_unstacked.to_csv("avdata/pt/corr_unstacked.csv")


def _load_already_used_symbols() -> list[str]:
    return [l.replace(".csv", "") for l in sorted(os.listdir("avdata/ts-daily-adj"))]
