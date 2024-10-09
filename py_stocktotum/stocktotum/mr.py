import typing

import pandas as pd

from . import sma


def std(df: pd.DataFrame, period: int) -> pd.Series:
    return df["adjusted_close"].rolling(window=period, min_periods=period).std()


def mr_bsp(
    ts: pd.DataFrame,
    smaperiod: int,
    uperiod: typing.Optional[int] = None,
    lperiod: typing.Optional[int] = None,
) -> pd.DataFrame:

    if uperiod is None:
        uperiod = smaperiod
    if lperiod is None:
        lperiod = smaperiod

    ts["sma"] = sma.sma(ts, smaperiod)

    ts["ustd"] = std(ts, uperiod)
    ts["lstd"] = std(ts, lperiod)

    ts["ll"] = ts["sma"] - ts["ustd"]
    ts["ul"] = ts["sma"] + ts["lstd"]
    ts["buy_signal"] = False
    ts["sell_signal"] = False

    # Loop through the DataFrame to find crossovers
    bought = False
    for i in range(1, len(ts)):
        if (
            not bought
            and ts["adjusted_close"].iloc[i - 1] > ts["ll"].iloc[i - 1]
            and ts["adjusted_close"].iloc[i] < ts["ll"].iloc[i]
        ):
            ts.at[ts.index[i], "buy_signal"] = True
            bought = True
        elif (
            bought
            and ts["adjusted_close"].iloc[i - 1] < ts["ul"].iloc[i - 1]
            and ts["adjusted_close"].iloc[i] > ts["ul"].iloc[i]
        ):
            ts.at[ts.index[i], "sell_signal"] = True
            bought = False

    return ts


def mr_stats(ts: pd.DataFrame) -> pd.DataFrame:

    buy_signals = ts[ts["buy_signal"]].reset_index()
    sell_signals = ts[ts["sell_signal"]].reset_index()

    stats = pd.DataFrame()
    stats["bought_price"] = buy_signals["adjusted_close"]
    stats["sell_price"] = sell_signals["adjusted_close"]
    stats["earnings"] = sell_signals["adjusted_close"] - buy_signals["adjusted_close"]
    stats["buy_date"] = buy_signals["timestamp"]
    stats["sell_date"] = sell_signals["timestamp"]

    return stats
