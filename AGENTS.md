# Agent guidelines

## Scope and project goals

These instructions apply to the entire repository unless a nested AGENTS.md
provides more specific guidance. Follow explicit task instructions first.

Modern PokerStove is a poker hand evaluation and equity enumeration library.
Prioritize correct poker rules, deterministic results, safe input handling,
and measured performance. Keep the core reusable independently of CLI tools
and Python bindings. Preserve upstream copyright and license notices.

## Current state and modernization

The repository requires C++20 and CMake 3.24. The layout below is implemented:
public headers under include/, implementations under src/, CLI programs under
apps/, tests under tests/, and SWIG bindings under bindings/python/. CLI parsing
uses vendored cxxopts; no active build requires Boost. Public includes have no
pokerstove/ prefix.

Modernize incrementally. Do not restructure the repository or remove
dependencies as a side effect of an unrelated fix. For an intentional move,
update CMake, includes, installation/export rules, Python packaging, CI,
and documentation together. The implemented layout intentionally changes public
include paths from <pokerstove/peval/CardSet.h> to <peval/CardSet.h> (and likewise
for penum and public utilities). Keep all consumers, examples, bindings, and installation rules consistent with
these paths.
Keep the C++ namespace pokerstove and existing target names unless the task
explicitly changes them. Prefer a separate move-only change before behavior changes.

## Directory layout

| Path | Responsibility |
| --- | --- |
| include/peval/ | Installed public evaluation headers. |
| include/penum/ | Installed public distribution and enumeration headers. |
| include/util/ | Utilities that are intentionally part of the public API. |
| src/peval/ | Evaluation implementation and private headers/tables. |
| src/penum/ | Distribution and enumeration implementation. |
| src/util/ | Internal shared utilities; keep this small and cohesive. |
| apps/ps-eval/, apps/ps-colex/, apps/ps-lut/ | CLI entry points and CLI-specific code. |
| tests/peval/, tests/penum/, tests/util/ | Unit and regression tests matching library components. |
| tests/integration/ | CLI, packaging, and cross-component tests. |
| tests/data/ | Small, deterministic test fixtures with documented provenance. |
| benchmarks/ | Reproducible performance workloads, separate from correctness tests. |
| bindings/python/ | SWIG interface, Python package code, and binding-specific tests. |
| cmake/ | Package configuration templates and reusable CMake modules. |
| docs/ | API, architecture, build, and development documentation. |
| examples/ | Small examples using installed public APIs. |
| tools/ | Maintainer scripts and lookup-table generators. |
| third_party/ | Explicitly vendored dependencies with licenses and pinned versions. |

Keep CMakeLists.txt, CMakePresets.json when introduced, README.md, LICENSE.txt,
AGENTS.md, formatting configuration, and pyproject.toml at the root as applicable.
Create directories only when they contain useful content.

Public headers must be self-contained. Private headers and generated evaluation
tables belong with their implementation unless consumers actually need them.
Do not expose vendor headers through the public API. Public includes use component
paths such as <peval/CardSet.h> and <penum/ShowdownEnumerator.h>; configure the
build and installation include root accordingly and preserve those component
subdirectories when installing headers. Do not add a pokerstove/ directory to
the target include tree. Keep utilities private unless consumers need them;
minimize public headers under the generic util/ path.

Use .cpp for implementations and .h for new headers to match the existing API;
do not mix extensions or rename existing headers without a specific reason.
Use focused lower_snake_case names for new internal files and *_test.cpp for
new tests. Preserve existing public class/header names during migration.

Build out of source under build/<configuration>/; keep generated sources in
the build tree, including SWIG outputs. Do not commit build products, downloaded
dependencies, local datasets, or CMakeUserPresets.json. Historical android/,
win32/, and doc/ files have been removed; README.md links to their upstream
locations. Keep current documentation under docs/ and do not reintroduce legacy
binaries.

## C++20 implementation guidelines

- When upgrading the build, require standard C++20 without compiler extensions.
  Do not silently introduce C++23 APIs such as std::expected, std::print, or
  std::string::contains.
- Prefer RAII and value semantics. Use std::unique_ptr for exclusive ownership;
  use std::shared_ptr only for genuinely shared ownership. Non-owning pointers,
  references, std::span, and std::string_view are appropriate when lifetimes
  are clear. Avoid owning raw pointers and manual new/delete.
- Prefer standard algorithms, containers, <bit>, <charconv>, and <chrono>.
  Use std::popcount and std::countr_zero with explicitly unsigned operands;
  account for their behavior on zero when replacing legacy bit helpers.
- Keep the compact card-mask representation. Validate card indices before
  shifts; use fixed-width unsigned types for masks, checked conversions for
  counts, and exact integer arithmetic for combination counts.
- Make headers self-contained with direct includes. Avoid namespace imports
  in headers. Use the pokerstove namespace; keep implementation-only helpers
  in anonymous namespaces or component-private detail namespaces.
- Use const and constexpr where they express real invariants. Apply noexcept
  only when all operations satisfy that guarantee. Use [[nodiscard]] for
  validation or computation results whose accidental omission is a bug.
- Follow checked-in formatting configuration. Until one exists, match the
  surrounding code and avoid unrelated formatting changes. Name new public
  APIs consistently with the existing library; document contracts and
  non-obvious poker rules rather than restating code.

## Correctness requirements

Validate external inputs before enumeration or evaluation: card syntax,
duplicate cards, supported game identifiers, game-specific card counts,
disjoint hands/boards, finite nonnegative weights, and available deck capacity.
Partial hands are allowed only according to the documented API contract.

Use exceptions or explicit checked results at API boundaries consistently;
assertions are for internal invariants, not release-build input validation.
Parsing should either succeed completely or leave a clearly defined state.
Define duplicate range-entry semantics and reject or combine them consistently.

Protect against unsigned subtraction underflow, invalid shifts, out-of-bounds
lookup access, and division by zero when no valid positive-weight outcome exists.
Keep normalization totals unchanged while assigning individual equities.
Do not assume a fixed maximum number of ties without validating that bound.

For split-pot games, test qualification, scoops, ties, and quartering. For Omaha,
enforce exactly two pocket cards and three board cards for completed hands.
Treat A-5 and 2-7 ace/straight/flush rules separately. Distinguish evaluation of
a current partial hand from equity over possible completions.

## Performance requirements

Optimize only with a representative workload and a correctness check.
Avoid repeated allocation, parsing, formatting, map lookups, and invariant
work inside enumeration loops. Prefer bounded stack storage for small fixed
candidate sets, cache pocket/board subsets when valid, skip zero-weight paths,
and prune card collisions early.

Enumerate set bits instead of scanning the whole deck when appropriate.
Avoid materializing every random hand when direct enumeration is sufficient.
Bound caches and document memory costs. Keep precomputed tables reproducible
with a generator and validation; do not hand-edit opaque table values.

Benchmarks should record build flags, compiler, hardware, seed, workload,
runtime, memory usage when relevant, and a result checksum. Compare equivalent
work under the same conditions. Do not claim a speedup without measurements.
For parallel enumeration, use local accumulators and deterministic merging;
document any floating-point reproducibility tradeoffs.

## Dependencies and build system

Prefer the standard library when it fully covers the requirement. Remove Boost
incrementally with behavior-preserving tests. Check parsing and formatting
semantics when replacing lexical_cast or format. Use std::format only when
the supported C++20 standard libraries implement it.

There is no standard CLI parser, test framework, or Python binding generator.
Keep GoogleTest for tests; isolate CLI parsing dependencies to apps and SWIG/
Python dependencies to bindings. Add dependencies only for a concrete benefit,
with a documented version/license and reproducible acquisition.

Use target-based CMake: target_compile_features(... PUBLIC cxx_std_20),
CXX_EXTENSIONS OFF, target_include_directories, target_link_libraries,
and target_compile_options. Avoid global compiler flags, include paths, and
link directories. Use imported targets and BUILD_INTERFACE/INSTALL_INTERFACE
for exported includes. Use GNUInstallDirs and install/export package targets.

When modernizing CMake, introduce BUILD_TESTING through include(CTest) and
project-specific options for apps, benchmarks, and Python bindings. Discover
optional dependencies only when their component is enabled. Tests must not be
required to build/install the core or a Python wheel. Document a supported
CMake minimum and compiler/standard-library versions; keep them consistent
across CMake, packaging, README, and CI.

## Validation and delivery

Use existing build instructions and options until modernization implements
replacements. The current default build can be checked with:

```sh
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release --parallel
ctest --test-dir build/release --output-on-failure
```

No component requires Boost. Vendored cxxopts is used only with
MODERN_POKERSTOVE_BUILD_APPS=ON. BUILD_TESTING controls the bundled tests and
MODERN_POKERSTOVE_BUILD_PYTHON controls the optional SWIG/Python bindings.
BUILD_WHEEL and BUILD_PYTHON remain supported packaging/legacy switches.
If dependencies or tools are unavailable, report the limitation instead of
claiming checks passed.

Add focused regression tests for behavior changes, including invalid inputs.
Use deterministic property/differential tests where useful: compare optimized
evaluation with best-five-card enumeration, and verify card removal, enumeration
counts, equity sum, player-order symmetry, and suit-permutation invariance.
Use independent known results as well as reference implementations.

For changes to masks, indexing, enumeration, or memory ownership, run relevant
tests under AddressSanitizer and UndefinedBehaviorSanitizer where supported.
Check Release builds too: validation must not depend on assert. Test Python
bindings or CLI integration when those interfaces change; verify installation
with a small external consumer when exported headers/targets move.

Keep changes focused. Summarize the behavior changed, tests actually run,
benchmark evidence for performance claims, and remaining limitations.
