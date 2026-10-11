# Incremental C++20 modernization

## Step 1: C++20 build foundation (implemented)

- Require C++20 without compiler extensions and CMake 3.24.
- Use explicit source lists and target-scoped dependencies and includes.
- Separate core libraries, applications, tests, and Python bindings.
- Export installable CMake targets with transitive Boost headers and C++20.
- Replace platform-specific bit-index implementations with `<bit>` while
  preserving the legacy zero-mask convention; use `std::popcount` for card counts.
- Use std::make_shared in the evaluator factory and remove two unused, leaked
  Omaha evaluator allocations.
- Keep existing target names, public include paths, and poker behavior.
- Check Release/shared builds, sanitizer tests, installed CMake consumers,
  Python imports, and wheel packaging.

## Step 2: Project layout and public includes (implemented)

- Separate public headers in `include/` from implementation in `src/`.
- Move CLI tools to `apps/`, unit tests to `tests/`, Python bindings to
  `bindings/python/`, and unchanged vendored GoogleTest to `third_party/`.
- Consolidate documentation in `docs/`.
- Migrate includes, CMake installation/export, SWIG, source distribution, and
  installed consumer examples together.
- Use `<peval/...>`, `<penum/...>`, and `<util/...>` instead of the old
  `<pokerstove/...>` paths. Consumers must update includes; namespaces and CMake
  target names are unchanged. Odometer and the unused timing helper are private.
- Keep evaluation tables public for now because inline public Omaha evaluators
  use them. Making them private requires moving those implementations separately.
- Preserve poker behavior and dependency versions.

## Step 3a: Standard-library combinatorics and integer text (implemented)

- Replace Boost.Math binomial calculation and its floating-point/size_t round
  trip with a checked `uint64_t` recurrence using `std::gcd` cancellation.
- `choose(int, int)` now returns `std::uint64_t` rather than `double`. Existing
  callers can still convert to double, but callers depending on the exact return
  type (for example function pointers) must update. Colex counts avoid the
  floating-point round trip. The public Colex API still returns size_t; only the
  binomial calculation is independent of its width.
- Impossible selections (`k > n`) still return zero. Negative inputs throw
  `std::invalid_argument`; counts beyond uint64_t throw `std::overflow_error`.
- Replace integer lexical_cast usage in combinations, partition diagnostics,
  and UniversalHandEvaluator diagnostics with `std::to_string`. Preserve text
  spacing and existing error messages. Remove unused Boost includes in CardSet.
- Replace assertion-free timing loops with known counts, an independent Pascal
  triangle reference, overflow tests, text checks, and Colex regression tests.
- Boost remains required for other public/core parsing and floating-point text,
  and for CLI Program_options. No speedup is claimed for this change.

## Next steps

These are separate changes, not prerequisites for using the C++20 build:

1. Remove remaining Boost utilities incrementally. Next address checked standard
   numeric parsing and compatible formatting; exact integer combinatorics is implemented.
   Keep CLI parsing and tests independently scoped. Verify parsing and output
   semantics before replacing them.
2. Fix upstream correctness issues in separate tested changes: split-pot tie
   bounds, A-5 paired-hand evaluation, normalization, evaluator selection,
   card removal, duplicate range handling, and invalid input validation.
3. Add reproducible benchmarks, then optimize allocation, subset caching,
   collision pruning, and random-hand enumeration based on measurements.

C++20 compilation alone does not fix the upstream poker correctness issues.
No performance improvement is claimed merely from replacing the bit helpers.
