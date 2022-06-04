# Data Member的布局

已知下面一组 data members：

```cpp
class Point3d
{
public:
    // ...
private:
    float x;
    static List<Point3d *> *freeList;
    float y;
    static const int chunkSize=250;
    float z;
};
```

<code>Nonstatic data members </code>在 class object中的排列顺序将和其声明的顺序一致，

任何中间介入的static data members如 freeList 和 chunkSize都不会被放进对象布局中。

Cpp Standard要求，在同一个access section中，members的排列只需要符合“较晚出现的members在 class object 中有较高的地址”这一条件即可。也就是说，members之间不需要连续排列。

那什么东西可能介于被声明的members之间呢？

members的边界调整就可能需要填补一些bytes。

编译器还可能会合成一些内部使用的data members，以支持整个对象模型。vptr就是这样的东西，Cpp standard允许编译器把那些内部产生出来的members自由地放在任何位置上，**甚至在那些被程序员声明出来的members之间**。

同时，Cpp standard也允许编译器将多个access sections之中的data members自由排列，不必在乎它们出现在class声明中的顺序。

目前各家编译器都是把一个以上的access sections连锁在一起，依照声明顺序成为一个连续区块。

Access sections的多寡并不会带来额外负担。

以下提供一个template function用来判断哪一个section先出现。

```cpp
template <class class_type,
		 class data_type1,
		 class data_type2>
char *access_order (
             data_type1 class_type::*mem1,
             data_type2 class_type::*mem1,
         )
{
	assert(mem1!=mem2) ;
   	return mem1<mem2?"mem1 first":"mem2 first" ;
}
```

```cpp
access_order(&Point3d::z,&Point3d::y);
```

于是class_type会被绑定为Point3d，而data_type1和data_type2会被绑定为float。