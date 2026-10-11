# Vendored dependencies

`googletest/` is the unchanged GoogleTest 1.14.0 source tree previously stored
under `src/ext/`. Its exact contents are pinned by this repository; updates must
be deliberate and reviewed separately. License: BSD-3-Clause, see
[googletest/LICENSE](googletest/LICENSE). CMake builds it only with
`BUILD_TESTING=ON` and does not install it with PokerStove.

`cxxopts/` contains the unmodified cxxopts 3.3.1 single header and MIT license
from https://github.com/jarro2783/cxxopts at commit
`44380e5a44706ab7347f400698c703eb2a196202` (tag `v3.3.1`). The upstream header's
Git blob SHA is `32c8f2ce787fb2209fad9721cccc443e4aa98cfd`.
The local CMake wrapper exposes `cxxopts::cxxopts` privately to CLI targets.
Its MIT license is installed with the applications component. Its header is
not installed/exported with the core, and it is needed only with `MODERN_POKERSTOVE_BUILD_APPS=ON`. Updates must retain the
upstream license and record the new release/commit and header checksum.
