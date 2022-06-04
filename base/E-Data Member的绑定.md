# Data Member的绑定

考虑下面的这段程序代码

```cpp
//某个foo.h头文件，在此处含入
extern float x;
//另一个文件
class Point3d
{
public:
    Point3d(float,float,float);
    // 问题：被传回的x是哪一个？
    float X() const {return x;}
    // 问题：被设定的x是哪一个？
    void X(float new_x) const {x=new_x;}
    // ...
private:
    float x,y,z;
};
```

以上提出了两个问题，在今天肯定可以回答是内部的x，这是正确的，但不是一直都是正确的。

## 以前的Cpp

在Cpp最早的编译器上，如果像这样对x做出参阅操作，该操作将会指向 global x object。这肯定不是在预期之中，并因此导出早期Cpp的两种防御性的程序设计风格。

1. 把所有的 <code>data members</code> 放在class声明起头处，以确保正确的绑定。
2. 把所有的<code>inline functions</code>，不管大小都放在class的声明之外。

这些古老的语言规则被称为 "member rewriting rule"，大意是“一个inline的函数实体，在整个class声明未被完全看见之前，是不会被评估求值的”。

## Cpp Standard

上述行为的必要性自从C++2.0之后就消失了。

Cpp已经提炼了这个“rewriting rule”，其效果是，如果一个inline函数在class声明之后立刻被定义的话，那么就还是对其评估求值。

也就是说如果写下以下这样的代码：

```cpp
extern int x;
//另一个文件
class Point3d
{
public:
    Point3d(float,float,float);
    float X() const {return x;}
private:
    float x;
};
```

此时，对member functions本体的分析，会直到 **整个class的声明都出现了** 才开始。

因此，在一个inline member function躯体之内的一个data member绑定操作，会在整个class声明之后才发生。

然而，这对于member function 的 <code>argumemt list</code>并不为真。

Argument list中的名称还是会在他们第一次遭遇时被适当地决议完成。

因此还是会存在 extern 和 nested type names 之间的非直觉绑定操作。

例如在下面的程序片段中，length的类型在两个member function signatures中都被决议为global typedef。

当后续再有length的nested typedef的声明出现时，Cpp Standard就会把稍早绑定的类型标示为非法：

```cpp
typedef int length;
class Point3d
{
public:
    // length val被决议为int
    void mumble(length val) {_val=val;}
    length mumble() {return _val;}
private:
    typedef float length;
     // length _val被决议为int
    length _val;
};
```

所以仍然需要某种防御性程序风格：请总是把nested type声明（考虑是否真的需要）放在class的起始处。