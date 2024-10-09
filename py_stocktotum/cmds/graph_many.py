import datetime
import itertools
import math

import alphvant
import alphvant.time_series
import dateutil  # type: ignore
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import stocktotum


def main(startdate: str, symbols: list[str]) -> None:

    closes = pd.DataFrame()

    adjusted_closes = {}

    for s in symbols:
        ts = alphvant.time_series.daily(s, startdate)
        adjusted_closes[s] = ts.adjusted_close

    closes = pd.DataFrame(adjusted_closes)
    if len(symbols) == 2:
        ns1 = np.log(closes[symbols[0]].to_numpy(np.float64))
        # print(ns1)
        # return
        # closes["spread"] = closes[symbols[0]] - closes[symbols[1]]
        # closes["spread"] = np.log(closes[symbols[0]].to_numpy(np.float64))
        closes[symbols[0] + "-log"] = np.log(closes[symbols[0]].to_numpy())
        closes[symbols[1] + "-log"] = np.log(closes[symbols[1]].to_numpy())
        closes["spread"] = np.log(closes[symbols[0]].to_numpy()) - np.log(
            closes[symbols[1]].to_numpy()
        )
        closes["spm"] = closes["spread"].mean()
        closes["sps"] = closes["spread"].std()

        spm = closes["spread"].mean()
        sps = closes["spread"].std()
        print(sps)
        minx = closes.index.min()
        maxx = closes.index.max()
        closes["spm"] = list(itertools.repeat(spm, len(closes)))
        closes["sps"] = list(itertools.repeat(sps, len(closes)))
        closes["spm-1std"] = closes["spm"] - closes["sps"]
        closes["spm+1std"] = closes["spm"] + closes["sps"]
        del closes["sps"]
        # del closes[symbols[0]]
        # del closes[symbols[1]]

        closes.reset_index(inplace=True)
        closes["row_number"] = closes.index

        closes.set_index("timestamp", inplace=True)
        closes.sort_values("timestamp", inplace=True)
        closes = perform_bs_actions(closes, symbols[0], symbols[1])
        print(closes)

        closes[[symbols[0], symbols[1], "spm", "spm-1std", "spm+1std", "spread"]].plot(
            figsize=(25, 12)
        )

        buy_1 = closes[closes["buy_1"]]
        plt.scatter(
            x=buy_1["row_number"],
            y=buy_1[symbols[0]],
            marker="^",
            color="black",
            label="Buy",
            alpha=1,
            s=100,
            zorder=5,
        )

        sell_1 = closes[closes["sell_1"]]
        plt.scatter(
            x=sell_1["row_number"],
            y=sell_1[symbols[0]],
            marker="^",
            color="g",
            label="Sell",
            alpha=1,
            s=100,
            zorder=5,
        )

        buy_1 = closes[closes["buy_1"]]
        plt.scatter(
            x=buy_1["row_number"],
            y=buy_1[symbols[1]],
            marker="^",
            color="black",
            label="Sell",
            alpha=1,
            s=100,
            zorder=5,
        )

        sell_1 = closes[closes["sell_1"]]
        plt.scatter(
            x=sell_1["row_number"],
            y=sell_1[symbols[1]],
            marker="^",
            color="r",
            label="Buy",
            alpha=1,
            s=100,
            zorder=5,
        )

        print(spm, sps)

        # print(adjusted_closes)
        # print(closes)

        plt.title("Time Series Data")
        plt.xlabel("Timestamps")
        plt.ylabel("Values")
        plt.legend(loc="upper left")
        plt.grid(True)
        plt.show()

    return


def perform_bs_actions(ts: pd.DataFrame, s1: str, s2: str) -> pd.DataFrame:

    ts["buy_1"] = False
    ts["sell_1"] = False
    ts["buy_2"] = False
    ts["sell_2"] = False

    took_action_L = False
    took_action_U = False
    print("here")
    l = len(ts)

    for i in range(1, len(ts)):
        curr = ts.iloc[i]
        prev = ts.iloc[i - 1]
        if (
            not took_action_L
            and curr.spread < curr["spm-1std"]
            and prev.spread > prev["spm-1std"]
        ):
            print(f"{curr.name} - START L - Buy  {s1}, Sell {s2}")
            ts.at[ts.index[i], "buy_1"] = True
            took_action_L = True
        elif took_action_L and curr.spread > curr.spm and prev.spread < prev.spm:
            print(f"{curr.name} - END   L - Sell {s1}, Buy  {s2}")
            ts.at[ts.index[i], "sell_1"] = True
            took_action_L = False
    return ts
