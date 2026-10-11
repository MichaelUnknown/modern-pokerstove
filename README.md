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
- CLI tools use bundled cxxopts 3.3.1; no system installation is needed.
- Python bindings additionally require SWIG 4.0+ and Python 3 development headers.

The project has no Boost dependency. Binomial counts use
exact, checked integer arithmetic; numeric parsing uses `std::from_chars`, and
formatting uses standard strings/streams. GoogleTest is bundled and built only
when tests are enabled. No dependencies are fetched at configure time.

On Debian/Ubuntu, a default development build needs:

```sh
sudo apt-get install cmake g++
```

On macOS and Windows, install a C++20-capable toolchain and CMake; the standard
library must support floating-point `std::from_chars`. The CLI parser is bundled
with its MIT license, and optional tests use bundled GoogleTest.

## Build and test

```sh
cmake --preset release
cmake --build --preset release --parallel
ctest --preset release
```

A `debug` preset is also provided. Both enable CLI tools and tests. Equivalent
commands without presets:

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
| `MODERN_POKERSTOVE_BUILD_APPS` | ON | Build CLI tools using bundled cxxopts. |
| `BUILD_TESTING` | ON | Build and register GoogleTest tests. |
| `MODERN_POKERSTOVE_BUILD_PYTHON` | OFF | Build SWIG/Python bindings. |
| `BUILD_SHARED_LIBS` | OFF | Build shared rather than static core libraries. |
| `BUILD_PYTHON` | OFF | Compatibility switch enabling Python bindings. |
| `BUILD_WHEEL` | OFF | Enable Python bindings; apps/tests default to OFF. |

For just the core, use `cmake --preset core`, then build that preset. No Python,
SWIG, GoogleTest, or Boost is required. For a shared-library
build, use the `shared` preset.

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

The package exports `pokerstove::peval` and `pokerstove::penum`. A consumer uses:

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
bindings in `bindings/python/`; vendored GoogleTest in `third_party/googletest/`.
Documentation is in `docs/`, and installed API examples in `examples/`.
See [AGENTS.md](AGENTS.md) for directory responsibilities and guidelines.
Only `include/` is installed as the C++ header tree. Odometer and the unused
timing helper are internal. `PokerEvaluationTables.h` remains public because
inline Omaha evaluators currently depend on it; hiding it needs a separate
implementation refactor.

## Python bindings

Install SWIG and development headers, then:

```sh
cmake --preset python
cmake --build --preset python --parallel
ctest --preset python
PYTHONPATH=build/python/python python3 -c 'import pokerstove; print(pokerstove.CardSet("AcAs"))'
```

The build-tree package is under `build/python/python/pokerstove/`; generated
wrappers stay out of the source tree. The Python preset disables CLI tools.
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
upstream copyright/license notices and historical `android/` and `win32/`
artifacts are preserved.
