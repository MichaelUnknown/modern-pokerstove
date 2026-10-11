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
- This was the first part of the dependency migration; Step 3b below completes
  core parsing and formatting. No speedup is claimed.

## Step 3b: Core without Boost (implemented)

- Replace weighted distribution numeric parsing with checked floating-point
  `std::from_chars`, preserving optional leading plus, decimal/scientific input,
  default weights, and the random distribution marker.
- Invalid, nonfinite, negative, out-of-range, or partially parsed weights return
  false rather than leaking Boost exceptions. Parsing is transactional: failure
  preserves the previous distribution. Numeric syntax is locale-independent.
- Replace comma splitting/substring searches with string_view, and parse each
  card from exactly two characters. Duplicate range semantics remain unchanged.
- Preserve distribution fixed-three-digit text, equity max_digits10 precision,
  evaluation padding, CLI table widths, and lookup-generator text using standard
  strings and streams. Formatting still uses the current locale.
- Replace factory case conversion with standard locale-aware transformation;
  empty/unsupported identifiers safely return nullptr. Existing game selection
  rules (including upstream o5/o6 and T issues) remain for separate fixes.
- Replace the unused internal Boost timer helper with a steady_clock sample.
- Remove Boost from core target exports, installed package discovery, public
  headers, Python builds and CI core requirements. Only CLI Program_options
  remains; there is no equivalent parser in the C++20 standard library.
- Floating-point from_chars support is required from the compiler's standard
  library; std::format is not required. No performance claim is made.

## Step 3c: cxxopts CLI parsing and complete Boost removal (implemented)

- Replace Boost.Program_options in all three CLI tools with unchanged vendored
  cxxopts 3.3.1, pinned by commit/header SHA with its MIT license.
- Keep option names/defaults, positional hands, -- separator and the legacy -?
  help alias. Help formatting/error wording now follow cxxopts; help/no-argument
  invocation retains exit status 1. Repeated hands retain their original tokens,
  including weighted comma-separated distributions and their order.
- Reject unmatched/duplicate scalar arguments, unsafe card counts, missing hands,
  invalid hand distributions and unsupported games with handled errors.
- Link cxxopts privately to CLI targets only. Remove Boost discovery, linking
  and installation requirements from all active CMake/CI configuration.
- Cover CLI behavior with CTest integration tests, enabled only for apps+tests.
  Core, Python packaging and installed C++ consumers acquire no CLI dependency.
- Historical android/win32 artifacts and modernization history are preserved.

## Next steps

These are separate changes, not prerequisites for using the C++20 build:

1. Fix upstream correctness issues in separate tested changes: split-pot tie
   bounds, A-5 paired-hand evaluation, normalization, evaluator selection,
   card removal, duplicate range handling, and invalid input validation.
2. Add reproducible benchmarks, then optimize allocation, subset caching,
   collision pruning, and random-hand enumeration based on measurements.

C++20 compilation alone does not fix the upstream poker correctness issues.
No performance improvement is claimed merely from replacing the bit helpers.
