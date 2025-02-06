# The Style Guide
This file covers essential style standards for different aspects of development,
namely **C++**, **CMake** and **Git**.

### Table of Contents

* [C++](#Cpp)
* CMake (coming soon...)
* Git (coming soon...)

<a name="Cpp"/>

### C++ 

**Topics covered**

* [1: Organize related data into structs](#CppStructsData)
* [2: Class single responsibility](#CppClassSingleResponsibility)
* [3: Naming: functions, classes and methods](#CppFuncClassMethodNames)
* [4: Naming: variables](#CppVariableNames)
* [5: Naming: namespaces](#CppNamespaceNames)
* [6: Naming: macros](#CppMacroNames)
* [7: How to deal with `{}`](#CppCurlyBracesPositioning)
* [8: Mark methods with `const` if possible](#CppConstMethods)
* [9: Always use `{}`](#CppUseCurlyBracesForBlocks)

<a name="CppStructsData"/>

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

<a name="CppClassSingleResponsibility"/>

### 2: A class has a single clearly defined responsibility

A class should only have **ONE** responsibility, for exapmle, an `std::vector`
just manages elements in the memory, it's not responsible (and it shouldn't be)
for printing them to console or serializing them into some kind of file. This is
a separate functionality provided by other classes.

<a name="CppFuncClassMethodNames"/>

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

<a name="CppVariableNames"/>

### 4: Variable names

Variables with automatic storage duration (including function and method
parameters) are named in `camelCase`. Variables with static storage duration are
named in `PascalCase` with an `s` prefix. Constant variables are named in
`PascalCase` with a `c` prefix. Private class fields are named in `PascalCase`
with an `m` prefix. Public class fields are named in `PascalCase`.

**Example**
```C++
const float cPi = 3.14f;
constexpr float cGravity = 10.0f;

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

<a name="CppNamespaceNames"/>

### 5: Namespace names

Namespace names are in `snake_case`.

**Example**
```C++
namespace uavpf
{
}
```

<a name="CppMacroNames"/>

### 6: Macro names

Macro names are in `MACRO_CASE`.

**Example**
```C++
#define UAVPF_ENABLE_DEBUG 1
#define UAVPF_DEBUG_ASSERT(condition, message) ::uavpf::DebugAssertImpl()
```

<a name="CppCurlyBracesPositioning"/>

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

<a name="CppConstMethods"/>

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

<a name="CppUseCurlyBracesForBlocks"/>

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
