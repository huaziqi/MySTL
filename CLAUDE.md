# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a learning project implementing STL data structures and algorithms from scratch in C++. It consists of two parts:

- **`MySTL/`** — the in-progress implementation being built (currently early-stage: only `TypeTraits.h`, `Alloc.h`, `memory.h` exist as stubs)
- **`TinySTL/`** — a reference implementation (submodule/copied repo) that serves as the target/inspiration

## Build System

The project uses Visual Studio 2022 (MSVC v143 toolset) on Windows. Build via:

```
# Open in Visual Studio
MySTL.sln         # builds MySTL project
TinySTL\TinySTL.sln  # builds TinySTL reference project
```

Or via MSBuild on the command line:
```powershell
msbuild MySTL.vcxproj /p:Configuration=Debug /p:Platform=x64
```

There is no CMake, no test runner CLI, and no lint tooling configured.

## Architecture

### TinySTL (reference implementation)

The layered dependency order is:

1. **`TypeTraits.h`** — compile-time type trait tags (`_true_type`/`_false_type`, `_type_traits<T>` specializations for POD types, `IfThenElse<bool,Ta,Tb>` for compile-time type selection)
2. **`Alloc.h` / `Detail/Alloc.cpp`** — two-level memory allocator: uses `malloc` for allocations >128 bytes, and a free-list pool (16 buckets, 8-byte aligned) for small allocations
3. **`Allocator.h`** — wraps `alloc` with an object-aware interface (`allocate`, `deallocate`, `construct`, `destroy`); this is the allocator passed to containers
4. **`Construct.h`**, **`UninitializedFunctions.h`** — placement-new helpers and uninitialized range operations (dispatch on `is_POD_type` for `memcpy` vs loop)
5. **`Iterator.h`**, **`ReverseIterator.h`** — iterator category tags and `reverse_iterator` adapter
6. **Container headers** (`Vector.h`, `List.h`, `Deque.h`, etc.) — each `#include`s its implementation from `Detail/<Name>.impl.h`

### MySTL (work in progress)

Mirrors the same layered structure but headers are mostly empty stubs. Implementation should follow the TinySTL pattern:
- `namespace MySTL` (vs `namespace TinySTL`)
- Same two-level allocator design
- Same `_type_traits` / `IfThenElse` pattern

### Testing pattern (TinySTL)

Each container has a paired test in `Test/<Name>Test.h` + `Test/<Name>Test.cpp`. Tests follow the pattern:

```cpp
namespace TinySTL {
    namespace XxxTest {
        void testAllCases();  // calls individual test functions
    }
}
```

`TestUtil.h` provides `container_equal(con1, con2)` and `print_container(con)` helpers. All tests are called from `main.cpp` sequentially — there is no test framework (no Google Test, no Catch2).

## Key Conventions

- Headers use `#ifndef` guards (TinySTL) or `#pragma once` (MySTL)
- Template implementations go in `Detail/<Name>.impl.h`, `#include`d at the bottom of the corresponding header
- The `alloc` class uses byte-level allocation; `allocator<T>` adds type safety and object lifetime management on top
- `IfThenElse<condition, TypeA, TypeB>::result` is the project's `std::conditional` equivalent, used to dispatch between POD-optimized and generic code paths
