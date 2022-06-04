#  STL

分配器+容器+配接器+迭代器+仿函数+算法

## vector

Q：扩容的时机

A：

1. push_back的时候，原vector大小不足。
2. resize的时候，修改后的大小比原大小大。

### 扩容方式

VS的msvc中按1.5倍扩容，GCC按2倍扩容

C++标准并没有指定要用哪个增长因子，这是由标准库的实现者实现的。

Q：为什么是成倍扩容而不是固定大小扩容

A：假设增长因子为m，一共插入n个元素。

如果成倍扩容，时间复杂度为均摊常数。

![这里写图片描述](https://ling_boogie.gitee.io/peasonal_pic/cpp_base/stl_grow_2.png)

如果以固定大小扩容，需要O(n)

![这里写图片描述](https://ling_boogie.gitee.io/peasonal_pic/cpp_base/stl_grow_1.png)

Q：如何选取扩容因子

A：扩容因子越大，预留空间越多，浪费的空间越多。但是扩展到相同长度下，扩容因子越大时间开销越小。

**时间上**

如果每次push_back一个元素，那么插入N=k^n^个元素的元素搬运开销是

![[公式]](https://www.zhihu.com/equation?tex=k%281%2Bk%2Bk%5E2%2B%5Ccdots%2Bk%5En%29+%5Capprox+%5Cfrac%7Bk%5E2%7D%7Bk-1%7DN)

均摊开销是

![[公式]](https://www.zhihu.com/equation?tex=f%28k%29+%3D+%5Cfrac%7Bk%5E2%7D%7Bk-1%7D)

求导得到

![[公式]](https://www.zhihu.com/equation?tex=f%27%28k%29+%3D+%5Cfrac%7B2k%28k-1%29-k%5E2%7D%7B%28k-1%29%5E2%7D+%3D+%5Cfrac%7Bk%282-k%29%7D%7B%28k-1%29%5E2%7D)

k=2时，导数为0，f(k)能取得极小值。

------

**空间利用率上**

如果k=2，假设分配内存都小于128 bytes，stl启动二级配置器，从内存池中取一块出来用。

那么取用顺序是 1，2，4，8，16...，每次都是取用新内存，**之前释放的空间无法被下一次利用，这对cache是很不友好的。**

使用 k=2 增长因子的问题在于，每次扩展的新尺寸必然刚好大于之前分配的总和

如果k=1.5，那么取用顺序是 1，1 + 2 <= 4; 2 + 3 + 4 <= 16; 6 + 9 <= 16 …….在理论上存在重复利用的可能性。

Q：最优扩容因子？

A：我们希望的是**上几次的空间，存在被下一次扩容利用的可能性。**

也就是X(n-1)+X(n-2)>=X(n)，

而且我们希望时间上也要更好，

也就是X(n-1)+X(n-2)=X(n)。

很明显，这个序列应该是1，2，3，5，8，13，21，34，55....

所以最佳扩容因子是完美分割率1.618。

### 下标检查问题

```cpp
std::vector<int> vec;
size_t n = 1;
int & i = vec[n];
int & j = *(vec.begin() + n);
int & k = vec.at(n);
```

在 linux 平台只有 at(n) 会抛出 std::out_of_range 异常，而其他两个都不会。

**在 windows 平台 VS 环境下，都会抛出异常**

```cpp
reference operator[](size_type _Pos)
{	// subscript mutable sequence
#if _HAS_ITERATOR_DEBUGGING
	if (size() <= _Pos) {
		_DEBUG_ERROR("vector subscript out of range");
		_SCL_SECURE_OUT_OF_RANGE;
	}
#endif /* _HAS_ITERATOR_DEBUGGING */
	_SCL_SECURE_VALIDATE_RANGE(_Pos < size());
	return (*(_Myfirst + _Pos));
}
```

在 Debug 配置下， **_HAS_ITERATOR_DEBUGGING** 默认定义为 1，但是即使强制定义为 0，也有异常，因为还有一个 **_SCL_SECURE_VALIDATE_RANGE** 检查。所以即使在 Release 配置下，下标检查也是存在的。

**结论：尽量使用at接口，让问题得以暴露**

------

## 仿函数

哲学意义：满足STL对抽象性的要求，高度的泛型编程，能够充当软件积木，能够简单地“可被接配”

**为什么STL普遍使用仿函数，而不是函数指针**

使用的时候，如果使用函数指针作为回调函数，那么大部分场景下只能将此函数设计为 **unary function**，也就是单参数函数。

很明显这种方案不具有拓展性，并不利于设计具有一般性的程序。

如果想要在使用函数指针的情况下，增加函数体内使用到的外部参数，可能就只能通过全局变量实现了，但是众所周知全局变量的开销非常大，并不是一个好方案。

而如果使用仿函数，那就可以通过局部化外部参数的方式使其成为仿函数的一个 **算子**，提高隔离性。

而且，仿函数本身是一个类，通过巧妙的设计可以让其具有 **保持状态** 的功能，这是函数指针不好实现的（又回归到静态变量和全局变量的方案上了）。

最后，在效率方面，仿函数的调用能在编译期决议出需要使用的实例函数体，甚至可以直接进行优化；

但如果使用函数指针的话，编译器不能直接确定指针指向的函数，需要在执行期才能 “看见” 函数体并调用。

**总结**

1. 仿函数可以多参数维护
2. 仿函数更方便于把参数局部化，成为算子，提高隔离性
3. 仿函数能在编译期就决议出实例函数体，指针要在执行期才能决议
4. 仿函数更符合泛型编程的哲学，而且能被接配

---

## SGI-STL二级配置器

如果要分配的区块大于128bytes，则移交给第一级配置器处理，通过mmap系统分配到映射区。

如果要分配的区块小于128bytes，每次配置一大块内存，**并维护对应的16个空闲链表（free-list）**。

16个空闲链表分别管理大小为8、16、24......120、128的数据块。

下次若有相同大小的内存需求，则直接从free-list中取。
如果有小额区块被释放，则由配置器回收到free-list中。

当用户申请的空间小于128字节时，**将字节数扩展到8的倍数**，然后在自由链表中查找对应大小的子链表

1. 如果在自由链表查找不到或者块数不够，则向内存池进行申请，一般一次申请20块
    2. 如果内存池空间足够，则取出内存
3. 如果不够分配20块，**则分配最多的块数给自由链表**，并且更新每次申请的块数
3. 如果一块都无法提供，**则把剩余的内存挂到自由链表**，然后向系统heap申请空间，
    5. 如果申请失败，则看看自由链表还有没有可用的块，**如果也没有，则最后调用一级空间配置器**

---

## map的emplace缺陷

map新增一个元素可以使用insert、emplace、[ ]等方法

而对于<emphasize>emplace_*的设计初衷是分离memory和构造行为使得新增对象能在某段memory上直接构造</emphasize>，从而避免多余的tmp对象出现和拷贝构造行为。

但是map在新增元素时除了新增，还有判重和排序两个动作。

遗憾的是，根据emplace语义，判重这一工作需要基于map的obj，所以如果要完成判重工作那就会不可避免地出现tmp对象，而且判重需要基于obj而不是key。

**所以对于map，emplace不一定比insert更快**

对于emplace，需要构建出一个<code>\_Tree\_temp\_node<\_Alnode> \_Newnode(\_Getal(), \_Scary->\_Myhead, \_STD forward\<\_Valtys\>(\_Vals)...) ;</code>

补充，这不代表emplace会产生很多拷贝构造，因为在实现时皆由完美转发而来，你可以利用move语义避免拷贝。

因此，如果key本来就存在，那么insert完成key的比较之后就可以直接return了；而emplace必须原地构建出一个新的对象才能进行比较。

补充，如果insert一个非const的pair，那么调用的是`template<class P> insert(P&& value)`这一重载，而不是`insert(value_type&& value)`。这实际上等同于直接调用`emplace`1。

### 测试代码

```cpp
static void test_emplace(benchmark::State& state)
{
  std::map<std::string, int> s;
  std::string val("qaq");
  s.emplace(val, 1);

  for (const auto _ : state) {
    s.emplace(val, 1);
  }
}

BENCHMARK(test_emplace);

static void test_insert(benchmark::State& state)
{
  std::map<std::string, int> s;
  std::string val("qaq");
  const std::pair<const std::string, int> p{val, 1};
  s.emplace(val, 1);

  for (const auto _ : state) {
    s.insert(p);
  }
}

BENCHMARK(test_insert);


static void test_emplace_non_tmp(benchmark::State& state)
{
  std::map<std::string, int> s;
  std::string val("qaq");
  s.emplace(val, 1);

  for (const auto _ : state) {
    s.emplace("qaq", 1);
  }
}
BENCHMARK(test_emplace_non_tmp);

```

![](https://ling_boogie.gitee.io/peasonal_pic/cpp_base/map_test.png)

---

## STL的 copyable 和 assignable 准则

> Since STL containers require that all contents be copyable and  assignable, what is the prefered idiom when working with non copyable  objects?

也就是说STL要求Obj需要 copyable 和 assignable ，而对于emplace_*相关方法可以要求Obj只具备assignable

所以对于Non copyable的对象，如果你的cpp版本高于cpp11，那么可以使用以下的策略

1. 使用智能指针包裹对象再传入STL
2. 自定义STL

而如果版本低于cpp11，那么你应该使用侵入式的容器，使用钩子函数来插入对象

[Boost.Intrusive](http://www.boost.org/doc/libs/1_53_0/doc/html/intrusive.html)可以满足你的要求。

### 何为copyable

概念上来说copyable不等同于对象copy，他描述的是对象整体的可复制能力，而不是单纯通过copy行为进行复制。（我可以触发bitwise、memwise、piecewise等等行为来完成）

而从代码上来说，copyable是

- 可以copy
- 可以move
- 可以assign

```cpp
template<class T>
    concept copyable = copy_constructible<T> && movable<T> && assignable_from<T&, T&> && assignable_from<T&, const T&> &&assignable_from<T&, const T>  ;
```

补充，为什么STL不能以引用传入，因为引用没有自己能转移的空间所以不是assignable（在.rodata段有自己的空间，但无法使用），而且引用被符号表固定所以也不是copyable。

----

## 实现Vector（三指针）

代码：https://blog.csdn.net/ygtrece_13/article/details/119785089

---

## stl-分配器allocator实现

来源：https://blog.csdn.net/tang123246235/article/details/121775914

简单实现：https://zhuanlan.zhihu.com/p/185611161