import pandas as pd


#
# Simpler version of the above
def sma(df: pd.DataFrame, period: int) -> pd.Series:
    return df["adjusted_close"].rolling(window=period, min_periods=period).mean()
