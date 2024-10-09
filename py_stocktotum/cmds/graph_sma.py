import datetime

import alphvant
import alphvant.time_series
import dateutil  # type: ignore
import matplotlib.pyplot as plt
import pandas as pd
import stocktotum
import stocktotum.sma


def main(symbol: str) -> None:
    ts = alphvant.time_series.daily(symbol)

    ts["sma50"] = stocktotum.sma(ts, 50)
    ts["sma200"] = stocktotum.sma(ts, 200)

    ts["buy_signal"] = False
    ts["sell_signal"] = False

    # Loop through the DataFrame to find crossovers
    for i in range(1, len(ts)):
        if (
            ts["sma50"].iloc[i - 1] < ts["sma200"].iloc[i - 1]
            and ts["sma50"].iloc[i] > ts["sma200"].iloc[i]
        ):
            ts.at[ts.index[i], "buy_signal"] = True
        elif (
            ts["sma50"].iloc[i - 1] > ts["sma200"].iloc[i - 1]
            and ts["sma50"].iloc[i] < ts["sma200"].iloc[i]
        ):
            ts.at[ts.index[i], "sell_signal"] = True

    # ts = ts.tail(1000)
    d = get_n_years_ago_date(6)
    ts = ts.loc[d:]
    ts = ts.reset_index()
    ts["row_number"] = ts.index
    ts.set_index("timestamp", inplace=True)
    ts.sort_values("timestamp", inplace=True)
    print(ts)

    dd = ts[["adjusted_close", "sma50", "sma200"]].plot(figsize=(12, 6))
    buy_signals = ts[ts["buy_signal"]]
    print(buy_signals)
    plt.scatter(
        x=buy_signals["row_number"],
        y=buy_signals["sma50"],
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
        sell_signals["sma50"],
        marker="v",
        color="r",
        label="Sell Signal",
        alpha=1,
        s=100,
        zorder=5,
    )
    plt.legend(loc=(1.04, 0))

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
