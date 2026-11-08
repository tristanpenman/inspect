# C++

These conventions apply to project-owned C++ and CUDA code. Third-party code should not be reformatted merely to match this document. At an integration boundary, retain the spelling and idioms of the external API, but use the project conventions inside project-owned wrappers and implementations.

When modifying older code, apply these conventions to the code being changed when doing so is low risk. Avoid unrelated, file-wide formatting or renaming changes.

## Core Guidelines

### Code Style

- Use `.h` for headers and the source extension already established by the project (`.cc` or `.cpp`) for implementations.
- Indent with four spaces. Do not use tabs.
- Put opening braces on the next line for functions, classes, structs, unions, and enums.
- Put opening braces on the same line for namespaces and control-flow statements.
- Always use braces for the body of `if`, `else`, `for`, `while`, and similar statements, including single-statement bodies.
- Put a space on both sides of the colon in a range-based `for` loop.
- Bind pointer and reference markers to the type: `Widget* widget`, `const Vector& value`.
- Keep short getters and setters in the normal multi-line form. An inline one-line definition is acceptable when it materially improves the readability of a long, repetitive class.
- For a multi-line function declaration or definition, indent each parameter by four spaces and keep the closing parenthesis with the opening brace.
- Put each member on its own line in a multi-member constructor initializer list.
- Follow the established line-ending and maximum-line-length settings of the repository.

```cpp
Result calculateResult(
    const Input& input,
    std::size_t sampleCount)
{
    if (sampleCount == 0) {
        return {};
    }

    for (const auto& sample : input.samples()) {
        // Process the sample.
    }

    return makeResult(input, sampleCount);
}
```

### Naming

- Classes, structs, unions, type aliases, and enum types use `PascalCase`.
- Functions and methods use `camelCase`.
- Local variables, parameters, and public struct fields use `camelCase`.
- Non-static data members use `camelCase` with a trailing underscore: `byteCount_`.
- Mutable global and namespace-scope variables use `camelCase` with a `g_` prefix: `g_shutdownRequested`. This includes variables in anonymous namespaces and `thread_local` variables because their storage duration is not apparent at the point of use. Prefer avoiding mutable global state when ownership or dependency injection is practical.
- Constants, `constexpr` values, and enumerators use `kPascalCase`: `kBufferSize`, `kInvalidArgument`.
- Preprocessor macros use `ALL_CAPS`. Prefer typed constants and functions to macros.
- Template parameters use `PascalCase`: `ValueType` or `T`.

Do not rename symbols owned by an external library or operating-system API. Where a project must implement a prescribed callback or interface, retain its required name. Project wrappers around such APIs use the normal project naming rules.

Legacy modules that consistently use `snake_case`, `m_` member prefixes, unprefixed PascalCase enumerators, or `ALL_CAPS` constants may retain that style until they are deliberately migrated. Do not mix competing styles within one class or closely related module.

### Headers and includes

- Use `#pragma once` in project headers.
- Include what the file directly uses.
- Group includes in this order, with one blank line between groups:
  1. The corresponding header, in an implementation file.
  2. C and C++ standard-library headers.
  3. Third-party and platform headers.
  4. Project headers.
- Sort includes consistently within each group.
- Prefer a forward declaration when it is valid and makes the dependency clearer. Include the owning header when a complete type or a library-provided smart-pointer declaration is required.
- Keep platform-specific includes out of portable public headers where practical.

### Namespaces

- Use an anonymous namespace for implementation details confined to one translation unit.
- Add a namespace name to the closing-brace comment. Use `// namespace` for an anonymous namespace.
- Do not use `using namespace` in a header. Use it sparingly in a narrow implementation scope.
- Do not indent the contents of a namespace.
- Include an empty line after opening a namespace and before closing one.

```cpp
namespace {

int helper()
{
    return 1;
}

}  // namespace

namespace example {

// ...

}  // namespace example
```

### Types, ownership, and interfaces

- Prefer fixed-width integer types such as `std::int32_t` where width is part of the interface or data format. Otherwise use the natural type for the operation, such as `std::size_t` for sizes.
- Pass small, inexpensive value types by value. Pass larger read-only objects by `const&` and mutable non-owning objects by reference or pointer as appropriate.
- Use `std::unique_ptr` for exclusive ownership and `std::shared_ptr` only for genuine shared ownership.
- Pass smart pointers by value when transferring or sharing ownership. Do not use a smart pointer merely to express non-owning access.
- Prefer constructor injection when an object requires a dependency for its valid lifetime.
- Mark read-only methods `const` and use `const` for values that do not change.
- Use `auto` when the type is obvious from the initializer or when spelling the type would obscure the intent. Avoid it when the concrete type conveys important ownership, precision, or conversion information.
- Initialise members at their declarations when the same default applies to every constructor.
- Use scoped enums (`enum class`) unless implicit conversion or compatibility with an existing interface is required.

### Comments and documentation

- Write comments that explain intent, constraints, or non-obvious behaviour rather than restating the code.
- Use sentence case and punctuation for prose comments.
- Avoid full stops for short annotations and non-prose comments.
- Documentation comments are appropriate for public types and APIs whose contract is not evident from the declaration.
- Section comments may be used to make a long file easier to navigate, but should not compensate for an overly large class or function.

### Example

```cpp
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "project/bar.h"

namespace example {

/** Processes values using the supplied service. */
class Foo
{
public:
    enum class Status
    {
        kReady,
        kFailed
    };

    explicit Foo(std::unique_ptr<Bar> bar);

    std::int32_t count() const
    {
        auto total = kStartValue;
        if (enabled_) {
            for (const auto value : values_) {
                total += value;
            }
        }

        return total;
    }

private:
    std::unique_ptr<Bar> bar_;
    bool enabled_ = true;
    std::vector<std::int32_t> values_;

    static constexpr std::int32_t kStartValue = 3;
};

}  // namespace example
```

## Integrations

### CUDA

CUDA code follows the C++ conventions above, with these additions:

- Use `.cu` for CUDA implementations and `.cuh` only for headers that expose CUDA-specific declarations.
- Name kernels with a `Kernel` suffix, for example `softmaxGradientKernel`.
- Prefix variables that refer to a particular memory space with `host`, `dev`, or `shm`, for example `hostInput`, `devOutput`, and `shmTile`. A prefix is unnecessary when the memory space is already unambiguous from a narrow scope or wrapper type.
- Place execution-space specifiers before the return type: `__global__ void`, `__host__ __device__ float`.
- Check CUDA runtime and library errors at API boundaries and after kernel launches where an error can be acted upon.
- Avoid unnecessary `cudaDeviceSynchronize()` calls. Use synchronization only when required for correctness, timing, host access, or debug-only error localisation.
- Keep kernel launches and device-specific details behind project-owned functions where this improves testability and keeps portable headers free of CUDA dependencies.
- Use explicit names for dimensions and indices. Guard accesses that can fall outside the logical data extent.

```cpp
template<typename T>
__global__ void sigmoidKernel(T* devValues, int valueCount)
{
    const int index = blockIdx.x * blockDim.x + threadIdx.x;
    if (index < valueCount) {
        devValues[index] = T{1} / (T{1} + exp(-devValues[index]));
    }
}
```

### Oxygine

These rules apply when Oxygine is used:

- Treat the modified engine directory as upstream-style code: match its local formatting and API shape when changing it.
- Game code should use the project C++ conventions and modern C++ facilities where they interoperate safely with the engine.
- Use Oxygine's `DECLARE_SMART(Type, spType)` and `spType` intrusive pointers for `oxygine::Object`- and `oxygine::Actor`-derived objects.
- Put `DECLARE_SMART` in the owning header immediately before the class declaration where practical.
- Include the owning header when a public declaration exposes its `spType`. Do not repeat `DECLARE_SMART` in consumer headers solely to enable a forward declaration unless compile-time requirements justify it.
- Use `std::unique_ptr` for exclusively owned non-Oxygine services and `std::shared_ptr` only for non-Oxygine shared ownership.
- Preserve Oxygine API conventions such as `init`, `update`, `getX`, `setX`, `DECLARE_SMART`, and `spType` in engine-facing code.
- Raw pointers are acceptable for non-owning engine callbacks when required by the Oxygine API. Make ownership clear at the boundary.
- Follow the surrounding subsystem for engine-owned event and flag names; project-owned values use `kPascalCase`.

### Box2D

These rules apply when Box2D is used:

- Preserve the names and types of the Box2D version in use, including its `b2` type prefixes and any PascalCase methods or C API function names.
- Do not wrap Box2D solely to change its naming style. Project-owned physics abstractions and callbacks use the normal project conventions unless Box2D prescribes their signatures.
- Treat Box2D object pointers and handles according to the lifetime rules of the version in use. Make non-owning references apparent and do not place them in owning smart pointers.
- Keep unit conversion and coordinate-system conversion at clear boundaries. Name converted values explicitly when units are not obvious.
- Avoid changing the physics world while it is locked or from callbacks when the API prohibits it; queue the project operation for a safe point instead.

### Dear ImGui

These rules apply when Dear ImGui is used:

- Preserve Dear ImGui names such as `ImGui::Begin`, `ImVec2`, `ImGuiID`, and `ImGuiWindowFlags`.
- Project-owned UI functions, state, and wrapper types use the normal C++ naming rules.
- Keep each successful `Begin`/`BeginChild` paired with its required `End`/`EndChild`, following the contract of the Dear ImGui version in use. Pair other push/pop and begin/end APIs on every control-flow path.
- Use stable, unique widget IDs. Use `##` or `###` suffixes when a visible label is not a sufficient identity.
- Do not retain pointers or references to transient ImGui data beyond the lifetime documented by the API.
- Keep persistent UI state in an owning application object rather than in unrelated global variables.
- Isolate optional backend-specific code from application UI code where practical.

### Win32

These rules apply to code that calls the Windows API:

- Preserve Win32 names and signatures, including types such as `HWND`, `WPARAM`, and `LRESULT`, constants such as `WM_PAINT`, and prescribed callbacks such as `WndProc`.
- Project-owned wrappers, functions, and variables use the normal C++ naming rules; do not introduce Hungarian notation for them.
- Use the wide-character API explicitly (`...W`) and UTF-16 conversions at the platform boundary unless an existing project has a documented alternative.
- Check API-specific failure values and preserve `GetLastError()` before calling other Win32 functions when it is needed for diagnostics.
- Manage owned handles with an appropriate RAII wrapper. Do not call `CloseHandle` for handles whose API requires a different release function, and do not release borrowed handles.
- Keep Windows headers and macros behind a platform layer where practical. Define `NOMINMAX` before including `<windows.h>` when needed, rather than working around `min` and `max` macros throughout the codebase.
- Use fixed-width types for serialized or cross-platform data; use Win32 types where the operating-system ABI requires them.
- Keep message handling concise and delegate application work to project-owned functions. Return `DefWindowProcW` for messages not handled by the application.

### Qt

- Use Qt types at Qt API boundaries and standard-library types in platform-independent code. Convert between them at the boundary.
- Use `QStringLiteral` for fixed strings and `tr()` or `QCoreApplication::translate()` for translatable user-visible text.
- Parent heap-allocated `QObject`s where practical. Store parent-owned objects as non-owning raw pointers and do not delete them manually.
- Keep GUI objects on the GUI thread; use queued signals and slots across threads.
- Add `Q_OBJECT` only when meta-object features are required.
- Prefer compiler-checked function-pointer or lambda connections. Give capturing lambdas a context object.
- Include Qt types used by value or as base classes; forward-declare types used only through pointers or references.
- List `Q_OBJECT` headers in the CMake target and explicitly find and link every Qt module used.
- Preserve Qt-prescribed names for overrides and interfaces; use project naming conventions for project-defined Qt APIs.
- Prefer cross-platform Qt APIs and isolate platform-specific integration.
