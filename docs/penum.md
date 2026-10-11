# `pokerstove::penum`

The penum library combines weighted card distributions with exhaustive showdown
enumeration. It deals missing pocket and board cards from the remaining deck,
evaluates each legal completion with [peval](peval.md), and accumulates pot
shares. There is no Monte Carlo sampling or betting strategy model.

Link an installed CMake consumer to `pokerstove::penum`, which brings in peval
transitively. Public headers use `<penum/...>` and the namespace is
`pokerstove`. See [the README](../README.md#install-and-consume-the-c-libraries)
for build and installation instructions.

## Card distributions

`<penum/CardDistribution.h>` defines `CardDistribution`: a list of raw card
sets and their relative weights. Entries may be complete or partial pockets.
A default-constructed distribution contains one empty card set with weight 1;
the enumerator completes it randomly to the selected game's pocket size.
`CardDistribution(CardSet("AcAs"))` instead fixes the two supplied cards.
`clear()` removes all entries; it does not reset to random.

### Parsing

`parse(text)` accepts comma-separated raw hands with optional `=weight`:

```cpp
pokerstove::CardDistribution range;
if (!range.parse("AcAs=+0.125,KhKd=1e-3")) {
    // Invalid input; range retains its previous contents.
}
```

- Each card is a two-character token, with ranks `23456789TJQKA` and suits
  `cdhs`. Hands such as `AcAs` name physical cards, not rank classes.
- Omitted weights default to 1. Weights need not sum to 1.
- Weights must be finite and nonnegative, in decimal/scientific notation with
  an ASCII decimal point and no whitespace. A leading `+` is accepted.
  Invalid syntax, overflow/underflow, nonfinite values, and negative weights
  return `false`.
- Invalid cards, duplicate cards within a hand, empty entries, and trailing
  commas return `false`. Failure preserves the entire previous distribution;
  success replaces it.
- `.` as the entire input selects the random (empty-card) distribution.
  It cannot be mixed with raw hands in a comma-separated list.
- Shorthand such as `AKs`, `QQ+`, or `22-99` is not supported.
- Game-specific card counts are not checked by `parse`; validate them before
  enumeration.

Repeated entries for the same card set have legacy behavior: every occurrence
is retained in the hand list, but the weight map keeps only the last weight.
Thus `AcAs=1,AcAs=2` has two list entries, both enumerated with weight 2, while
`weight()` returns 2. Reject or combine duplicate entries before using a range;
do not rely on `weight()` as an enumeration total in that case.

### Other operations

| Operation | Current behavior |
| --- | --- |
| `size()` | Number of hand-list entries, including repeated entries. |
| `dist[index]` | Card set at a list index; throws on an out-of-range index. |
| `const_dist[hand]` | Stored weight, or zero if the hand is absent. |
| `dist[hand] = weight` | Updates the weight map; does not add a new hand-list entry or validate the weight. |
| `weight()` | Sum of the stored weight map, not a normalized probability. |
| `removeCards(dead)` | Sets colliding entries' weights to zero; retains entries and their indices. |
| `fill(n)` | Replaces the distribution with all distinct `n`-card combinations of the full deck, each weighted 1. |
| `fill(cards, n)` | Uses the supplied available-card set instead of the full deck. |
| `data()` | Returns a copy of the raw distribution. |
| `str()`, `display()` | Comma-separated hands and weights printed with three decimal places. |

Require `0 <= n <= cards.size()` for `fill(cards, n)`. Large fills materialize
both a vector and a map and can consume substantial memory. Prefer the default
random distribution for unrestricted pockets. Display rounds weights, so it is
not a lossless serialization format for small or high-precision weights.

## Exhaustive enumeration

`<penum/ShowdownEnumerator.h>` exposes:

```cpp
std::vector<pokerstove::EquityResult> calculateEquity(
    const std::vector<pokerstove::CardDistribution>& dists,
    const pokerstove::CardSet& board,
    std::shared_ptr<pokerstove::PokerHandEvaluator> evaluator) const;
```

Provide one distribution per player, in result order. For each combination of
range entries the enumerator:

1. Collects fixed pocket/board cards and skips combinations with collisions.
2. Multiplies the selected range weights.
3. Removes fixed cards from a fresh remaining deck and partitions the available
   cards into the missing pocket and board cards.
4. Evaluates every completion and adds weighted win/tie shares to each result.

Cards within each partition are unordered, players are distinct, and cards
cannot be shared between partitions. Fixed complete hands with a complete
board produce one showdown per noncolliding range combination. A partial
Hold'em board with fixed pockets produces `choose(remaining_cards,
missing_board_cards)` completions. For example, two fixed two-card pockets on
a flop leave 45 cards and produce `choose(45, 2) = 990` turn/river combinations.

An empty pocket means all cards are unknown; a singleton such as `Ac` fixes
one card and completes the rest. This completion happens before the game
evaluator is called, so direct evaluation's partial-hand restrictions do not
apply to the completed pockets. The board is fixed or partial, not a weighted
board distribution. Games with `boardSize() == 0` should receive an empty board.
There is no separate dead-card argument or draw/discard simulation.

Weights apply to each enumerated completion. With uniformly sized range entries,
this gives relative weights over legal completions. Mixing differently sized
partial hands gives entries different numbers of completions, so their total
contribution is not simply proportional to their listed weights. Keep the
number of known cards consistent when that interpretation matters.

### Preconditions and current limits

Input checks are incomplete in the inherited implementation. Before calling
`calculateEquity`, require:

- A nonnull evaluator (null throws `std::runtime_error`), at least two players,
  and nonempty distributions. Player/distribution checks currently use assertions.
- Valid standard-deck masks and pocket/board sizes no greater than
  `handSize()`/`boardSize()`. Oversized inputs can underflow missing-card counts.
- Enough live cards to fill every partition. Do not pass impossible deals.
- Finite nonnegative weights, also when weights were assigned directly through
  `operator[]`. At least one positive-weight, noncolliding outcome must exist
  for equity normalization.
- At most ten players for single-component games or five for split-pot games;
  the inherited payout lookup supports indices only up to 10.

The enumerator skips collisions, but does not reject an entire impossible
scenario with an exception. All-colliding or all-zero ranges yield zero total
shares; normalization must handle that case. Zero-weight entries still cost
enumeration work, including entries marked by `removeCards`.

Exhaustive runtime grows with the Cartesian product of ranges and the number
of missing-card partitions. Prefer complete pockets and later boards for
small examples. The current implementation is serial and has no cancellation,
progress callback, or configurable sampling count.

## Calculating equity

Returned `EquityResult` objects contain accumulated `winShares` and `tieShares`.
A sole winner receives the awarded pot component as win shares; tied winners
receive their fractional part as tie shares. These are weighted pot shares,
not counts of outright wins or ties. When a low qualifies, high and low each
receive half the pot; otherwise high receives the whole pot.

`calculateEquity` leaves `equity` and the unused `equity2` at zero. Normalize
using one **fixed** total across all players:

```cpp
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <penum/ShowdownEnumerator.h>

int main()
{
    using namespace pokerstove;
    auto evaluator = PokerHandEvaluator::alloc("h");
    if (!evaluator)
        throw std::runtime_error("Unsupported game");
    const std::vector<CardDistribution> players{
        CardDistribution(CardSet("AcAs")),
        CardDistribution(CardSet("KhKd"))
    };
    // The river is unknown: enumerate each of the 44 remaining cards.
    auto results = ShowdownEnumerator().calculateEquity(
        players, CardSet("2c3d4h5s"), evaluator);

    double total = 0.0;
    for (const auto& result : results)
        total += result.winShares + result.tieShares;
    if (!std::isfinite(total) || total <= 0.0)
        throw std::runtime_error("No finite positive total pot share");
    for (auto& result : results) {
        result.equity = (result.winShares + result.tieShares) / total;
        std::cout << result.equity * 100.0 << "%\n";
    }
}
```

Do not currently use `EquityResult::normalize()` for this: it changes the
denominator while assigning equities, causing later players' values to be
incorrect. This guide documents a workaround; it does not fix that method.

The equivalent CLI scenario is:

```sh
build/release/bin/ps-eval AcAs KhKd --board 2c3d4h5s
# Weighted range on a complete board (quote each player's distribution):
build/release/bin/ps-eval 'AcAs=2,AhAd=1' KhKd --board 2c3d4h5s9c
```

Each hand argument is one player; commas separate that player's range entries.
`--game` selects the evaluator (default `h`; see [game IDs](peval.md#game-evaluation)).
A single player argument makes `ps-eval` add a random opponent. Its percentage
output uses a fixed total; the raw result tuple prints accumulated shares and
still-zero `equity`/`equity2` fields.

## Enumeration building blocks

`PartitionEnumerator2` in `<penum/PartitionEnumerator.h>` enumerates ordered,
disjoint partitions of an index set for supplied partition sizes. Read the
initial state before advancing with `next()`, commonly in a `do/while` loop.
`get(part)` returns selected indices and `getMask(part)` returns a bit mask of
those indices. Supply a nonempty partition-size vector, sizes whose sum does
not exceed the set size, and at most 64 elements when using masks.

`SimpleDeck` in `<penum/SimpleDeck.hpp>` stores the 52 cards and a live-card
boundary. `peek(mask)` interprets bits as **deck positions**, not physical card
codes; partition masks must therefore be interpreted through the same deck.
`reset()` restores the live boundary without restoring order, and `shuffle()`
shuffles and resets it. `deal(n)` requires `n <= size()`; it does not check
capacity. For reliable removal, reset the deck and remove the combined dead
set once, as the showdown enumerator does, rather than assuming incremental
removal always preserves earlier dead cards.
