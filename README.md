# Limit-Order-Book

## Overview

A Limit Order Book (LOB) is the core data structure powering every modern stock exchange and trading venue. The LOB maintains two sorted lists of all outstanding orders — bid (buy) orders and ask (sell) orders — continuously matching the highest bid with the lowest ask until prices are matched.

The goal of this project is to challenge myself and develop an understanding of the infrastructure behind every major stock exchange, rather than simulating trading strategy. Beyond replicating standard exchange behavior, this engine introduces two custom order types — a Banded Execution Order and VWAP Pre-Execution Check — designed to address liquidity risk and unintended average fill costs that standard limit orders leave unsolved.

---

## Key Concepts

**Limit Order** — An order to buy or sell an asset or stock once it reaches the specified level or exceeds it.

**Bid** — The price at which an investor is willing to buy a security. The buyer sets the bid.

**Ask / Offer** — The price at which an investor is willing to sell a security. The seller sets the ask.

**Bid-Ask Spread** — The gap between the highest buy price and lowest sell price. When this spread is zero (or negative), a trade executes.

**Price-Time Priority** — At any price level, the order that arrived first gets matched first. This is the standard fairness rule used by most exchanges.

**Partial Fills** — A large incoming order can consume multiple resting orders at the same or different price levels before being fully filled or resting itself.

**Order Cancellation** — Resting orders can be cancelled at any time. The `unordered_map` of order IDs to book iterators enables O(1) lookup for removal without scanning the book.

---

## Features / Functionality

**Buy-Limit Order** — Tracks the price point the investor wants to purchase the security and matches it with an ask satisfying the bid or lower.
```
Example: An investor wants to buy security 'XYZ' but it currently trades at $20.00/share.
         They set a limit-buy order at $15.00/share, which will buy X shares once the
         share price falls to $15.00 or lower.

         Note: Although the investor is guaranteed to pay the buy-limit order price,
         in high volatility markets their order has a chance of not fulfilling.
```

**Sell-Limit Order** — Conversely to buy-limit orders, allows an investor to set a price at which they are willing to sell.
```
Example: An investor wants to sell their shares (assume it starts at $40.00/share)
         of a security, but are not willing to do so until it reaches $50.00/share.
         They set a sell-limit order of up to X shares which will execute at $50.00/share.
```

---

## Extending Limit-Order Options

### Banded Execution Order

A Banded Execution Order extends the standard limit order with a `tolerance_pct` parameter that defines an acceptable execution band below the limit price. A buy order with limit price `P` and tolerance `T` will only execute if the fill price satisfies:

```
fill_price >= P * (1 - T)
```

If the market price gaps beyond the tolerance floor, the order remains resting rather than executing at an unexpectedly adverse price. This protects investors from flash crashes and extreme slippage while still allowing execution within a reasonable range of their intended price.

```
Example: An investor wants to buy shares of company XYZ (currently at $30.00/share)
         and sets a banded-limit order with a limit buy of $25.00/share and a
         tolerance window of 25%.

         Floor = $25.00 * (1 - 0.25) = $18.75

         ├── Ask at $20.00 → within band ($20.00 >= $18.75) → executes
         └── Ask at $15.00 → below floor ($15.00 < $18.75)  → does not execute, order rests
```

```
├── Buy side:  floor protects against crash, ceiling allows slight overpay for liquidity
└── Sell side: ceiling protects against spike, floor allows slight underpay for liquidity
```

---

### VWAP Pre-Execution Check

When an investor places a large limit order on a thin book, standard greedy matching fills against the cheapest asks first and works progressively deeper into more expensive territory until the full quantity is satisfied. Each individual fill technically respects the limit price — but the blended average cost across all fills can creep far higher than the investor intended without them realizing it.

The core problem is that a limit price is not just a ceiling on a single fill — it should be the ceiling on the investor's **average** fill across their entire order.

Before executing, this engine calculates the volume-weighted average price (VWAP) across all available asks in price order up to the investor's requested quantity. Only if that blended average falls within the investor's limit does the order execute. Fill order remains unchanged — cheapest asks are still consumed first. The VWAP check is purely a pre-execution viability gate.

```
VWAP = Σ(ask_price × ask_volume) / total_requested_quantity

If VWAP <= limit_price → execute normally
If VWAP >  limit_price → reject, or partially fill up to the point the average remains acceptable
```

**Example A — Executes:**
```
Investor wants: 20,000 shares, limit $51.00

Ask $50.00 —  5,000 shares    cost:   $250,000
Ask $50.20 —  8,000 shares    cost:   $401,600
Ask $50.60 —  7,000 shares    cost:   $354,200
                               ──────────────────
Total:         20,000 shares   cost: $1,005,800

VWAP = $1,005,800 / 20,000 = $50.29

$50.29 <= $51.00 → execute
```

**Example B — Rejected (book too thin):**
```
Investor wants: 20,000 shares, limit $51.00

Ask $50.00 —  1,000 shares    cost:    $50,000
Ask $50.50 —  2,000 shares    cost:   $101,000
Ask $51.80 — 17,000 shares    cost:   $880,600
                               ──────────────────
Total:         20,000 shares   cost: $1,031,600

VWAP = $1,031,600 / 20,000 = $51.58

$51.58 > $51.00 → does not execute, investor is notified book is too thin
```

---

## Architecture

```
src/
├── main.cpp           # Entry point, benchmark runner
├── Order.h            # Order struct (id, side, price, qty, timestamp)
├── OrderBook.h/.cpp   # Core bid/ask book, add/cancel/match logic
├── MatchingEngine.h   # Wraps OrderBook, processes order stream
└── Utils.h            # Timing, logging, order generation helpers

tests/
├── test_matching.cpp  # Unit tests: basic fills, partial fills, cancel
└── test_benchmark.cpp # Throughput benchmarks (synthetic order stream)
```

---

## Data Structures

***TBD***

---

## Pseudo-Algorithm

***TBD***

---

## Math Required

***TBD***

---

## Performance

***TBD***

---

## References

Fernando, J. (n.d.). *Volume-weighted average price (VWAP): Definition and calculation.* Investopedia.
https://www.investopedia.com/terms/v/vwap.asp

Kenton, W. (n.d.). *What is a limit order book? Key concepts and data.* Investopedia.
https://www.investopedia.com/terms/l/limitorderbook.asp

Kramer, M. J. (n.d.). *What is a limit order in trading, and how does it work?* Investopedia.
https://www.investopedia.com/terms/l/limitorder.asp

---

*Part of a two-project portfolio targeting computational systems roles in quantitative finance and computational biology.*
