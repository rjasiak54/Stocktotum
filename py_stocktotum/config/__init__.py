import logging
import os

import toml

ENV_FILE: str = os.path.join("cicd", "local", "env.toml")
ENVS = toml.load(ENV_FILE)
AV_DOMAIN = "https://www.alphavantage.co/query"
AV_DEMO_PATH = "/query?function=TIME_SERIES_INTRADAY&symbol=IBM&interval=5min&apikey=demo&datatype=csv"
AV_KEY = ENVS["alpha_vantage"]["key"]

# AV_KEY =

DATA_DIR = os.path.join("data")
