# Data member的存取

参考以下这段代码

```cpp
Point3d origin;
origin.x=0.0
```

现在我们想知道x的存取成本是什么。

答案需要视x和Point3d如何声明而定。

x可能是个static member，也可能nonstatic。

而Point3d可能是个独立的class，也可能是单一继承而来，也可能是多重继承或虚拟继承而来。

除此之外，我们还希望知道通过以下两种方法进行存取有什么重大差异吗？

```cpp
origin.x=0.0;
pt->x=0.0;
```



## Static Data Members

static data members被编译器提出于class之外，并被视作一个global变量（但是只在class生命范围之内可见）。

每一个member的存取许可，以及与class的关联，**并不会招致任何空间上或执行时间上的额外负担**——不管在个别的class obj还是在 static data member本身。

每一个这种member只有一份实例，存放在程序的data segment。

每次取用的时候都会被内部转化为对该唯一extern实例的直接参考操作。

所以上述两种存取方式都会被转化为

```cpp
Point3d::x=0.0;
```

> 这是Cpp中“通过一个指针和通过一个对象来存取member，结论完全相同”的唯一一种情况。

PS：“经由member selection operators（也就是 . 运算符）对一个static data member进行存取“只是一种文法上的便宜行事。

member并不在class obj之中，因此存取static members并不需要通过class obj。

就算class是一个从复杂继承关系中继承而来的member也无关紧要，存取路径仍然是那么直接。

如果static data member是经由函数调用，或者其他语法而被存取呢？

```cpp
foobar().x=0.0;
```

此问题的关键在于，foobar()是否必须被求值，cfront的做法是简单地把它丢掉。

但是Cpp Standard明确要求foobar()必须被求值，尽管其结果并无用处。

下面是一种可能的转化：

```cpp
//foobar.x=0.0;
(void)foobar();
Point3d::x=0.0;
```

如果去取一个static data member的地址，会得到一个指向其数据类型的指针，而不是一个指向其class member的指针，关键还是因为static member并不内含在一个 class obj之中。

例如：

```cpp
&Point3d::x ;
```

会得到类型如下的内存地址：

```cpp
const int*
```

如果有两个 **不相同** 的class，都声明了一个同名 static data member，当他们同处data segment时，就会导致名称冲突。

编译器的解决方法是对每一个static data member进行 **name-mangling**。

每个编译器的 mangling 手法可能都不尽相同，

但是他们有两个重点：

1. 一个算法，能推导出独一无二的名称。
2. 假设编译系统必须与使用者交谈，那些独一无二的名称可以轻易地被推导回到原来的名称。

## Nonstatic Data Members

nonstatic data members直接存放在每一个class obj之中。

需要经由显式的或隐式的的obj才能直接存取他们。

**只要在一个member function中**直接处理一个 nonstatic data member，就会发生 <code>implicit class object</code>。

```cpp
Point3d Point3d::translate(const Point3d &pt) {
    x+=pt.x ;
    ...
}
```

事实上这个函数的参数是：

```cpp
Point3d Point3d::translate(Point3d *const this, const Point3d &pt) {
    this->x+=pt.x ;
    ...
}
```

本质上，存取一个nonstatic data member的操作的情况是这样：

```cpp
origin._y=0.0;
//程序员眼中的代码
*(&origin + (&Point3d::_y - 1))=0.0 ;
//可能是编译器眼中的代码
```

PS：请注意其中的-1操作，指向data member的指针，其offset值总是会被加1，目的是让编译器区分出”一个指向data member的指针，用以指出class的第一个member“和”一个指向data member的指针，没有指出任何member“两种情况。

每一个nonstatic data member的 offset在编译时期就可以获知，

如果member属于一个base class obj（派生自单一或多重继承串链）也一样。

因此，存取一个nonstatic data member，其效率和存取一个C struct member是一样的。

现在来讨论虚拟继承。

虚拟继承将为”经由base class object存取class members“导入一层新的间接性。

比如：

```cpp
Point3d *pt;
pt->_x=0.0;
```

如果_x是一个virtual base class的member，存取速度会稍慢一点。

## 总结

回到一开始的问题：

```cpp
origin.x=0.0;
pt->x=0.0;
```

答案是“当Point3d是一个derived class，而且其继承结构中有一个virtual base class，并且被存取的member是一个从该virtual base class继承而来的member“时，就会有重大的差异。

我们不能确定pt必然指向哪一种class type，

我们也就不知道编译时期这个member真正的offset的位置。

所以这个存取操作 **必须延迟到执行期**，经由一个额外的间接导引才能解决。

但是如果使用origin，就不会有这些问题，其类型无疑是Point3d class（编译期的符号表可以确定），members 的offset位置也就在编译期就能固定。

一个积极进取的编译器甚至能静态地经由origin就解决掉对x的存取。