# CURSE++ v0.2.1

[日本語の使い方 / Japanese guide](README.ja.md)

C++20 header-only *esoteric DSL* that abuses operator overloading to make source code lie to readers. **A joke/contest project; do not use for production business logic.**

## Compile the example without CMake

```sh
g++ -std=c++20 -Iinclude examples/fizzbuzz.cpp -o fizzbuzz
./fizzbuzz
```

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/cursepp_fizzbuzz
```

## Use from another project

Option A: copy `include/cursepp/cursepp.hpp` into your project and use `#include <cursepp/cursepp.hpp>` with `-I` pointing at the containing `include` directory.

Option B: in your CMake project:

```cmake
add_subdirectory(path/to/cursepp-library)
target_link_libraries(myapp PRIVATE cursepp::cursepp)
```

Option C: install, then `find_package`:

```sh
cmake -S . -B build -DCURSEPP_BUILD_TESTS=OFF -DCURSEPP_BUILD_EXAMPLES=OFF
cmake --install build --prefix "$PWD/local"
```

Consumer `CMakeLists.txt`:

```cmake
find_package(cursepp 0.2 CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE cursepp::cursepp)
```

Set `CMAKE_PREFIX_PATH=/absolute/path/to/cursepp-library/local` for the consumer if CMake cannot discover the package.

## Minimal usage

```cpp
#include <cursepp/cursepp.hpp>
using namespace cursepp;
using namespace cursepp::literals;
int main() {
    Z _;
    ~(_[{3_q, 1_q}] << (!_, *_)); // prints 1, 2, 3
}
```

`_q` literals are in `cursepp::literals`. Operator overloads and types are contained in `cursepp`, avoiding accidental namespace pollution. Non-member definitions are `inline`, so inclusion from multiple source files does not break linking.

An alternative to `~program` is `cursepp::run(program, output_stream)`, useful for testing or capturing output.

## Core types: `E`, `P`, `A`, `B`

The misleading operators apply to dedicated CURSE++ types, not to built-in `int` or `bool`.

| Type | Purpose | Example |
|---|---|---|
| `E` (*Expression*) | Lazily evaluated numeric expression using the current loop index | `3_q`, `_ + 1_q` |
| `P` (*Predicate*) | Lazily evaluated condition; not a built-in `bool` | `(_ ^ 3_q) != 0_q` |
| `A` (*Action*) | Deferred output/newline instruction | `*_`, `!_`, `(!_, *_)` |
| `B` (*Branches*) | Ordered list of guarded actions; first matching branch wins | `(p ->* a) \| (q ->* b)` |

`E` and `P` expose `at(i)`; `A` exposes `at(i, output_stream)`.
Normally, compose these objects with operators and execute the program using `~` or `run()`.
Additional types: `Z` is the implicit loop index, `R` a range, `K` a runnable loop program, and `W` a byte-reversed string wrapper.

## Notorious operators

| Appearance | Actually does |
|---|---|
| `E + E` | subtract |
| `E - E` | multiply |
| `E / E` | add |
| `E ^ E` | remainder |
| `++E`, `--E` | decrement/increment copies |
| `E != E` | equals |
| `P || P` | AND, no short-circuit when executed |
| `P ->* A` | guarded branch |
| `B \| B`, `B \| A` | accumulate branches / fallback |
| `*E`, `*W` | output lazy number / byte-reversed string |
| `!E` | newline |
| `A , A` | evaluate RHS instruction before LHS instruction |
| `Z[{last,first}]` | inclusive loop range |
| `R << A` | attach loop body |
| `~K` | execute to `std::cout` |

**Caveats:** Expressions are lazy; number operations use `long long` with its usual overflow limitations. String reversal is byte-based (use ASCII strings). Overloaded operators cannot change C++ parsing precedence, and overloaded `||` does not provide built-in short-circuit evaluation. This package prioritizes deliberate obfuscation over conventional readability.

See `examples/fizzbuzz.cpp` for the original 30-line FizzBuzz.

## License

MIT License. See [LICENSE](LICENSE). When redistributing, retain the copyright and permission notice. 
