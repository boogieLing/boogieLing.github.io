# 关键词带来的差异

关键词的差异主要是Cpp努力维护与C之间的兼容性而带来的 。

比如，如果Cpp独立于C，就不需要判断下面一行代码其实是一个 **函数调用** 

而不是一个函数声明

```cpp
//在此并不知道是一个declaration还是invocation
int (*pf)(1024) ;
//因为整数常量1024，知道了是一个invocation
```

在此出现了一种 “向前预览“ 的行为

而在下面的一个声明，这种行为甚至不能起作用

```cpp
// meta-language rule ;
// pq是一个declaration而不是invocation
int (*pq)() ;
```

> 当语言无法区分是一个声明还是一个表达式时，就需要一个超越语言范围的规则，而该规则会将上述式子判断为一个”声明“

## 困扰

”什么时候一个人应该使用struct取代class？”

“当它当一个人感觉比较好的时候”

以上是 **Stanley B.Lippman** 的原话。

当人们以及在教科书中说到struct的时候，他们的意思是一个数据集合体，没有private data，也没有data的相应操作。亦即纯然的C用法。

在C这一边，这个关键词的设计理由因其用法而存在；

而在Cpp，选择struct或class“作为关键词，并用以导入ADT”的理由，是希望从此比较健全。

在两者之间有一个观念上的差异，但是，关键词本身并不提供这种差异。

在cfront之中，两个关键词在语义分析器中是用 **共享的AGGR** 替换的。

而在Foundation项目中，ALF层次结构则保留了程序员真正使用的关键词。

真正的问题并不在于所有“使用者自定义类型”的声明是否必须使用相同的关键词，问题在于使用class或struct关键词是否可以给予“类型的内部声明“以 **某种承诺** 。

当然，”不一致性“是一种错误的语言用法。比如下面，一个变量被矛盾地声明为static和extern

```cpp
static int foo ;
extern int foo ;
```

这组声明对于foo的存储空间造成了矛盾。

但是struct与class并不会造成这样的矛盾。

class的真正特性是由声明本身来决定的 。”一致性用法“只不过是一种编写风格上的问题而已 。

如果这个语言只支持其中一个关键词，肯定可以省掉许多混淆和迷惑。

但是Cpp要支持现存的C程序代码，就不得不支持struct。

而引入新的关键词class也同样令人满意，因为引入的不只是关键词，还有它所支持的封装和继承的哲学。

## 策略性正确的struct

在C中，有些巧妙的设计会成为Cpp程序的陷阱。

例如把单一元素的数组放在一个struct的尾部，于是每个struct obj都能拥有可变大小的数组。

``` c
struct mumble
{
    char pc[1] ;
};
//从文件或标准输入装置中取得一个字符串
//为struct本身和该字符串配置足够的内存
struct mumble *pmumbl = (struct mumble *)\
    malloc(sizeof(struct mumble) + strlen(string) + 1) ;
strcpy(&mumble.pc,string) ;
```

如果改用class在声明，而该class是

1. 指定了多个access sections，皆内含数据 ；
2. 从另一个class中派生而来；
3. 定义了一个或者多个virtual function；

这就不一定能顺利转化！

Cpp中凡处于同一access section中的数据，必定保证以其声明顺序出现在内存布局中。

然而被放置在多个access section中的各个数据，排列顺序就不一定了。

需视protected data members被放在private data members的前面还是后面而定。

（放在前面才行）

同样地，base classes和derived classes的data members的布局也没有固定的顺序。

> 如果你需要一个相当复杂的Cpp class的部分数据，使他拥有C声明的模样，那么最好将此部分抽取出来成为一个独立的struct声明。

不推荐继承，因为某些邪恶的编译器（如Microsoft C++）在支持Virtual function的机制中对class的继承布局做了一些变化。

组合，而非继承，才是把C和Cpp结合在一起的 **唯一可行方法**

（conversion运算符将会使得这个工作变得非常便利）

## 合理用途

当需要保证某部分数据拥有与C兼容且固定的空间布局时，可以使用struct声明将数据封装起来。

然而这项保证也只会在组合的情况下存在，如果是继承而不是组合，编译器会决定是否应该有额外的data members被 ”安插“ 到base struct subobject之中。