# Cpp对象模式

Cpp中，Class data member分为 static 、nonstatic，Class member functions分为 static、nonstatic、virtual三种。

先给出一个Point声明

```cpp
class Point
{
public:
    Point(float xval) ;
    virtual ~Point() ;
    float x() const ;
    static int PointCount() ;
protected:
    virtual ostream &print(ostream &os) const ;
    
    float _x ;
    static int _point_count ;
} ;
```

 以下的讨论都基于以上的Class声明。

首先要讨论的是，在机器中如何模塑出各种 data members 和 function members ；

## 简单对象模型

目的：尽量减少Cpp编译器的设计复杂度

缺点：降低空间和执行期的效率。

在此模型中，**一个object由一系列的slots组成**，每一个slot指向一个member。

Members按照其声明顺序，被指定一个slot（指针） ；

在此模型下，**members本身并不在object中**，object中只有指向member的指针。

优点：可以避免因members类型不同引起的存储空间调整问题。



## 表格驱动模型

目的：对所有的Class的所有Obj都有 **一致的表达方法**

（简单对象模型中的表达方式会因为 members 数量变化而变化）





![Simple_oo_1](https://ling_boogie.gitee.io/peasonal_pic/cpp_base/Simple_oo_2.png)

（并没有实际应用到真正的Cpp编译器身上，但是成为支持virtual function的一个有效方案。）



## Cpp对象模型

在此模型中，

Nonstatic data members被配置在每一个class obj之中，

static data members被存放在 **个别的class obj** 之外。

static 和 nonstatic function members 被存放在 **个别的class obj** 之外。

Virtual function则以两个步骤支持：

1. 每一个class产生一堆指向 virtual function 的指针 ，放在 **某个表格** 之中，此表格就是 <code>virtual table</code> （vtbl）。
2. 每个 class obj 安插一个指针，指向相关的 vtbl 。此指针被称为 vptr 。**vptr的设定和重置** 都需要由每一个class的 constructor , destructor , copy assignment 运算符自动完成。
3. 每一个 class 需要关联一个 **type_info obj** ，经由 vtbl 被指出，通常放在表格的第一个 slot 。



![cpp_oo](https://ling_boogie.gitee.io/peasonal_pic/cpp_base/cpp_oo.png)

缺点：如果应用代码本身未改变，但是用到的class obj和nonstatic data members有所修改，那么应用代码同样需要重新编译。

（双表格模型则有较大的弹性）

## 虚拟继承

bass class不管在继承串链中被 virtual derived 多少次，**永远都只会存在一个实例** 。

Cpp最初的继承模型并不采用任何间接性，

base claaa subobject 的 data members 被直接放置在 derived class obj 中。 

Cpp 2.0起新导入了 virtual base class。原始模型是在class obj中为每一个有关联的 virtual base class加上一个指针。

其他演化出来的模型不是导入一个virtual base class table，就是扩充原已存在的virtual table，以便维护每一个virtual base class的位置。



## 对象模型如何影响程序

不同的对象模型，会导致“现有的程序代码必须修改”以及“必须加入新的程序代码”两个结果 。

```cpp
X foobar()
{
    X xx ;
    X *px = new X ;
    // foo is a virtual function
    xx.foo() ;
    px->foo() ;
    delete px ;
    return xx ;
}
```

此函数有可能在内部被转化为

```cpp
void foobar(X &_result)
{
    //构造_result
    //_result 用来取代 local xx ...
    _result.X::x() ;
    //扩展X *px = new X ;
    px = _new(sizeof(X)) ;
    if (0!=px) 
        px->X::X() ;
    //xx不是一个指针或引用
    //扩展xx.foo()但不使用 virtual 机制
    //以 _result 取代 xx
    foo(&_result) ;
    //px是一个base 指针
    //只用virtual机制扩展
    (*px->vtbl[2])(px) ;
    //扩展 delete
    if (0!=px) {
        (*px->vtbl[1])(px) ;
        _delete(px) ;
    }
    return ;
}
```

