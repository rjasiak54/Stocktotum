import json
import logging
import os
import typing as _t

import config
import pandas as pd


def write_json(
    fname: str,
    obj: dict[str, _t.Any] | list[_t.Any],
    format: bool = True,
    indent: int = 4,
) -> None:
    full = _prepare_full_path(fname)
    with open(full, "w") as file:
        json.dump(obj, file, indent=indent if format else None)


def write_csv(fname: str, df: pd.DataFrame) -> None:
    full = _prepare_full_path(fname)
    df.to_csv(full, index=False)


def _prepare_full_path(fname: str) -> str:
    full = os.path.join(config.DATA_DIR, fname)
    path = os.path.dirname(full)
    if not os.path.exists(path):
        logging.info(f'making path "{path}"')
        os.makedirs(path)
    return full
