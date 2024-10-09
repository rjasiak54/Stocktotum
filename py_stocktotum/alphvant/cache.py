import datetime
import os

import pandas as pd

_ALL_READ = dict[str, pd.DataFrame]()


def exists(fname: str) -> bool:
    full = _make_full(fname)
    return os.path.exists(full)


def is_valid(fname: str) -> bool:
    # return True
    full = _make_full(fname)
    today_str = datetime.datetime.today().strftime("%Y-%m-%d")
    timestamp = os.path.getmtime(full)
    last_mod_date = datetime.datetime.fromtimestamp(timestamp)
    las_mod_str = last_mod_date.strftime("%Y-%m-%d")
    return las_mod_str == today_str


def write_csv(df: pd.DataFrame, fname: str) -> None:
    full = _make_full(fname)
    df.to_csv(full)


def read_csv(fname: str) -> pd.DataFrame:
    full = _make_full(fname)
    if full in _ALL_READ:
        return _ALL_READ[full]

    df = pd.read_csv(full)
    df["row_number"] = df.index
    df.set_index("timestamp", inplace=True)
    df.sort_values("timestamp", inplace=True)
    _ALL_READ[full] = df
    return df  # .iloc[::-1]


def _make_full(fname: str) -> str:
    return os.path.join("avdata", fname)
