# The Style Guide
This file covers essential style standards for different aspects of development,
namely **C++**, **CMake** and **Git**.

### Table of Contents

* [C++](#Cpp)
* CMake (coming soon...)
* Git (coming soon...)

### C++ <a name="Cpp"></a>

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

### 1: Organize related data into structs <a name="CppStructsData"></a>

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

### 2: A class has a single clearly defined responsibility <a name="CppClassSingleResponsibility"></a>

A class should only have **ONE** responsibility, for exapmle, an `std::vector`
just manages elements in the memory, it's not responsible (and it shouldn't be)
for printing them to console or serializing them into some kind of file. This is
a separate functionality provided by other classes.

### 3: Function, class and method names are in `PascalCase` <a name="CppFuncClassMethodNames"></a>

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

### 4: Variable names <a name="CppVariableNames"></a>

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

### 5: Namespace names <a name="CppNamespaceNames"></a>

Namespace names are in `snake_case`.

**Example**
```C++
namespace uavpf
{
}
```

### 6: Macro names <a name="CppMacroNames"></a>

Macro names are in `MACRO_CASE`.

**Example**
```C++
#define UAVPF_ENABLE_DEBUG 1
#define UAVPF_DEBUG_ASSERT(condition, message) ::uavpf::DebugAssertImpl()
```

### 7: Curly braces positioning <a name="CppCurlyBracesPositioning"></a>

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

### 8: Mark methods with `const` where possible <a name="CppConstMethods"></a>

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

### 9: Always use curly-braces <a name="CppUseCurlyBracesForBlocks"></a>

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
