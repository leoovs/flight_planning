# The Style Guide
This file covers essential style standards for different aspects of development,
namely **C++**, **CMake** and **Git**.

### Table of Contents
* [C++](#c)
    + [1: Organize related data into structs](#1-organize-related-data-into-structs)
    + [2: A class has a single clearly defined responsibility](#2-a-class-has-a-single-clearly-defined-responsibility)
    + [3: Function, class and method names are in `PascalCase`](#3-function--class-and-method-names-are-in--pascalcase-)
    + [4: Variable names](#4-variable-names)
    + [5: Namespace names](#5-namespace-names)
    + [6: Macro names](#6-macro-names)
    + [7: Curly braces positioning](#7-curly-braces-positioning)
    + [8: Mark methods with `const` where possible](#8-mark-methods-with--const--where-possible)
    + [9: Always use curly-braces](#9-always-use-curly-braces)
* [Git](#git)
    + [1: Commit message templates](#1-commit-message-templates)
    + [2: Branching convention](#2-branching-convention)
    + [3: Semantic versioning](#3-semantic-versioning)

## C++

### 1: Organize related data into structs

**Reason**

Easy to understand, if the data is related and grouped.

**Don't**
```C++
void DrawLine3D(float x0, float y0, float z0, float x1, float y1, float z1);
```

**Do**
```C++
struct Vector3
{
    float X;
    float Y;
    float Z;
};

void DrawLine3D(const Vector3& point1, const Vector3& point2);
```

### 2: A class has a single clearly defined responsibility

A class should only have **ONE** responsibility, for exapmle, an `std::vector`
just manages elements in the memory, it's not responsible (and it shouldn't be)
for printing them to console or serializing them into some kind of file. This is
a separate functionality provided by other classes.

### 3: Function, class and method names are in `PascalCase`

**Don't**
```C++
class car;
void buildRoute();

class graph_node
{
    graph_node get_parent();
};
```

**Do**
```C++
class Car;
void BuildRoute();

class GraphNode 
{
    GraphNode GetParent();
};
```

### 4: Variable names

Variables with automatic storage duration (including function and method
parameters) are named in `camelCase`. Variables with static storage duration are
named in `PascalCase` with an `s` prefix. Constant variables are named in
`PascalCase` with a `c` prefix. Private class fields are named in `PascalCase`
with an `m` prefix. Public class fields are named in `PascalCase`.

**Example**
```C++
const float cPi = 3.14f;

// Better
// constexpr float cPi = 3.14f;

int Counter()
{
    static int sCounter = 0;
    return ++sCounter;
}

class Button
{
public:
    EventCallback<void()> OnClick;

    void SetWidth(int width);

private:
    std::string mLabel = "Press Me";
};
```

### 5: Namespace names

Namespace names are in `snake_case`.

**Example**
```C++
namespace uavpf
{
}
```

### 6: Macro names

Macro names are in `MACRO_CASE`.

**Example**
```C++
#define UAVPF_ENABLE_DEBUG 1
#define UAVPF_DEBUG_ASSERT(condition, message) ::uavpf::DebugAssertImpl()
```

### 7: Curly braces positioning

There should always be a line break before and after the curly braces. This is
related to any kind of scope blocks.

**Don't**
```C++
void Empty() {}

int Add(int x, int y) {
    return x + y;
}

class Algorithm {
public:
    AlgorithmResult GetResult() { return AlgorithmResult(); }
};
```

**Do**
```C++
void Empty()
{
}

int Add(int x, int y)
{
    return x + y;
}

class Algorithm
{
public:
    AlgorithmResult GetResult()
    {
        return AlgorithmResult(); 
    }
};
```

### 8: Mark methods with `const` where possible

**Reason**

Easy to track which class methods have side effects and which do not.

**Don't**
```C++
class AlgorithmResult
{
public:
    int GetResultA()
    {
        return mResultA;
    }

    std::string& GetResultB()
    {
        return ResultB;
    }

private:
    int mResultA;
    std::string mResultB;
};
```

**Do**
```C++
class AlgorithmResult
{
public:
    int GetResultA() const
    {
        return mResultA;
    }

    // std::string_view is even better.
    const std::string& GetResultB() const
    {
        return mResultB;
    }

private:
    int mResultA;
    std::string mResultB;
};
```

### 9: Always use curly-braces

**Reason**

Encourages idiot-proof coding.

**Don't**
```C++
// To which `if` does the `else` belong to? First or second?
if (cond1)
    if (cond2)
        expr1;
    else
        expr2;

// And now?
if (cond1)
    if (cond2)
        expr1;
else
    expr2;

// Remember that C++ is not Python and indentation does not matter here.
```

**Do**
```C++
// Requires more typing but is clearly better.
if (cond1)
{
    if (cond2)
    {
        expr1;
    }
    else
    {
        expr2;
    }
}
```

## Git

### 1: Commit message templates

Use `fix: <describe bugfix>` template for bugfixes. You can also use an extended
message for like that:

```
fix: <describe bugfix>

- Fixed this by doing that.
- Fixed that by doing this.
```

Use `feat: <describe implemented feature>` for feature commits. In case large
changes made use extended message to describe the commit:

```
feat: implement <some feature>

- Add this.
- Add that.
```

### 2: Branching convention 

Branches `master` and `dev` are public and should be used as a reference point
for introducing new features. Thay can be modified through pull-request only.

The `dev` branch is used to stage newly implemented features and perform final
tests before making a big update for the `master`. The `master` branch should
stay as perfect, as clean as possible at all times.

In order to implement new features or fix bugs create local branches:
`features/<feature name>` for features and `fix/<bug name>` for bugfixes.

### 3: Semantic versioning

Top-level `CMakeLists.txt` contains `project()` command that specifies its
version. The versioning is done according to the
[*semver specification*](https://semver.org/).
