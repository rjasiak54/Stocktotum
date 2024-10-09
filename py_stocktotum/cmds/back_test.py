# import itertools
import os
import typing

import alphvant
import alphvant.browse_symbols
import alphvant.cache
import alphvant.time_series
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import stocktotum
import stocktotum.mr
import tqdm

from . import graph_pt


def mr(
    smaperiod: int,
    uperiod: typing.Optional[int] = None,
    lperiod: typing.Optional[int] = None,
    startdate: typing.Optional[str] = None,
) -> None:
    # listing_status = pd.read_csv("avdata/listings/listing_status.csv")
    # symbols = listing_status.sample(50).symbol.to_list()
    symbols = _load_already_used_symbols()
    pps = []
    earns = []
    used_symbols = []
    print(symbols)
    for symbol in symbols:
        print(f"symbol: {symbol}")
        ts = alphvant.time_series.daily(symbol)
        if len(ts) < smaperiod:
            continue
        used_symbols.append(symbol)
        ts = stocktotum.mr.mr_bsp(ts, smaperiod, uperiod, lperiod)
        stats = stocktotum.mr.mr_stats(ts)
        percent_positive = len(stats[stats["earnings"] > 0]) / len(stats)
        pps.append(percent_positive)
        earns.append(stats.earnings.sum())
    df = pd.DataFrame({"symbol": used_symbols, "earnings": earns, "pps": pps})
    df.sort_values(by="earnings", inplace=False).to_csv("avdata/backtest/mr.csv")
    print(df)
    print("total earnings: ", df["earnings"].sum())
    print("% win rate: ", df["pps"].mean() * 100)


_TRANSACTIONS: list = []


def pt_mr(
    smaperiod: int,
    uperiod: typing.Optional[int] = None,
    lperiod: typing.Optional[int] = None,
    startdate: typing.Optional[str] = None,
    enddate: typing.Optional[str] = None,
) -> None:
    coint_start = "2022-01-03"
    coint_end = "2023-07-03"
    # coints = graph_pt.find_corrs_on_all_symbols(coint_start, coint_end)
    coints = pd.read_csv("avdata/pt/coint_passes.csv")
    coints["coint_start"] = coint_start
    coints["coint_end"] = coint_end
    coints.rename(columns={"0": "sym1", "1": "sym2"}, inplace=True)
    coints.apply(pt_mr_row, axis=1)  # type: ignore
    df = pd.DataFrame(_TRANSACTIONS)
    df.to_csv("avdata/pt/pt_results.csv", index=False)


def pt_mr_row(row: pd.Series) -> None:
    try:

        print(row.sym1, row.sym2)

        ts1, ts2 = load_ts_pair(row.sym1, row.sym2, row.coint_start)

        # print(ts1)
        # print(ts2)

        s1log = row.sym1 + "-log"
        s2log = row.sym2 + "-log"

        df = pd.DataFrame()
        df[row.sym1] = ts1.adjusted_close
        df[row.sym2] = ts2.adjusted_close
        df[s1log] = np.log(ts1.adjusted_close.to_numpy())
        df[s2log] = np.log(ts2.adjusted_close.to_numpy())

        df["spread"] = df[s1log] - df[s2log]

        df["spm"] = df["spread"].mean()
        df["sps"] = df["spread"].std()

        df["spm-1std"] = df["spm"] - df["sps"]
        df["spm+1std"] = df["spm"] + df["sps"]
        del df["sps"]

        df.reset_index(inplace=True)
        df["row_number"] = df.index

        df.set_index("timestamp", inplace=True)
        df.sort_values("timestamp", inplace=True)
        df = perform_bs_actions(df, row.sym1, row.sym2, row.coint_end)

        plt.style.use("dark_background")
        df[[row.sym1, row.sym2, "spm", "spm-1std", "spm+1std", "spread"]].plot(
            figsize=(25, 12)
        )

        start_L = df[df["start_L"]]
        plt.scatter(
            x=start_L["row_number"],
            y=start_L[row.sym1],
            marker="^",
            color="mediumpurple",
            label="start_1",
            alpha=1,
            s=200,
            zorder=5,
        )
        plt.scatter(
            x=start_L["row_number"],
            y=start_L[row.sym2],
            marker="^",
            color="mediumpurple",
            label="start_1",
            alpha=1,
            s=200,
            zorder=5,
        )

        end_L = df[df["end_L"]]
        plt.scatter(
            x=end_L["row_number"],
            y=end_L[row.sym1],
            marker="^",
            color="thistle",
            label="end_1",
            alpha=1,
            s=200,
            zorder=5,
        )
        plt.scatter(
            x=end_L["row_number"],
            y=end_L[row.sym2],
            marker="^",
            color="thistle",
            label="end_1",
            alpha=1,
            s=200,
            zorder=5,
        )
        start_U = df[df["start_U"]]
        plt.scatter(
            x=start_U["row_number"],
            y=start_U[row.sym2],
            marker="^",
            color="green",
            label="start_2",
            alpha=1,
            s=200,
            zorder=5,
        )
        plt.scatter(
            x=start_U["row_number"],
            y=start_U[row.sym1],
            marker="^",
            color="green",
            label="start_2",
            alpha=1,
            s=200,
            zorder=5,
        )
        end_U = df[df["end_U"]]
        plt.scatter(
            x=end_U["row_number"],
            y=end_U[row.sym2],
            marker="^",
            color="palegreen",
            label="end_2",
            alpha=1,
            s=200,
            zorder=5,
        )
        plt.scatter(
            x=end_U["row_number"],
            y=end_U[row.sym1],
            marker="^",
            color="palegreen",
            label="end_2",
            alpha=1,
            s=200,
            zorder=5,
        )

        plt.title("Time Series Data")
        plt.xlabel("Timestamps")
        plt.ylabel("Values")
        plt.legend(loc="upper left")
        plt.grid(True)
        # plt.show()
    except Exception as e:
        print(f"Failed! {e}")


def perform_bs_actions(
    ts: pd.DataFrame, s1: str, s2: str, startdate: str
) -> pd.DataFrame:

    ts["start_L"] = False
    ts["end_L"] = False
    ts["start_U"] = False
    ts["end_U"] = False

    took_action_L = False
    took_action_U = False

    last_transaction_L = 0
    last_transaction_U = 0

    for i in range(1, len(ts)):
        curr = ts.iloc[i]
        prev = ts.iloc[i - 1]
        if ts.index[i] < startdate:
            continue
        if (
            not took_action_L
            and curr.spread < curr["spm-1std"]
            and prev.spread > prev["spm-1std"]
        ):
            _TRANSACTIONS.append(
                {
                    "timestamp": ts.index[i],
                    "buy_symbol": s1,
                    "spent": curr[s1],
                    "sell_symbol": s2,
                    "recieved": curr[s2],
                }
            )
            last_transaction_L = curr[s2] - curr[s1]
            print(f"{curr.name} - START L - Buy  {s1}, Sell {s2}, {last_transaction_L}")
            ts.at[ts.index[i], "start_L"] = True
            took_action_L = True
        elif took_action_L and curr.spread > curr.spm and prev.spread < prev.spm:
            _TRANSACTIONS.append(
                {
                    "timestamp": ts.index[i],
                    "buy_symbol": s2,
                    "spent": curr[s2],
                    "sell_symbol": s1,
                    "recieved": curr[s1],
                }
            )
            rev = curr[s1] - curr[s2]
            print(
                f"{curr.name} - END   L - Sell {s1}, Buy  {s2}, rev: {rev}, prof: {rev + last_transaction_L}"
            )
            ts.at[ts.index[i], "end_L"] = True
            took_action_L = False

        if (
            not took_action_U
            and curr.spread > curr["spm+1std"]
            and prev.spread < prev["spm+1std"]
        ):
            _TRANSACTIONS.append(
                {
                    "timestamp": ts.index[i],
                    "buy_symbol": s2,
                    "spent": curr[s2],
                    "sell_symbol": s1,
                    "recieved": curr[s1],
                }
            )
            last_transaction_U = curr[s1] - curr[s2]
            print(f"{curr.name} - START U - Buy  {s1}, Sell {s2}, {last_transaction_U}")
            ts.at[ts.index[i], "start_U"] = True
            took_action_U = True
        elif took_action_U and curr.spread < curr.spm and prev.spread > prev.spm:
            rev = curr[s2] - curr[s1]
            _TRANSACTIONS.append(
                {
                    "timestamp": ts.index[i],
                    "buy_symbol": s1,
                    "spent": curr[s1],
                    "sell_symbol": s2,
                    "recieved": curr[s2],
                }
            )
            print(
                f"{curr.name} - END   U - Sell {s1}, Buy  {s2}, rev: {rev}, prof: {rev + last_transaction_U}"
            )
            ts.at[ts.index[i], "end_U"] = True
            took_action_U = False
    return ts


def load_ts_pair(s1: str, s2: str, startdate: str) -> tuple[pd.DataFrame, pd.DataFrame]:
    ts1 = alphvant.time_series.daily(s1, startdate=startdate)
    ts2 = alphvant.time_series.daily(s2, startdate=startdate)

    if len(ts1) != len(ts2):
        start = max(ts1.index.min(), ts2.index.min())
        end = min(ts1.index.max(), ts2.index.max())
        ts1 = ts1.loc[start:end]
        ts2 = ts2.loc[start:end]

    return (ts1, ts2)


def _logg(symbol: str, total: float, win_rate: float) -> None:
    print(f"{symbol:<{7}} {round(total, 3):<{9}} {round(win_rate*100, 3)}%")


_DEFAULT_SYMBOLS = [
    "INTC",
    "ERIC",
    "TSLA",
    "POT",
    "AGNC",
    "EEM",
    "EWZ",
    "TCBI",
    "RIVN",
    "C",
    "SPY",
    "BAC",
    "JPM",
    "AMD",
    "IWM",
    "SOUN",
    "AAPL",
    "MSFT",
    "XOM",
    "AMZN",
    "SOFI",
    "HPQ",
    "GOOG",
    "AAL",
    "LCID",
    "CSCO",
    "QQQ",
    "OPEN",
    "GLD",
    "EFA",
    "NVDA",
    "SDS",
    "SIRI",
]


def _load_already_used_symbols() -> list[str]:
    return [l.replace(".csv", "") for l in sorted(os.listdir("avdata/ts-daily-adj"))]
