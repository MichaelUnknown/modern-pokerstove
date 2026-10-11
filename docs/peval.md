# `pokerstove::peval`

The peval library represents cards and evaluates the strength of an existing
hand. It does not deal missing cards or perform equity rollouts. Use
[penum](penum.md) to enumerate possible hand and board completions.

Link an installed CMake consumer to `pokerstove::peval`; public includes use
`<peval/...>` and the C++ namespace remains `pokerstove`. The library requires
C++20. See [the README](../README.md#install-and-consume-the-c-libraries) for
build and installation instructions.

## Cards and card sets

`Card`, `Rank`, and `Suit` are defined in `<peval/Card.h>`, `<peval/Rank.h>`, and
`<peval/Suit.h>`. Use two-character card tokens: ranks `23456789TJQKA` and suits
`cdhs` (clubs, diamonds, hearts, spades). A ten is `T`, not `10`.

`<peval/CardSet.h>` defines an unordered set of unique cards stored in a
`uint64_t`. Bits 0–51 represent the standard deck, in suit blocks:
`2c` through `Ac`, then diamonds, hearts, and spades through `As`. Bits 52–63
are unused; they do not represent supported jokers or other cards. The raw-mask
constructor does not validate the mask, so callers must keep those bits clear.

```cpp
#include <iostream>
#include <peval/Card.h>
#include <peval/CardSet.h>

int main()
{
    using namespace pokerstove;
    CardSet big_slick = CardSet(Card(Rank('A'), Suit('s'))) |
                        CardSet(Card(Rank('K'), Suit('s')));
    CardSet dead_mans_hand("AcAs8d8hJs");
    std::cout << big_slick.str() << '\n';
    std::cout << dead_mans_hand.str() << '\n';
}
```

With the default ASCII suit display, `str()` prints in card-code order, so the
output is `KsAs` and `Ac8d8hJsAs`, rather than preserving input order.

### Input requirements

The `CardSet(string)` constructor is a legacy convenience parser, not a checked
input boundary. Duplicate cards clear the set, and malformed tokens are not
reliably rejected with exceptions. Validate syntax, uniqueness, and game-specific
card counts before construction. For external input, `CardDistribution::parse`
provides checked raw-card parsing (see [penum](penum.md#card-distributions)); for
a single hand, require exactly one entry and the expected card count.

Set union (`|`, `|=`, `insert`) combines known cards; intersection (`&`),
`contains`, `intersects`, and `disjoint` help enforce card consistency.
`cards()` returns `Card` objects and `cardSets()` returns singleton sets, both
in card-code order. `clear()` empties a set; `fill()` selects the entire deck.

The current `remove(Card)` and `remove(CardSet)` implementations use XOR. They
remove present cards but add absent ones. Only call them when all requested
cards are already present; they are not general set-difference operations.

### Formatting and suit canonicalization

- `rankstr()` returns the rank characters, including duplicates, sorted by
  character value rather than poker rank.
- `rankMask()` gives a 13-bit mask of occupied ranks.
- `toRankBitString()` actually prints 52 card-presence markers (`1` or `.`),
  grouped into four 13-card suit blocks, with spaces after each block.
- `Suit::setSuitStringType` changes suit display globally (ASCII, HTML, ANSI,
  and other formats). Only ASCII output is guaranteed to be parseable again;
  avoid changing the display mode concurrently with other output.
- `canonize()` relabels suits to a canonical representation; `canonize(other)`
  does so relative to another set. `canonizeToBoard(board, hand)` applies the
  board's canonical suit permutation to the hand. These operations relabel
  suits, not ranks, and are separate from dense indexing.

## Evaluation results and ordering

`<peval/PokerEvaluation.h>` defines an order-preserving evaluation code.
**A better hand compares greater than a worse hand**, including lowball and
Badugi evaluations. Compare only evaluations produced under the same rules;
high, A-5, 2-7, and Badugi codes are not interchangeable.

Use `str()`, `handType()`, `type()`, `majorRank()`, `minorRank()`, and
`kickerBits()` to inspect a result. `code()` exposes the integer encoding and
`bitstr()` helps debug it. Codes are not dense array indices. `showdownCode()`
and `reducedCode()` discard information; use the full evaluation for general
hand comparison, including kickers. No stable cross-version serialized code
format is promised here.

`<peval/PokerHandEvaluation.h>` holds up to two evaluations:

- `high()` or `eval(0)` is the primary evaluation, even in a low-only game.
- `low()` or `eval(1)` is the secondary evaluation for split-pot games.
- A zero secondary evaluation means there is no qualifying low.
  `highlow()` and `size()` describe the result, not the game's capabilities.
  Use the evaluator's `evaluationSize()` to ask whether the game supports two
  pot components.
- `shares(hero, villain)` returns the first hand's heads-up share for the
  supplied evaluations, including ties, scoops, and quartering. It enumerates
  no future cards.

## Low-level evaluation

`CardSet` provides the building blocks for game evaluators:

| Method | Rules |
| --- | --- |
| `evaluateHigh()` | Best high-poker hand from the supplied cards. |
| `evaluateLowA5()` | Ace-to-five lowball: aces low; straights and flushes ignored. |
| `evaluate8LowA5()` | A-5 low with an eight-or-better qualifier; zero when no low qualifies. |
| `evaluateLow2to7()` | Deuce-to-seven lowball: aces high; straights and flushes count against the hand. |
| `evaluate3CP()` | Three-card poker ordering. |
| `evaluateBadugi()` | Best subset of distinct ranks and suits, with low ranks preferred. |

Rank-only and flush/suit-only helpers support specialized optimizations; their
results are not complete game evaluations. Low-level routines do not enforce
pocket/board selection rules and some are unsafe for empty or oversized sets.
Use documented game sizes and validate input before evaluation; do not assume
all methods accept arbitrary subsets of the 52-card deck.

## Game evaluation

`<peval/PokerHandEvaluator.h>` defines the polymorphic game interface. Its
`alloc(id)` factory returns a `shared_ptr<PokerHandEvaluator>` or `nullptr` for
an empty/unsupported identifier. Always check the result before use.

The following IDs reflect the current factory implementation. Pocket and board
sizes are the completed sizes used by the enumerator:

| ID | Game | Pocket / board | Pot components |
| --- | --- | --- | --- |
| `h` | Hold'em high | 2 / 5 | 1 |
| `o`, `omaha` | Omaha high | 4 / 5 | 1 |
| `o8`, `o/8`, `omaha/8` | Omaha eight-or-better | 4 / 5 | 2 |
| `s` | Seven-card stud high | 7 / 0 | 1 |
| `r` | Razz (A-5) | 7 / 0 | 1 |
| `e` | Seven-card stud eight-or-better | 7 / 0 | 2 |
| `q` | Seven-card stud high/low without a qualifier | 7 / 0 | 2 |
| `d` | Five-card draw high | 5 / 0 | 1 |
| `k`, `t` | Five-card 2-7 lowball | 5 / 0 | 1 |
| `l` | Five-card A-5 lowball | 5 / 0 | 1 |
| `3` | Three-card poker | 3 / 0 | 1 |
| `b` | Badugi | 4 / 0 | 1 |

Identifiers are lowercased. Most non-Omaha games are selected only by the first
character; this is not strict full-name validation. In particular, `O` selects
Omaha high, and `T` selects 2-7, despite historical header comments assigning
those capitals other meanings. Use `l` for A-5. The accepted `o5`, `o6`,
`o5/8`, and `o6/8` IDs currently configure four-card pockets, so they do not
implement five/six-card Omaha correctly.

Hold'em evaluates the best five cards from pocket plus board, permitting any
number of pocket cards to play. Completed Omaha evaluation requires exactly
two of four pocket cards and three board cards; its specialized evaluator
expects four pocket cards and a board of three to five cards. `handSize()`,
`boardSize()`, and `evaluationSize()` describe the evaluator; they do not imply
that every method checks these input constraints.

```cpp
#include <iostream>
#include <stdexcept>
#include <peval/PokerHandEvaluator.h>

int main()
{
    using namespace pokerstove;
    auto evaluator = PokerHandEvaluator::alloc("h");
    if (!evaluator)
        throw std::runtime_error("Unsupported game");
    const CardSet board("2c3d4h5s9c");
    const auto aces = evaluator->evaluateHand(CardSet("AcAs"), board);
    const auto kings = evaluator->evaluateHand(CardSet("KhKd"), board);
    std::cout << aces.str() << '\n';
    std::cout << (aces.high() > kings.high()) << '\n'; // 1: aces make a straight
}
```

`evaluateHand()` returns the full result. `eval()` returns just the primary
component and loses split-pot information. `UniversalHandEvaluator` is a
configurable alternative using pocket/board bounds, a pocket-selection count,
and one or two `CardSet` evaluation functions; it is not the specialized fast
path for every game.

### Current hands versus equity

Evaluating a flop or partial hand describes only the cards currently supplied;
it does not calculate the probability of winning after future deals. For
exhaustive equity over missing cards, use `ShowdownEnumerator` from penum.

`evaluateShowdown(hands, board, evals, results, weight)` accumulates weighted
pot shares without clearing `results`. `evals` and `results` must have equal,
nonzero sizes; `hands` must contain at least that many hands. Validate disjoint
cards and game-specific counts before calling it. In split-pot games, the
whole pot goes to high when nobody qualifies for low; otherwise high and low
are awarded separately.

`evaluateEquity(c1, c2, board)` evaluates only the supplied showdown, with no
completions. It currently calls the defective `EquityResult::normalize()`;
for reliable normalization, use a fixed total as shown in the
[penum example](penum.md#calculating-equity). The existing payout lookup also
has a maximum index of 10: keep player count at most ten for single-component
games and at most five for split-pot games.

## Colexicographic indices

For a fixed card count `k`, `CardSet::colex()` maps each physical card set to a
unique zero-based index in `[0, choose(52, k))`. With ascending card codes
`c[0] ... c[k-1]`, it computes `sum(choose(c[i], i + 1))`.
`CardSet::fromColex(k, index)` reverses this mapping. Supply the same card count
and an in-range index; counts must not be mixed in one index space. The overload
with a deck size additionally requires a compatible card-code domain.

```cpp
const pokerstove::CardSet hand("AcAs");
const auto index = hand.colex();
const auto restored = pokerstove::CardSet::fromColex(2, index);
// restored == hand
```

`rankColex()` instead indexes rank multiplicities while ignoring suits; it does
not uniquely identify physical card sets. Colex indices and evaluation codes
serve different purposes. The public colex API returns `size_t`; large spaces
such as seven-card sets need a 64-bit platform. `choose` in
`<util/combinations.h>` uses checked `uint64_t` arithmetic, but that does not
widen a 32-bit `size_t` result.
