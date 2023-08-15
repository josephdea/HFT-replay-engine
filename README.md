# HFT-replay-engine

Simulates tick data on 4 different exchanges - DYDX, Binance, Coinbase, Kraken.
Functionality Includes:
 - gridsearching signals with the highest per-day correlation
 - deployment of linear regression and boosted tree models (LGBM models with linear mode on)
 - grid searching order execution methods for making and taking
 - simulation of order execution strategies and dumping PNL, volume and other useful statistics
