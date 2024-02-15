Zen C++ Coding Style
====================

This document outlines the coding style that is used thoughout the Zen++
libraries.

⚠️ These conventions are not yet mature enough to be used in serious projects. Expect
many breaking changes. If you plan to use them, know that you might have to
refactor your code.

This guide is primarily targeted at people who are staring a fresh project that
makes use of the Zen++ libraries and developers wishing to contribute to the
Zen++ project. Existing projects that wish to make use of these libraries
should ignore this document and instead follow the coding style of their project.

If you're planning to contribute to the Zen++ project, please take some time to
skim through this document. Code that does not follow the guidelines may be
rejected.

## General

### Always use the correct naming convention

 - Macro names should be `ALL_CAPS`.
 - Functions and variables should be `snake_case`
 - Types, classes and concepts should be `CamelCase`

**Motivation:** `CamelCase` is used in many programming languages for types and
classes. Concepts are closely related to types and functions so they should use
the same casing. Since functions can also be variables and should not be
confused with type constructors, they both have the same casing. `snake_case`
is ideal because it interoperates well with C. Almost all C/C++ code uses
`ALL_CAPS` for macros, so people are already used to it. Above that, it clearly
indicates that a macro call is not a function and may generate arbitrary code
with potential side-effects.

### Prefix non-member functions that _construct_ a specific type with `make_`

This is the same convention that is used in the standard library. Function
names such as `make_unique` and `make_shared` remind the user that a new type
will be constructed, and that the caller is responsible for its ownership.

## Template metaprogramming

### Use `CamelCase` for template parameters

**Motivation:** type variables are substituted for regular types, so they exist
on roughly the same level as types, classes and concepts. Using the same
convention for type variables ensures that developers can easily spot them.

### Template metafunctions should be suffixed with `_`

In order to distinguish metafunctions from other types and from plain
functions, the suffix `_` should be added to the name of the metafunction. For
example, a metafunction that calculates the size of a type list should
be called `size_` and not `size`, `size_t`, or `size_type`.

This rule is meant to make a seperate namespace (e.g. `zen::meta`) obsolete,
leading to more concise code. It also redcues name collisions, e.g. when out of
frustration the user would write something like `using namespace zen::meta`.
Above that, the suffix is a reminder that the function operates on the type
level.

The rules for determining whether a given type is a metafunction are simple. If
the template may be used as the argument to another metafunction, then that
template is also a metafunction. If instanciating the template results in a
type that may appear in class members, function arguments or return types, then
that template is **not** a metafunction.

More generally, a metafunction is a template that results in a structure of
which all members can be inspected at compile-time and has no use at run-time.
The members of the resulting type usually hold information about how other
metafunctions should operate on it.

### A public template metafunction that returns a type should have a type alias suffixed with `_t`

The type alias should resolve to the result of the compile-time computation. For example,
a template `foo_` with one type parameter `T` should have the following declaration:

```cpp
template<typenme T>
using foo_t = typename foo_<T>::type;
```

This rule is a convenience that is also present in the standard library. Those
who write libraries are required to do some more work, while the user can
write the shorter `foo_t<T>` instead of `typename foo_<T>::type`.

### Don't use non-type template parameters in public template metafunctions

User-facing metafunctions should operate solely on types. The only exception to
this rule are functions that lift a primitive value to a type.

The reason that this rule exists is because it makes the template
metaprogramming API more stable and future-proof. When a certain metafunction
only accepts boolean values, changing it to accept a type requires adjusting
all code that relies on it, whereas a type can in principle hold any value.

The following code is incorrect:

```cpp
template<std::size_t N, typename SeqT>
struct nth_;
```

The previous example should be replaced with the following code:

```cpp
template<typename N, typename SeqT>
struct nth_;
```

When end-users or developers of other components of Zen++ encounter a `_helper`
template metafunction, they should know that the function is not meant to be
used or extended.

### Use `_impl_` as a suffix for metafunctions that may be extended by the end-user

This might sound unproductive, but the extra indirection effectively decouples
the interface from the actual implementation. This allows us to add additional
logic in the public API (such as validity checks) while leaving third-party
code unaffected.


