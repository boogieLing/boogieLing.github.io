# Data语意学

考虑以下结构

```cpp
class X{ };
class Y : public virtual X { };
class Z : public virtual X { };
class A : public Y , public Z { };
```

上述X Y Z A没有任何一个class内含明显的数据。

试问他们的大小各自应该为多少？

结果是

```cpp
X:1
Y:8
Z:8
A:12
```

特别地，在Visual C++5.0中

```cpp
X:1
Y:4
Z:4
A:8
```

**一个空的class，在事实上并不是空的！**

## X为什么为1 byte?

空class会有1 byte大小，那是被编译器按插进去的一个 <code>char</code>

目的是使得同一class的两个obj得以在内存中配置独一无二的地址。

## Y,Z为什么为8 bytes?

这个大小和机器有关，也和编译器有关。

1. 语言本身造成的额外负担：当语言支持 virtual base classes ，就会导致一些额外负担。在 derived class 中，这个额外负担一般是某种形式的指针，它或者只想 vritual base class subobject ，或者指向一个相关表格（不一定是vtbl）；表格中存放是不是 virutual base class subobject 的地址，就是其偏移地址。
2. 编译器的某些优化处理：Virtual base class X subobject 的 1 byte 也会出现在Y和Z身上。传统上它会被放在 derived class 的固定部分的 **尾端** 。
3. Alignment的限制：Y和Z的大小会被调整到某个数的整数倍，在32位计算机上通常位 4 bytes ，以使bus的运输量达到最高效率。

所以Y和Z为5 bytes ，字节对齐后达到 8 bytes。

但是，某些新近的编译器对 Empty virtual base class 提供了特殊的处理。（注意是）

在新策略下，一个 Empty virtual base class 被视作derived class object **最开头的一部分**，但是因为它 Empty，所以它也 **没有花费任何的额外空间** 。这就节省了上述第二点的 1 bytes（相当于已经有member了，不需要那 1 char 了） ，而字节对齐之后也是 4 bytes ，在此模型下，Y和Z的大小都是4。

## 期望的 A 多大？

我们需要关注那种并未特别处理 empty virtual base class 的编译器。

如果Y和Z都不是“虚拟派生“自X，我们可能会回答16 。

但是，一个 virutual base class subobject 只会在 derived class 中存在 **一份实例** ，不管它在class继承实例中出现了多少次！

class A的大小由下列几点决定：

- 被共享的一个X实例，1 byte 。
- Base class Y的大小，减去”因 virtual base class X而配置“ 的大小（只有一份class X！现在在A中！） 4 bytes 。
- Z同上。
- A自己的大小：0 byte 。（不是virtual，且已经是derived class，不存在1 byte用作区别）
- Alignment数量（如果有），填补 3 bytes 。

所以期望中的 A 的大小是 12 bytes 。（具体还需看编译器如何实现）

如果考虑了特别处理 empty virtual base class 的编译器，X实例的1 byte将会被拿掉！于是 A 的大小会是 8 bytes。

注意，差异来自 virtual base class X。

如果我们在 X 中放置 data members，那么两种编译器就会产生出完全相同的对象布局。

## 其他

Cpp对象模型尽量以空间优化和存取速度优化的考虑来表现 nonstatic data members，并且保持和C struct数据配置的兼容性。

对于继承而来的 nonstatic data members也是如此，不过并没有强制定义其间的排列顺序。

至于static data members，则会被放置在程序的一个global data segment之中，不会影响个别class obj的大小（注意只是个别）。

甚至即使该class没有任何obj实例，其 static data members 也已存在。

但是一个 template class 的 static data members 的行为却会稍有不同。

每一个Obj 都必须有足够的大小以容纳它所有的 nonstatic members ，用以支持某些语言特性（主要是各自virtual特性）

还有边界调整的需要。

最后它的大小可能会让人吃惊。

---

