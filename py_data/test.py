import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import yfinance as yf

# Fetch historical data for a stock
data = yf.download("AAPL", start="2020-01-01", end="2021-01-01")
data["SMA_50"] = data["Close"].rolling(window=50).mean()
data["SMA_200"] = data["Close"].rolling(window=200).mean()

# Generate buy/sell signals
data["Signal"] = 0
data["Signal"][50:] = np.where(data["SMA_50"][50:] > data["SMA_200"][50:], 1, 0)
data["Position"] = data["Signal"].diff()

# Plot
plt.figure(figsize=(10, 5))
plt.plot(data["Close"], label="Close Price")
plt.plot(data["SMA_50"], label="50-Day SMA")
plt.plot(data["SMA_200"], label="200-Day SMA")
plt.plot(
    data[data["Position"] == 1].index,
    data["SMA_50"][data["Position"] == 1],
    "^",
    markersize=10,
    color="g",
    lw=0,
    label="Buy Signal",
)
plt.plot(
    data[data["Position"] == -1].index,
    data["SMA_50"][data["Position"] == -1],
    "v",
    markersize=10,
    color="r",
    lw=0,
    label="Sell Signal",
)
plt.title("Simple Moving Average Crossover Strategy")
plt.legend()
plt.show()
