# Vendored dependencies

`googletest/` is the unchanged GoogleTest 1.14.0 source tree previously stored
under `src/ext/`. Its exact contents are pinned by this repository; updates must
be deliberate and reviewed separately. License: BSD-3-Clause, see
[googletest/LICENSE](googletest/LICENSE). CMake builds it only with
`BUILD_TESTING=ON` and does not install it with PokerStove.
