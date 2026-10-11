# Modern PokerStove

A C++20 modernization of [Andrew Prock's PokerStove](https://github.com/andrewprock/pokerstove),
a poker hand evaluation and equity enumeration library.

- **peval**: card representations and evaluators for poker variants.
- **penum**: weighted card distributions and exhaustive equity enumeration.
- **ps-eval**, **ps-colex**, **ps-lut**: optional command-line tools.
- **Python bindings**: optional SWIG-generated package.

## Requirements

- CMake 3.24 or newer.
- A compiler and standard library supporting C++20, including `<bit>`.
  Baseline toolchains: GCC 11+, Clang 14+ with a C++20 standard library,
  or Visual Studio 2022 (MSVC 19.30+).
- Boost 1.71+ headers. CLI tools also require Boost.Program_options.
- Python bindings additionally require SWIG 4.0+ and Python 3 development headers.

The core still uses Boost for parsing, formatting, and combinatorics. It does
not need compiled Boost libraries when CLI tools are disabled. GoogleTest is
bundled and is built only when tests are enabled. No dependencies are fetched
at configure time.

On Debian/Ubuntu, a default development build needs:

```sh
sudo apt-get install cmake g++ libboost-dev libboost-program-options-dev
```

On macOS, install a C++20-capable Xcode toolchain, CMake, and Boost (for example
through Homebrew). On Windows, use a compatible Boost installation and pass its
prefix through `CMAKE_PREFIX_PATH` if CMake cannot locate it automatically.

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
| `MODERN_POKERSTOVE_BUILD_APPS` | ON | Build CLI tools; requires Boost.Program_options. |
| `BUILD_TESTING` | ON | Build and register GoogleTest tests. |
| `MODERN_POKERSTOVE_BUILD_PYTHON` | OFF | Build SWIG/Python bindings. |
| `BUILD_SHARED_LIBS` | OFF | Build shared rather than static core libraries. |
| `BUILD_PYTHON` | OFF | Compatibility switch enabling Python bindings. |
| `BUILD_WHEEL` | OFF | Enable Python bindings; apps/tests default to OFF. |

For just the core, use `cmake --preset core`, then build that preset. No Python,
SWIG, GoogleTest, or compiled Boost library is required. For a shared-library
build, use the `shared` preset.

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
consumer with `-DCMAKE_PREFIX_PATH=/path/to/prefix`; also make the Boost headers
available. See `examples/cmake-consumer` for a complete example.

This first migration step preserves source locations and public includes such
as `<pokerstove/peval/CardSet.h>`. The planned layout removes the `pokerstove/`
include prefix in a separate migration; see [AGENTS.md](AGENTS.md).

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

## Migration and validation

This step establishes a standard C++20 build, target-based CMake, optional
components, installed package exports, and standard-library bit operations.
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
