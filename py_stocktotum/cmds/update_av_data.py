import io
import os

import alphvant
import pandas as pd
import requests

import config

from . import cmd_utils

SYMBOLS: list[str] = [
    "IBM",
]


def _get_intraday_params(symbol: str) -> dict[str, str]:
    return {
        "function": "TIME_SERIES_DAILY",
        "symbol": "symbol",
        "apikey": config.AV_KEY,
        "datatype": "csv",
        "outputsize": "full",
    }


def main_v1() -> None:
    for symbol in SYMBOLS:
        params = _get_intraday_params(symbol)
        response = requests.get(config.AV_DOMAIN, params=params)
        decoded_content = response.content.decode("utf-8")
        df = pd.read_csv(io.StringIO(decoded_content))
        alphvant.file.write_csv(os.path.join("daily", f"{symbol}.csv"), df)
