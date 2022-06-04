# 全局对象

考虑以下程序片段

```cpp
Matrix identity ;
int main()
{
    // identity 必须在这被初始化
    Matrix m1 = identity ;
    ...
    // identity 必须在这被摧毁
    return 0 ;
}
```

如果这种global obj存在constructor和destructor的话，我们说它需要静态初始化和内存释放操作。

## 前言

请注意，我们都知道所谓的”已初始化的全局对象在data段，未初始化的全局对象在bss段“。

这并不是完全正确的。

在C中，没有显式初始化的global被视为一个”临时性的定义“，一个”临时性的定义”可以在程序中发生多次，只是那些实例会被 **链接器** 折叠起来（比如有多个文件用到了同一个global，每个想用的文件都实例化一下），最终只留下一个实例，被放在data segment中一个 **特别保留给未初始化的global使用** 的空间。这就是 Block Started by Symbol的由来，是IBM 704 assembler的一个pseudo-up（你可以理解为一个伪上升概念）。

也就是说BSS段实际上是Data段的一部分。

但是！在C++中没有，或者是不支持“临时性的定义”，这是因为存在class构造行为的隐式应用。这可能不太好懂。

只需要知道最终global在C++中被视为完全定义（它会阻止第二个或更多个定义）。

Bss data segment在C++中相对地不重要，**C++的所有全局对象都被以“初始化过的数据”来对待！**所有global obj都被放置在data segment，会把显式指定的值作为初值，否则所配置到的内存内容为0.（这和C略有不同，C并不自动设定初值）

比如以下的代码

```cpp
int v1 = 1024 ;
int v2 ;
```

现在我们知道v1的值是1024，v2的值是0 。

而在C中一个global只能被一个常量表达式（目的是在编译时期就求值）设定初值。

很明显，constructor并不是一个常量表达式虽然class obj能在编译期被放置在data segment并设置为0，但是constructor需要一直到程序启动时才会实施。

因此必须对一个“放置于data segment的obj的初始化表达式”做评估以防止非法初始化（运行期，程序启动时），这正是obj需要静态初始化的原因。

## 静态初始化的策略

首先策略必须在每一个UNIX平台上都有效，而且对相关的 linker 或 object-file format 不能预先做任何假设。

### munch程序

基于这样的限制，有一个可移植但是成本颇高的munch策略。

1. 为每个需要静态初始化的**文件**产生一个_sti()函数（static initialization），内含必要的constructor调用或inline expansions。
2. 类似的情况，产生一个_std()函数（static deallocation），内含必要的destructor调用操作或inline expansions。
3. 提供一组 **runtime library** "munch"函数：一个_main()函数用以调用可执行文件中的所有 _sti()，一个exit()函数调用所有的 _std()。

在这种策略下，开头我们提到的identity对象会在matrix.c中产生以下的函数

```cpp
__sti__matrix_c__identity() {
    identity.Matrix::Matrix() ;
}
__std__matrix_c__identity() {
    ...
}
```

其中matrix_c是文件名编码，_identity表示文件中所定义的第一个 static object。在__sti之后附上这两个名称，可以为执行文件提供一个独一无二的标识符号，一个mangling。

这里的exit()与C library的exit()不同，为了链接前者，在cfront的CC命令中必须指定C++ standard library。

最后一个需要解决的问题是，如何收集一个程序中的所有object file的_sti()函数和__std()函数。而且他们需要是可移植的——至少在所有UNIX平台上。

最终的解决方法是依靠nm指令，nm会倾印出object fil的符号表格项目。

一个可执行文件是由.o文件产生出来的，nm将施行于可执行文件上。其输出被导入(piped into)munch程序中，然后munch “用力咀嚼”符号表格中的名称，然后把需要的函数加到一个sti()函数和std()函数的 **跳离表格** 中。

接下来它把这个表格写到一个很小的program text文件中，然后CC命令被 **重新激活** ，将这个小文件加以编译。

最后将整个可执行文件重新链接，_main()和exit()函数在各个表格上走访一遍，轮流调用每一个项目(代表一个函数地址) 。

### munch patch

munch程序可以解决问题，但是从某些哲学角度上来说离正统的计算机科学稍微远了一点。

于是修补版munch(patch)中有一个快速的变种方法。

patch假设可执行文件都是System V COFF格式，于是它会检验可执行文件并找出那些“有着__link nodes ，并内含一个指针，指向 \_\_sti()函数和\_\_std()函数”的文件，串链起它们，将链表的根源设为一个全局性的 \_\_head object。

最后新的_main()和exit()会以 _head为起始走访一遍链表，而不需要重新链接可执行文件再检查各个表格。

### 其他

不管怎么样，最终目的都是想要直接支持静态初始化和内存释放操作。

例如， System V 的 Executable and Linking Format 被扩充以增加.init和.fint两个sections，这其中内含对象所需要的信息，分别对应于静态初始化和释放操作。

编译器特定的setup函数会完成平台制定的支持。

> 所谓section就是16位时代的segment

## 弊端

使用被静态初始化的object，有一些缺点。

如果exception handling被支持，那些objects就不能被放置在try区段之内，这对于被静态调用的constructor可能是特别无法接受的。

因为任何的throw操作都必然会触发exception handling library默认的 <code>terminate()</code>函数，如果该函数内部构造了对象的话，系统会先对该对象调用析构函数，当对象调用完了析构函数以后，才开始执行异常的抛出工作。

但是问题在于静态初始的object的清除操作在exit()中，应该在文件的末尾。

还有缺点是，假如不同模块中的全局对象在构造时存在依赖，那么我们就必须控制对象之间的相依顺序，这种行为扯出来的复杂度会直接导致程序启动的缓慢。

## 克服弊端

1. 很简单，不适用全局对象就可以了。在绝对需要全局资源的情况下，通过引用返回并使用<code>std :: call_once()</code>初始化的函数静态对象是更好的选择。
2. 在链接可执行文件时，将全局对象放置在适当的位置（例如最后）会导致它首先被初始化。可以将对象文件适当地放置在需要维护的所有系统上。这里的主要缺点是：依然无法保证顺序，并且在编译器版本之间切换时可能会发生变化。但是，对于C ++标准库，这是可以接受的（并且在执行此操作时，通常只关心全局流对象）。
3. 将全局对象放入专用的共享库中：加载共享库时，将执行其初始化代码。共享库中的对象仅在初始化完成后才可用。这可以可靠地工作，但需要一个额外的库。

