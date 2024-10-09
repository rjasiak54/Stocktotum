import os

import pandas as pd
import tqdm

from . import time_series

_DEFAULT_VOLUMOUS_SYMBOLS_NAME = "avdata/listings/volumous_symbols.csv"


def get_all_used_symbols() -> list[str]:
    return [l.replace(".csv", "") for l in sorted(os.listdir("avdata/ts-daily-adj"))]


def get_volumous_symbols(fname: str = _DEFAULT_VOLUMOUS_SYMBOLS_NAME) -> list[str]:
    df = pd.read_csv(fname)
    return df["symbol"].to_list()


def compute_voluous_symbols(fname: str = _DEFAULT_VOLUMOUS_SYMBOLS_NAME) -> list[str]:
    symbols = get_all_used_symbols()
    okie_doke = []
    for s in tqdm.tqdm(symbols):
        ts = time_series.daily(s, refresh=False)
        if ts.tail(100).adjusted_close.mean() < 10:
            continue

        okie_doke.append(s)

    df = pd.DataFrame({"symbol": okie_doke})
    df.to_csv(fname, index=False)
    return okie_doke
