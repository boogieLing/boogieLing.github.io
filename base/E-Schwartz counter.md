# Schwartz counter

Schwartz计数器

（以Jerry  Schwartz的名字命名，当时他设计了IOStreams库的基础，现在已成为标准；请注意，不能将许多奇怪的选择归咎于他，因为这些选择已标记在原始设计上）

主要是为了控制 **需要跨越模块做静态初始化** 的 Obj 的相依顺序。

例如现在以下文件

Foo.h

```cpp
class Foo
{
	Foo::Foo() ;
} ;
```

Foo.cpp

假定<code>"MyStream.h"</code>提供 <code>myStream</code>的访存允许，并且其能为了初始化 <code>myStream</code>而创建出一个 <code>file-static object </code>(Schwartz counter)

```cpp
#include "Foo.h"
#include "MyStream.h"
Foo::Foo()
{
	myStream << "Hello world\n" ;
}
```

如果Foo::Foo()能在main()之后才被进程使用，那么 myStream 的使用能保证会成功。（其会在被使用之前初始化），因为上文所提到<code>file-static object </code>

但是，假如 Foo 实例的创建在 main() 的话，比如让他成为一个 global obj。

Global.cpp

```cpp
#include "Foo.h"
Foo foo ;
```

Global.cpp不会像Foo.cpp那样获得文件静态初始化程序对象。

Schwartz计数器还能否正常工作呢？

事实上，使用  Schwartz计数器，仍然可能会导致在对象创建之前对其进行访问。最明显的情况是在构造全局对象的过程中调用一个函数，该函数使用通过Schwartz计数器构造的自身全局变量来调用另一个单元（使用std :: cout作为全局变量，并由Schwartz计数器进行保护）：****

```cpp
// file a.h
void a();

// file a.cpp
#include <iostream>
void a() { 
    std::cout << "a()\n"; 
}

// file b.cpp
#include <a.h>
struct b { 
    b() { 
        a(); 
    } 
} bobject;
```

如果 b.cpp 中的全局对象在 a.cpp 的全局对象之前被构造，并且 std::cout 是通过 Schwartz计数器来构造的，那么这个例子毫无疑问会失败。

当在很多全局对象上使用Schwartz计数器的时候，Schwartz计数器需要在每个需要加载的对象前运行一段代码，这会导致严重的启动延迟。

**最好的解决方法就是不要使用class类型的全局变量！**

