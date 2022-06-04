# 关于Cpp对象

## 前言

在C语言中，“数据”和“处理数据的操作”是（或者说应当是）**分开声明**的，

也就是说，语言层面上并没有支持”数据与函数“之间的关联性/

这种程序方法称为 **程序性** 的（procedural），

由一组”分布在各个以功能为导向的函数中“的算法所驱动，

处理的是共同的外部数据。

所以在C语言中经常能见到以下的函数模式。

```c
void foo_1(struct point *output, const struct point *input)
{
    output.x=input.x ;
    ...
    return ;
}
```

举个例子，如果现在我们需要一个描述**三维坐标**的数据类型。

在C中会像这样：

```c
typedef struct point3d
{
    float x ;
    float x ;
    float x ;
}Point3d ;
```

想要打印一个Point3d，我们可能需要定义以下的函数：

```c
void Point3d_print(const Point3d *input)
{
	printf("(%g %g %g)",Point3d->x,Point3d->y,Point3d->z) ;
}
```

但是在Cpp中，可能会采用独立的**”抽象数据类型“**

（ADT——abstract data type）

```cpp
class Point3d
{
public:
	Point3d(float x=0.0,float y=0.0,float z=0.0):x_(x),y_(y),z_(z){}
    float x(){return x_;}
   	...
private:
    float x_ ;
    float y_ ;
    float z_ ;
};
inline ostream& operator<<(ostream &os,const Point3d &input)
{
    os<<"("<<input.x()<<","<<input.y()<<","<<input.z()<<")" ;
}
signed main()
{
    Point3d pt ;
    cout<<pt ;
}
```

## 奇怪的角度

看完上述例子可能会认为两者其实没有太大不同

但是在Cpp的ADT之后，你还可以对<code>Point3d</code>成员的坐标的数据类型进行**参数化**

甚至连坐标数量也参数化。

很明显，这不止是在程序风格上有着显著不同，在对于程序的思考上也有明显差异。

C的优势在于它的**精简**，执行起来快且有力。

## 封装后的布局成本

现实是 ：<code>class Point3d </code>并没有增加成本。

- 三个data members 直接 **内含** 在每一个 class obj中，与C中情况相同。
- 而member functions 虽然也是含在class的声明之内，却不会出现在 obj 之中。每一个non-inline member function **只会诞生一个函数实例**。 
- 每一个 **拥有零个或者一个定义** 的inline function 则会在每一个使用者上产生一个函数实例。

**结论**

Cpp在布局上主要的额外负担是由 virtual 引起的。

- virtual function
- virtual base class

此外还有一些多重继承下的额外负担，发生在”一个derived class 与其第二或后继的base class的转换“之间。

## 





