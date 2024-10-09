import logging
import os
import typing as _t

import alphvant

META_DATA_KEY = "Meta Data"
SYMBOL_KEY = "2. Symbol"
LAST_REFRESHED_KEY = "3. Last Refreshed"
INTERVAL_KEY = "4. Interval"


def write_av_data(data: dict[str, _t.Any]) -> str:
    """
    Raises:
        `KeyError`
    """

    meta_data: dict[str, str] = data[META_DATA_KEY]

    symbol = meta_data[SYMBOL_KEY]
    interval = meta_data[INTERVAL_KEY]
    date = meta_data[LAST_REFRESHED_KEY].split()[0]

    fname = os.path.join(f"intraday-{interval}", symbol, f"{date}.json")
    alphvant.file.write_json(fname, data)

    return fname
