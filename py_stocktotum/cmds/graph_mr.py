import datetime

import alphvant
import alphvant.time_series
import dateutil  # type: ignore
import matplotlib.pyplot as plt
import pandas as pd
import stocktotum


def main(symbol: str, startdate: str | None = None) -> None:
    period = 20
    print("startdate: ", startdate)
    ts = alphvant.time_series.daily(symbol, startdate)

    ts = stocktotum.mr.mr_bsp(ts, period)

    stats = stocktotum.mr.mr_stats(ts)
    print(stats)
    print(stats["earnings"].sum())

    d = get_n_years_ago_date(5)
    # ts = ts.loc[d:]
    ts = ts.reset_index()
    ts["row_number"] = ts.index
    ts.set_index("timestamp", inplace=True)
    ts.sort_values("timestamp", inplace=True)

    max_close = ts["ul"].max()
    max_vol = ts["volume"].max()
    ts["norm_volume"] = ts["volume"] / max_vol * (max_close)
    dd = ts[["adjusted_close", "sma", "ul", "ll", "norm_volume"]].plot(figsize=(25, 12))
    buy_signals = ts[ts["buy_signal"]]
    plt.scatter(
        x=buy_signals["row_number"],
        y=buy_signals["adjusted_close"],
        marker="^",
        color="g",
        label="Buy Signal",
        alpha=1,
        s=100,
        zorder=5,
    )

    # # Plot sell signals
    sell_signals = ts[ts["sell_signal"]]
    ax = plt.scatter(
        sell_signals["row_number"],
        sell_signals["adjusted_close"],
        marker="v",
        color="r",
        label="Sell Signal",
        alpha=1,
        s=100,
        zorder=5,
    )

    plt.title("Time Series Data")
    plt.xlabel("Timestamps")
    plt.ylabel("Values")
    plt.legend(loc="upper left")
    plt.grid(True)
    plt.show()


def get_n_years_ago_date(n: int) -> str:
    current_date = datetime.datetime.now()
    n_years_ago = current_date - dateutil.relativedelta.relativedelta(years=n)
    return n_years_ago.strftime("%Y-%m-%d")
