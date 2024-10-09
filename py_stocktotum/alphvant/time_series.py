import datetime
import io
import logging
import os
import typing

import pandas as pd
import requests

import config

from . import cache


def daily(
    symbol: str,
    startdate: str | None = None,
    enddate: typing.Optional[str] = None,
    refresh: bool = True,
) -> pd.DataFrame:
    fname = os.path.join("ts-daily-adj", f"{symbol}.csv")
    df = _init_av_data(symbol, fname, refresh)
    if startdate is not None and enddate is not None:
        return df.loc[startdate:enddate]  # type: ignore
    if startdate is not None:
        df = df.loc[startdate:]  # type: ignore
    return df


def _init_av_data(symbol: str, fname: str, refresh: bool = True) -> pd.DataFrame:
    if not cache.exists(fname):
        _logg(f"{symbol:<8} - No Cache")
    else:
        if refresh and not cache.is_valid(fname):
            _logg(f"{symbol:<8} - Cache Expired")
        else:
            _logg(f"{symbol:<8} - Loading Cache")
            df = cache.read_csv(fname)
            return df
    return _refresh_from_av(symbol, fname)


def _refresh_from_av(symbol: str, fname: str) -> pd.DataFrame:
    _logg(f"{symbol:<8} - Refreshing from AV")
    p = _get_intraday_params(symbol)
    response = requests.get(config.AV_DOMAIN, params=p)
    decoded_content = response.content.decode("utf-8")
    df = pd.read_csv(io.StringIO(decoded_content))
    df = df.set_index("timestamp")
    cache.write_csv(df, fname)
    return df.iloc[::-1]


def _logg(msg: str) -> None:
    return
    logging.info(f"[TimeSeries] {msg}")


def _get_intraday_params(symbol: str) -> dict[str, str]:
    return {
        "function": "TIME_SERIES_DAILY_ADJUSTED",
        "symbol": symbol,
        "apikey": config.AV_KEY,
        "datatype": "csv",
        "outputsize": "full",
    }
