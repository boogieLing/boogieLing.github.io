# 程序转化语义

考虑以下代码

```cpp
#include <bits/stdc++.h>
using namespace std ;

class Vector
{
public:
    static int cnt ;
    int x, y ;
    Vector(int x=0, int y=0){
        this->x = x ;
        this->y = y ;
        ++cnt ;
        printf(".Default constructor. %p\n", (void *)(this));
    }
    Vector(const Vector &a) {
        this->x = a.x ;
        this->y = a.y ;
        ++cnt ;
        printf(".Copy constructor.%p\n", (void *)(this));
    }
    ~Vector(){
        printf(".Delete constructor.%p\n", (void *)(this));
    }
    static void init() {
        cnt = 0 ;
    }
};

Vector add(Vector &a, Vector &b) {
    Vector v ;
    v.x = a.x + b.x ;
    v.y = a.y + b.y ;
    printf("+Add-v %p\n", &v);
    return v ;
}
Vector create() {
    Vector v ;
    v.x = 10 ;
    v.y = 30 ;
    printf("+Create-v %p\n", &v);
    return v ;
}
void foo(Vector v) {
    printf("+foo-v %p\n", &v);
    return ;
}
int Vector::cnt = 1 ;

signed main(int argc, char const *argv[])
{
    printf("Vector size: %d\n", (int)sizeof(Vector));
    Vector a(1, 1) ;
    Vector b(2, 10) ;
    
    Vector::init() ;
    printf("---Test---\n");
    Vector c ;
    foo(c) ;
    printf("---Constructor were called %d times---\n", Vector::cnt);
    getchar() ;
    
    Vector::init() ;
    printf("---Test---\n");
    Vector c1 = Vector::Vector(90, 100) ;
    printf("---Constructor were called %d times---\n", Vector::cnt);
    getchar() ;

    Vector::init() ;
    printf("---Test---\n");
    Vector c2 = create() ;
    printf("---Constructor were called %d times---\n", Vector::cnt);
    getchar() ;

    Vector::init() ;
    printf("---Test---\n");
    Vector c3 = add(a, b) ;
    printf("---Constructor were called %d times---\n", Vector::cnt);
    getchar() ;
    //Vector c4 = std::move(create()) ;
    return 0;
}
```

对于 class Vector，我定义了一个默认构造函数、一个拷贝构造函数、一个析构函数，其中有两个 non-static member data和一个 static member data。

cnt用来统计构造函数（包括默认构造和拷贝构造）被调用的次数。

普通函数create()用以返回一个新object；

普通函数add()用以返回两个Vector相加的结果（一个Vector object）

程序开始，首先显式地以参数构造出两个object：a和b。

然后定义c1来自一个显式调用的默认构造函数。

将create()的返回值赋给c2 。

将add()的返回值赋给c2 。

现在我们需要探究默认构造函数以及拷贝构造函数被调用的次数，以及中间可能会生成什么临时对象。

## 结果

首先在有优化的情况下运行：

```shell
Vector size: 8
.Default constructor. 00000000006ffdf0
.Default constructor. 00000000006ffde0
---Test---
.Default constructor. 00000000006ffdd0
.Copy constructor.00000000006ffe00
+foo-v 00000000006ffe00
.Delete constructor.00000000006ffe00
---Constructor were called 2 times---

---Test---
.Default constructor. 00000000006ffdc0
---Constructor were called 1 times---

---Test---
.Default constructor. 00000000006ffdb0
+Create-v 00000000006ffdb0
---Constructor were called 1 times---

---Test---
.Default constructor. 00000000006ffda0
+Add-v 00000000006ffda0
---Constructor were called 1 times---

.Delete constructor.00000000006ffda0
.Delete constructor.00000000006ffdb0
.Delete constructor.00000000006ffdc0
.Delete constructor.00000000006ffdd0
.Delete constructor.00000000006ffde0
.Delete constructor.00000000006ffdf0
Press any key to continue . . .

```

使用<code>-fno-elide-constructors </code>取消优化：

```shell
Vector size: 8
.Default constructor. 00000000006ffdd0
.Default constructor. 00000000006ffdc0
---Test---
.Default constructor. 00000000006ffda0
.Copy constructor.00000000006ffdd0
+foo-v 00000000006ffdd0
.Delete constructor.00000000006ffdd0
---Constructor were called 2 times---

---Test---
.Default constructor. 00000000006ffde0
.Copy constructor.00000000006ffdb0
.Delete constructor.00000000006ffde0
---Constructor were called 2 times---

---Test---
.Default constructor. 00000000006ffd50
+Create-v 00000000006ffd50
.Copy constructor.00000000006ffdf0
.Delete constructor.00000000006ffd50
.Copy constructor.00000000006ffda0
.Delete constructor.00000000006ffdf0
---Constructor were called 3 times---

---Test---
.Default constructor. 00000000006ffd50
+Add-v 00000000006ffd50
.Copy constructor.00000000006ffe00
.Delete constructor.00000000006ffd50
.Copy constructor.00000000006ffd90
.Delete constructor.00000000006ffe00
---Constructor were called 3 times---

.Delete constructor.00000000006ffd70
.Delete constructor.00000000006ffd80
.Delete constructor.00000000006ffd90
.Delete constructor.00000000006ffda0
.Delete constructor.00000000006ffdb0
.Delete constructor.00000000006ffdc0
Press any key to continue . . .
```

从结果看，在有优化的情况下，拷贝构造函数全部被优化掉了，而且从地址输出可以得知额外的临时对象也减少非常多了。

所以编译器到底做了什么，优化了什么？

## 显式的初始化操作

**显式的初始化操作转化是必要的。**

比如：

```cpp
X x ;
void foo(X x) {
    X x1(x) ;
    X x2 = x ;
    X x3 = X::X(x) ;
}
```

会施行两步转换

1. 重写每一个定义，剥除其中的初始化操作。在严谨的C++用词中，“声明”是指“准备占用内存”的行为，“定义”是指“占用内存”的行为。
2. class 的 copy constructor 会被安插进去。

所以转化后如下：

```cpp
X x ;
void foo(X x) {
    X x1 ;
    X x2 ;
    X x3 ;
    x1.X::X(x) ;
    ...
}
```

------

## 破坏对称性

**这一步不是必要的，而且破坏对称性是此步的一个副作用**

比如：

```cpp
X foo() {
    X a ;
    return a ;
}
X xx = foo() ;
```

对称性就是：以copying方式产生出一个object的时候，对称地调用destructor 。

所以我们希望在L5处会有一个copy constructor，这也符合显式初始化操作的转换。

然后在L5之后的某处，当xx生命周期结束时有一个destructor。

然而优化会打破这种对称性：

```cpp
void foo() {
    X x1(1024) ;
    X x2 = X::X(1024) ;
    X x3 = (X)1024 ;
}
```

三个初始化操作在语义上相等，但是第二和第三个明显应该更不一样。

1. 将一个临时性的object设以初值1024；
2. 将临时性的object以拷贝构造的方式作为explicit object的初值。

也就是说x1以单一的default constructor初始化，而x2和x3却需要两次constructor。

```cpp
X x2 ;
X __temp0 ;
X::X(__temp0, 1024) ; // X::X(X &__this, int n)
X::X(x2, __temp0) ; //X::X(X &__this, cosnt X &a)
X::~X(__temp0) ;// X::~X(X &__this)
```

而在优化下，则会剔除copy constructor的调用操作，就变成了以下的情况

```cpp
X x2 ; // do not default constructor
X::X(x2, 1024) ; // X::X(X &__this, int n)
```

这就是对于示例代码中c1的情况

```cpp
//优化后
---Test---
.Default constructor. 00000000006ffde0
---Constructor were called 1 times---
```

```cpp
//未优化
---Test---
.Default constructor. 00000000006ffde0
.Copy constructor.00000000006ffdb0
.Delete constructor.00000000006ffde0
---Constructor were called 2 times---
```

符合理论。

------

## 参数初始化

**参数初始化转化是必要的**

把一个 class object 当作参数传给一个函数（或者是作为一个函数的返回值），相当于以下形式的初始化操作：

```cpp
X xx = arg ;
```

xx形式参数（或返回值），arg代表真正的参数值。

因此会出现以下这种情况：

```cpp
void foo(X x) ;

//以这样的方式调用
X xx ;
foo(xx) ;

//必然会转化
//改写foo()的声明
void foo(X &x) ;

X xx ;
X __temp0 ; //注意，此处没有默认构造，因为后面会有拷贝构造
X::X(__temp0, xx) ; //X::X(X &__this, cosnt X &a)
foo(__temp0) ; //foo(X &x)
```

这会产生一个临时对象，将xx以bitwise的方式拷贝到这个临时对象中，这个临时对象作为foo()的局部实例。而这个额外对象会在函数结束时被析构。

这一步是**必要的**，所以可以看到在示例代码中的<code>Vector c; foo(c) ;</code>不管有没有优化都会产生一个临时性对象 ；

```cpp
//优化后
---Test---
.Default constructor. 00000000006ffdd0
.Copy constructor.00000000006ffe00
+foo-v 00000000006ffe00
.Delete constructor.00000000006ffe00
---Constructor were called 2 times---
```

```cpp
//未优化
---Test---
.Default constructor. 00000000006ffda0
.Copy constructor.00000000006ffdd0
+foo-v 00000000006ffdd0
.Delete constructor.00000000006ffdd0
---Constructor were called 2 times---
    
```

请注意，此处的转化是针对入参的，也就是说形参入了一个class的情况，你也可以说传入了一个object的情况。

------

## 返回值初始化（未优化）

对于实例代码中的：

```cpp
Vector create() {
    Vector v ;
    v.x = 10 ;
    v.y = 30 ;
    printf("+Create-v %p\n", &v);
    return v ;
}
```

在完全取消优化的情况下，会有一个 **双阶段转化**。

1. 首先被调用函数会在入参列表首部加上一个额外参数，他的类型是class reference，此参数用以放置“拷贝构建”而得的返回值。
2. 在return指令之前安插一个 copy constructor 调用操作，以便将欲传回的object的内容当作上述新增参数的初值。

所以此函数会转换如下：

```cpp
void create(Vector &__result) {
    Vector v ;
    ...
    printf("+Create-v %p\n", &v);
   	Vector::Vector(__result, v) ;
    Vector::~Vector(v) ;
    return ;
}
```

而调用处也会发生转换：

```cpp
//未转换
Vector c2 = create() ;

//取消优化，施行转换
Vector c2 ; 
//注意，不必施行user default constructor,因为后面有拷贝构造
Vector __temp0 ; 
//注意，不必施行user default constructor,因为后面有拷贝构造
create(__temp0) ;
Vector::Vector(c2, __temp0) ;
Vector::~Vector(__temp0) ;
```

所以会发生这样的事情：

1. 新增一个临时对象__temp0但不调用用户的默认构造
2. v 默认构造
3. 用 v 拷贝构造 __result
4. 析构 v
5. __result 和 \_\_temp0 是同一个，用 \_\_temp0 拷贝构造 c2
6. 析构 __temp0

```cpp
---Test---
.Default constructor. 00000000006ffd30
+Create-v 00000000006ffd30
.Copy constructor.00000000006ffdf0
.Delete constructor.00000000006ffd30
.Copy constructor.00000000006ffd80
.Delete constructor.00000000006ffdf0
---Constructor were called 3 times---
```



------

## 返回值初始化（NRV优化）

NRV优化如今已被视为标准C++编译器中一个 **义不容辞** 的优化操作。

可以发现在 create() 中，不管是 <code>Vector v ;</code>还是<code>Vector z ;</code>还是其他什么变量。

用以保存返回值的都是 <code>X __result</code>。

我们可以称 <code>X __result</code>为无名数值，而 v 和 z 则是具名数值（named value）。

而NRV优化就是 Named Return Value 优化，也就是以 __result 取代函数中的具名数值。

优化后：

```cpp
void create(Vector &__result) {
    ... //直接操作 __result
    printf("+Create-v %p\n", &__result); //直接操作 __result
    return ;
}
```

而调用处也没必要产生一个临时对象来拷贝构造返回值了

```cpp
//未转换
Vector c2 = create() ;

//优化，施行转换
Vector c2 ; 
//注意，需要施行user default constructor,因为后面不会再有拷贝构造
create(c2) ;
```

现在只会发生这些事情了：

1. c2 默认构造

**芜湖起飞！**

------

## NRV优化的缺陷

目的是尽力抑制 拷贝构造函数，所以如果程序员没有为 class 提供一个显式的拷贝构造函数，那么编译器就认为这个class的拷贝构造不重要（也就是bitwise就可以满足要求了），在这种情况下就可能不施行NRV优化（编译器认为没必要）。

但是在测试时把拷贝构造函数删了，调试时发现依然施行了NRV。

而且如果拷贝构造函数中输出了某些信息，那么NRV优化之后就会造成代码输出不一致的问题 。