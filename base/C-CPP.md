---

typora-root-url: ./
---

# C & CPP

[TOC]

------



## Cpp和C的区别 

Cpp是C的超集，兼容大部分C的语法的结构。

1. 设计思想
    CPP面向对象，具有封装、继承、多态三种特性；
    C是面向过程的结构化编程。

2. 关键字

    1. 数量：C99有32个关键字，CPP98有63关键字。
    2. 具体
        1. struct：C中不能放函数，只能放函数指针；Cpp中可以放函数 。
        2. malloc：C中可以将其返回值赋给任意类型的指针；Cpp中必须强制类型转换。
        3. ...

3. 函数参数列表
    没有指定参数列表：C可以接收任意多个参数；Cpp会进行严格的参数类型检测，不接受任何参数。
    缺省参数：C不支持缺省参数；Cpp支持指定默认值。

    **Cpp中使用缺省函数有可能产生二义性。**

4. 函数重载：C中没有函数重载；Cpp支持函数重载（依靠name mangling手法）。
    **key：Cpp的名字修饰与C不同**  
    [mangling](#name_mangling)
    [overload](#overload)

5. 泛型编程：Cpp支持，比如模板类、函数模板、类模板。

6. 引用：C没有引用；Cpp有引用。

7. 默认链接属性：C对于变量默认链接属性是内链接；Cpp默认是外链接。

8. 输入输出流：cin，cout之所以效率低，是因为先把要输出的东西存入缓冲区，再输出，导致效率降低。可以使用<code>std::ios::sync_with_stdio(false);</code>取消同步。

9. 动态管理内存：C中使用malloc/free；Cpp使用new/delete。
    [malloc_new](#malloc_new)

------



## Cpp中的名称修饰（Name mangling）

 <span id="name_mangling">Name mangling</span>

维基：[https://en.wikipedia.org/wiki/Name_mangling](https://en.wikipedia.org/wiki/Name_mangling)

*[C++](https://en.wikipedia.org/wiki/C%2B%2B) compilers are the most widespread users of name mangling. The first C++ compilers were implemented as translators to[C](https://en.wikipedia.org/wiki/C_(programming_language)) source code, which would then be compiled by a C compiler to object  code; because of this, symbol names had to conform to C identifier  rules. Even later, with the emergence of compilers which produced  machine code or assembly directly, the system's[linker](https://en.wikipedia.org/wiki/Linker_(computing)) generally did not support C++ symbols, and mangling was still required.*

大概意思：C++编译器将C++代码转换为C代码，然后使用C编译器来编译从而得到目标代码。因此，符号的名称需要符合C语言的标志符规则。

### 为什么需要“修饰”

Consider the following two definitions of `f()` in a C++ program:

```cpp
int  f (void) { return 1; }
int  f (int)  { return 0; }
void g (void) { int i = f(), j = f(0); }
```

*These are distinct functions, with no relation to each other apart from  the name. If they were natively translated into C with no changes, the  result would be an error — C does not permit two functions with the same name. The C++ compiler therefore will encode the type information in  the symbol name, the result being something resembling:*

大概意思：不同的函数，相互之间除了名称没有其他关联。如果这些函数不做任何改变而直接转换为C代码，会带来一个错误——C语言不允许存在两个同名的函数。

**所以C++编译器需要将函数的签名信息编码到函数的符号名称中**

```cpp
int  __f_void (void) { return 1; }
int  __f_int (int)  { return 0; }
void __g_void (void) { int i = __f_v(), j = __f_i(0); }
```

*Notice that `g()` is mangled even though there is no conflict; name mangling applies to**all** symbols.*

注意函数g() 的名称也被修饰了，即使不存在与函数g() 的名称相冲突的地方：名称修饰会应用到所用的符号上。

### Complex example

GNU GCC 3.x 的编译器实现中的名称修饰规则

```cpp
namespace wikipedia 
{
   class article 
   {
   public:
      std::string format (void); 
         /* = _ZN9wikipedia7article6formatEv */
      bool print_to (std::ostream&); 
         /* = _ZN9wikipedia7article8print_toERSo */
      class wikilink 
      {
      public:
         wikilink (std::string const& name);
            /* = _ZN9wikipedia7article8wikilinkC1ERKSs */
      };
   };
}
```

1. 所有修饰的符号都是以_Z开头
2. 下划线后紧跟一个大写字母的名称在C语言中是保留的标志符
    (避免与用户的标志符相冲突)
3. 对于嵌套的名称（命名空间、类等等），紧跟着一个N。
4. 接着是一系列的 <length, id> pairs 。（<标志符长度，标志符>）
5. 最后接着一个E。

名称 wikipedia::article::format

被修饰为：<code>_ZN9wikipedia7article6formatE</code>

*For functions, this is then followed by the type information; as `format()` is a`void` function, this is simply `**v**`; hence:*

对于函数，接着的是类型信息，正如`format()`的参数的类型为void，所以名称中简单的加入一个 v 来表示。

被修饰为：<code>_ZN9wikipedia7article6formatEv</code>

而对于 *print_to *

参数的类型为std::ostream(准确来说是 std::basic_ostream<char, char_traits \<char\> > )

修饰时将用一个特定的别名**So**来表示。实际上参数的类型是引用，所以实际中采用的修饰名为**RSo**

最终得到的完整的修饰名称为：<code>_ZN9wikipedia7article8print_toERSo</code>

### Handling of C symbols when linking from C++

The job of the common C++ idiom:

```cpp
#ifdef __cplusplus 
extern "C" {
#endif
    /* ... */
#ifdef __cplusplus
}
#endif
```

通常C++中的extern “C”的惯用法就是保证C++编译器像C语言那样对待这些符号不进行修饰。C语言中的定义是不进行修饰的，C++编译器需要避免对那些符号引用的修饰。

For example, the standard strings library, `<string.h>` usually contains something resembling:

```cpp
#ifdef __cplusplus
extern "C" {
#endif

void *memset (void *, int, size_t);
char *strcat (char *, const char *);
int   strcmp (const char *, const char *);
char *strcpy (char *, const char *);

#ifdef __cplusplus
}
#endif
```

Thus, code such as:

```cpp
if (0==strcmp(argv[1], "-x")) 
    strcpy(a, argv[2]);
else 
    memset (a, 0, sizeof(a));
```

这会是正确的，没有被C++编译器修饰过的 `strcmp` 和 memset。

而如果没有使用 extern “C”，C++编译器（SunPro版本）将会产生下边的代码 

```cpp
if (0==__1cGstrcmp6Fpkc1_i_(argv[1], "-x")) 
    __1cGstrcpy6Fpcpkc_0_(a, argv[2]);
else 
    __1cGmemset6FpviI_0_ (a, 0, sizeof(a));
```

由于上边的这些函数符号在**C语言运行时库中并不存在，所以最后将发生链接错误。**

------



## C & CPP实现 重载

<span id="overload">Overload</span>

Cpp实现函数重载很大程度上依赖与编译器对函数名的 Mangling。

在 C/Cpp 程序中,我们可以忽略函数的返回值。在这种情况下,编译器和程序员都不知道哪个 Function 函数被调用。

所以只能**靠参数而不能靠返回值类型的不同来区分重载函数**。

### 可变参数实现重载

但是可以利用==可变参数==实现C语言的函数重载。

ANSI C 标准中，有可变参数的概念，可以通过一组宏实现

| 函数                    | 描述                                                         |
| ----------------------- | ------------------------------------------------------------ |
| col 3 is                | right-aligned                                                |
| va_list arg_ptr         | 定义一个可变参数列表指针                                     |
| va_start(arg_ptr, argN) | 让arg_ptr指向参数argN                                        |
| va_arg(arg_ptr, type)   | 返回类型为type的参数指针,并指向下一个参数                    |
| va_copy(dest, src)      | 拷贝参数列表指针,src->dest,                                  |
| va_end(arg_ptr)         | 清空参数列表，并置参数指针arg_ptr无效。每个va_start()必须与一个va_end()对应 |

```cpp
#include<stdio.h>  
#include<stdarg.h>  
int getMax(int n, ...)  
{  
        va_list va;  
        va_start(va,n); // init va, pointing to the first argument  
        int tmp,smax=-1;  
        int i;  
        for(i=0;i<n;i++)  
        {  
                tmp=va_arg(va,int); // get the next argument, the type is int  
                if(tmp>smax) 
                	smax=tmp;  
        }  
        va_end(va);  
        return smax;  
}  
int main()  
{  
        printf("%d/n",getMax(4,9,5,2,19));  
        printf("%d/n",getMax(6,1,3,4,5,2,0));  
}  
```

### 函数指针实现重载

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct _int_param
{
    int param1;
    int param2;
}INT_PARAM;
typedef struct _double_param_
{
    double param1;
    double param2;
}DOUBLE_PARAM;

typedef void* (*ADDFUNC)(void*);

void* int_add_func(void* wParam)
{
    INT_PARAM* lParam = (INT_PARAM*)wParam;
    int res = lParam->param1 + lParam->param2;
    printf("result = %d\n", res);
}

void* double_add_func(void* wParam)
{
    DOUBLE_PARAM* lParam = (DOUBLE_PARAM*)wParam;
    double res = lParam->param1 + lParam->param2;
    printf("result = %f\n", res);
}
void* add_func(ADDFUNC f, void* wParam)
{
    return f(wParam);
}
int main()
{
    INT_PARAM val1 = {10, 20};
    DOUBLE_PARAM val2 = {30.5, 40.5};
    add_func(int_add_func, &val1);
    add_func(double_add_func, &val2);
    return 0;
}

```

### 内置函数实现重载

```c
struct s1
{
    int a;
    int b;

    double c;
};

struct s2
{
    long long a;
    long long b;
};

void gcc_overload_s1(struct s1 s)
{
    printf("Got a struct s1: %d %d %f\n", s.a, s.b, s.c);
}

void gcc_overload_s2(struct s2 s)
{
    printf("Got a struct s2: %lld %lld\n", s.a, s.b);
}

// warning: dereferencing type-punned pointer will break strict-aliasing rules
#define gcc_overload(A)\
    __builtin_choose_expr(__builtin_types_compatible_p(typeof(A), struct s1),\
        gcc_overload_s1(*(struct s1 *)&A),\
    __builtin_choose_expr(__builtin_types_compatible_p(typeof(A), struct s2),\
        gcc_overload_s2(*(struct s2 *)&A),(void)0))
```

------

## C实现虚函数

简易实现，实现虚表、暴力寻址、this传入

```c
#include <bits/stdc++.h>
using namespace std ;
struct Class {
    const struct Vtabl *v_tabl ;
};
struct Vtabl {
    void (*VirtualFunc)(struct Class *) ;
};
void normal_fuc(struct Class *fake_this)
{
    printf("normal_fuc\n");
}
void virtual_func(struct Class *fake_this)
{
    const struct Vtabl *v_tabl = *(const Vtabl **)fake_this ;
    
    if (v_tabl->VirtualFunc) {
        v_tabl->VirtualFunc(fake_this) ;
    } else {
        printf("VirtualFunc not implemented\n");
    }
}
static void real_func(struct Class *fake_this)
{
    printf("real_func\n");
}
signed main(int argc, char const *argv[])
{
    const struct Vtabl v1 = {NULL} ;
    const struct Vtabl v2 = {real_func} ;
    struct Class a = {&v1} ;
    struct Class b = {&v2} ;
    virtual_func(&a) ;
    virtual_func(&b) ;
    return 0;
}
```



---

## 其他链接

SET CERT C++编码标准
https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=88046682

c++ FAQ
https://www.cs.technion.ac.il/users/yechiel/c++-faq/
