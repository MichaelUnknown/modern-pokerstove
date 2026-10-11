# Modern PokerStove

A C++20 modernization of [Andrew Prock's PokerStove](https://github.com/andrewprock/pokerstove),
a poker hand evaluation and equity enumeration library.

- **peval**: card representations and evaluators for poker variants.
- **penum**: weighted card distributions and exhaustive equity enumeration.
- **ps-eval**, **ps-colex**, **ps-lut**: optional command-line tools.
- **Python bindings**: optional SWIG-generated package.

## Requirements

- CMake 3.24 or newer.
- A compiler and standard library supporting C++20, including `<bit>` and
  floating-point `std::from_chars`.
  Baseline toolchains: GCC 11+, Clang 14+ with a C++20 standard library,
  or Visual Studio 2022 (MSVC 19.30+).
- Git and network access for the first configure (or a populated CPM source cache).
- CPM.cmake 0.43.2 acquires PackageProject.cmake 1.13.0, optional cxxopts 3.3.1
  and GoogleTest 1.14.0 at pinned upstream commits; no system installation is needed.
- Python bindings additionally require SWIG 4.0+ and Python 3 development headers.

The project has no Boost dependency. Binomial counts use
exact, checked integer arithmetic; numeric parsing uses `std::from_chars`, and
formatting uses standard strings/streams. GoogleTest is downloaded and built only
when tests are enabled. PackageProject.cmake generates installable CMake packages.

On Debian/Ubuntu, a default development build needs:

```sh
sudo apt-get install cmake g++ git
```

On macOS and Windows, install a C++20-capable toolchain and CMake; the standard
library must support floating-point `std::from_chars`. The CLI parser
is acquired with its MIT license, and optional tests use GoogleTest.

## Build and test

```sh
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release --parallel
ctest --test-dir build/release --output-on-failure
```

CLI executables are under `build/release/bin/` (with an additional configuration
subdirectory for multi-configuration generators). For example:

```sh
build/release/bin/ps-eval AcAs KhKd --board 2c3d4h5s9c
```

| CMake option | Default | Purpose |
| --- | --- | --- |
| `MODERN_POKERSTOVE_BUILD_APPS` | ON | Build CLI tools using cxxopts. |
| `BUILD_TESTING` | ON | Build and register GoogleTest tests. |
| `MODERN_POKERSTOVE_BUILD_PYTHON` | OFF | Build SWIG/Python bindings. |
| `BUILD_SHARED_LIBS` | OFF | Build shared rather than static core libraries. |

Apps and tests default to OFF when this project is included with
`add_subdirectory`; Python bindings always default to OFF. For just the core:

```sh
cmake -S . -B build/core -DCMAKE_BUILD_TYPE=Release \
  -DMODERN_POKERSTOVE_BUILD_APPS=OFF -DBUILD_TESTING=OFF
cmake --build build/core --parallel
```

For shared libraries, set `-DBUILD_SHARED_LIBS=ON`. A Debug build uses
`-DCMAKE_BUILD_TYPE=Debug`. The previous presets and legacy `BUILD_PYTHON` /
`BUILD_WHEEL` switches have been removed; enable Python with the option above.

To share downloaded sources between builds, configure an absolute cache path:

```sh
export CPM_SOURCE_CACHE="$HOME/.cache/modern-pokerstove/cpm"
```

The bootstrap checks CPM's SHA256 and caches it alongside dependency sources.
The first configure downloads enabled dependencies; subsequent configurations
reuse the cache, including offline builds after it is fully populated. CPM also
supports local source overrides such as `-DCPM_cxxopts_SOURCE=/path/to/cxxopts`.
Never commit the cache. Dependency versions and licenses are recorded in
`cmake/dependencies.cmake` and the upstream sources (CPM/PackageProject/cxxopts:
MIT; GoogleTest: BSD-3-Clause).

## CLI compatibility

Existing long/short option names, defaults and positional hands remain supported.
`--help`, `-?`, and invocation without arguments print help and return status 1,
matching the previous tools. `ps-eval -h` remains the hand option. cxxopts changes
help/error wording; errors return status 1 rather than terminating uncaught.
Repeated `--hand` arguments and mixed named/positional hands preserve their order.
Commas within a hand argument remain distribution separators, not player splits.
The usual `--` separator ends option parsing. Scalar options may appear only once.

## Install and consume the C++ libraries

```sh
cmake --install build/release --prefix /path/to/prefix
```

PackageProject generates `peval`, `penum`, and the aggregate `pokerstove` package.
The aggregate imports `pokerstove::peval` and `pokerstove::penum` and also offers
`pokerstove::pokerstove` for both libraries. Component packages can be used with
`find_package(peval 1.2 CONFIG REQUIRED)` or `find_package(penum 1.2 CONFIG REQUIRED)`.
Installed consumers need no CPM or downloaded build dependencies. A consumer uses:

```cmake
find_package(pokerstove 1.2 CONFIG REQUIRED)
target_link_libraries(my_application PRIVATE pokerstove::penum)
```

This propagates C++20, include paths, and transitive dependencies. Configure the
consumer with `-DCMAKE_PREFIX_PATH=/path/to/prefix`. See
`examples/cmake-consumer` for a complete example.

Public headers now use component paths such as `<peval/CardSet.h>` and
`<penum/ShowdownEnumerator.h>`. This intentionally changes the previous
`<pokerstove/...>` include paths; update consumer includes accordingly.
The C++ namespace `pokerstove` and exported target names remain unchanged.

## Project layout

Public headers live in `include/{peval,penum,util}`; implementations and private
headers in `src/`; CLI tools in `apps/`; component tests in `tests/`; Python
bindings in `bindings/python/`; build and dependency modules in `cmake/`.
Documentation is in `docs/`, and installed API examples in `examples/`.
See the [peval API guide](docs/peval.md) for card representations and game
evaluation, and the [penum API guide](docs/penum.md) for weighted distributions,
exhaustive enumeration, and equity calculation.
See [AGENTS.md](AGENTS.md) for directory responsibilities and guidelines.
Only `include/` is installed as the C++ header tree. Odometer and the unused
timing helper are internal. `PokerEvaluationTables.h` remains public because
inline Omaha evaluators currently depend on it; hiding it needs a separate
implementation refactor.

## Python bindings

Install SWIG and development headers, then:

```sh
cmake -S . -B build/python -DCMAKE_BUILD_TYPE=Release \
  -DMODERN_POKERSTOVE_BUILD_PYTHON=ON -DMODERN_POKERSTOVE_BUILD_APPS=OFF
cmake --build build/python --parallel
ctest --test-dir build/python --output-on-failure
PYTHONPATH=build/python/python python3 -c 'import pokerstove; print(pokerstove.CardSet("AcAs"))'
```

The build-tree package is under `build/python/python/pokerstove/`; generated
wrappers stay out of the source tree. This configuration disables CLI tools.
On Unix, the extension may use shared core libraries for a build-tree workflow;
for a portable wheel, packaging explicitly builds static core libraries.

To build and install a wheel:

```sh
python3 -m pip wheel . --no-deps --wheel-dir dist
python3 -m pip install dist/pokerstove-*.whl
```

The packaging backend installs only the `python` component, including the
extension, wrapper, and package initializer. C++ headers and CLI tools are not
included in the wheel.

## Distribution parsing

`CardDistribution::parse` accepts comma-separated raw hands with optional
nonnegative, finite decimal/scientific weights, such as
`AcAs=+0.125,KhKd=1e-3`. Numeric syntax uses an ASCII decimal point and requires
complete consumption without whitespace. Invalid syntax, nonfinite/negative
weights, and numeric overflow/underflow return `false` and preserve the previous
distribution. `.` represents the random distribution. Duplicate range-entry
semantics remain unchanged and will be addressed separately.

## Migration and validation

The implemented steps establish a C++20 build, target-based CMake, optional
components, installed package exports, standard-library bit operations, and
the modern project layout.
It does not change poker rules or address all correctness findings in upstream.
The next stages are described in [docs/modernization.md](docs/modernization.md).

When changing bit operations or enumeration, use a separate sanitizer build:

```sh
cmake -S . -B build/sanitizers -DCMAKE_BUILD_TYPE=Debug \
  -DMODERN_POKERSTOVE_BUILD_APPS=OFF \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build/sanitizers --parallel
ctest --test-dir build/sanitizers --output-on-failure
```

Sanitizer flags above apply to GCC/Clang on supported platforms. Existing
upstream copyright/license notices are preserved.

## Legacy files

The historical Windows installer (`win32/PokerStoveSetup124.exe`) and Android
demo (`android/PokerStoveDemo.apk`) have been removed from this fork. They remain
available in the upstream repository:

- [PokerStove Windows installer](https://github.com/andrewprock/pokerstove/blob/master/win32/PokerStoveSetup124.exe)
- [PokerStove Android demo](https://github.com/andrewprock/pokerstove/blob/master/android/PokerStoveDemo.apk)

These are upstream binaries, not builds of Modern PokerStove. The legacy
AsciiDoc `doc/` directory has also been removed; its historical design notes
remain in [upstream `doc/`](https://github.com/andrewprock/pokerstove/tree/master/doc).
Current documentation lives in `docs/`.
