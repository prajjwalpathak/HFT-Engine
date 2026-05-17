# HFT-Engine

Low-Latency Exchange Platform built in modern C++.

HFT-Engine is a low-latency exchange engine and real-time market simulation platform built in modern C++. The system features an intrusive FIFO order book, custom memory pool allocator, UDP market dissemination, WebSocket-based live visualization dashboard, runtime simulation modes, and latency instrumentation for experimenting with high-performance trading infrastructure concepts.

MarketSimulator<br>
&emsp;&emsp;&emsp;&ensp;↓<br>
MatchingEngine<br>
&emsp;&emsp;&emsp;&ensp;↓<br>
UDP Publisher<br>
&emsp;&emsp;&emsp;&ensp;↓<br>
Node WebSocket Bridge<br>
&emsp;&emsp;&emsp;&ensp;↓<br>
Frontend Dashboard

## Features

- Intrusive FIFO order book
- Custom fixed-size memory pool allocator
- Partial fill matching support
- TCP-based order entry infrastructure
- UDP market data dissemination
- Real-time WebSocket dashboard
- Runtime simulation modes (Normal / HFT / Stress)
- Latency instrumentation
- Cache-line alignment and hot-path optimizations
- Randomized stress testing