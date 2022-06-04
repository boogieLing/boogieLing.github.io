# go

文档：https://www.topgoer.com/go%E5%9F%BA%E7%A1%80/

## 基础

1.  = 和 :=的区别
    = 赋值， :=声明同时赋值
    **使用:=后循环体内的变量和循环体外的变量并不是同一个, 也就是作用域确实不同.** 
2. go中，很多函数会返回两个值，一个是函数本身的返回值，另一个代表函数执行是否成功的标识。可以多返回
3. error类型是一个简单的接口类型
4. Go语言局部变量分配在栈还是堆？
    Go语言编译器会自动决定把一个变量放在栈还是放在堆，编译器会做逃逸分析，当发现变量的作用域没有跑出函数范围，就可以在栈上，反之则必须分配在堆。
5. 

## goroutine

**调度原理**

1. M(machine): 代表着真正的执行计算资源，可以认为它就是os thread（系统线程）
2. P(processor): 表示逻辑processor，是线程M的执行的上下文。
3. G(goroutine): 调度系统的最基本单位goroutine，存储了goroutine的执行stack信息、goroutine状态以及goroutine的任务函数等。

## go-zero

**工具大于约定和文档的理念**，**编码自动化**

https://blog.csdn.net/yyz_1987/article/details/108367692

`go-zero` 集成了开箱即用的 **限流器** 。

1. periodlimit 
    `go-zero` 采取 **滑动窗口** 计数的方式，计算一段时间内对同一个资源的访问次数，如果超过指定的 `limit` ，则拒绝访问。当然如果你是在一段时间内访问不同的资源，每一个资源访问量都不超过 `limit` ，此种情况是允许大量请求进来的。
    1. 而在一个分布式系统中，存在多个微服务提供服务。所以当瞬间的流量同时访问同一个资源，如何让计数器在分布式系统中正常计数？ 同时在计算资源访问时，可能会涉及多个计算，如何保证计算的原子性？
    2. go-zero 借助 redis 的 incrby 做资源访问计数采用 lua script 做整个窗口计算，保证计算的原子性

后续处理

如果在服务某个时间点，请求大批量打进来，`periodlimit` 短期时间内达到 `limit` 阈值，而且设置的时间范围还远远没有到达。后续请求的处理就成为问题。

`periodlimit` 中并没有处理，而是返回 `code` 。把后续请求的处理交给了开发者自己处理。

1. 如果不做处理，那就是简单的将请求拒绝
2. 如果需要处理这些请求，开发者可以借助 `mq` 将请求缓冲，减缓请求的压力
3. 采用 `tokenlimit`，允许暂时的流量冲击

----

# make和new的区别

来源：https://www.zhihu.com/question/446317882/answer/2245768201

make

1.  分配和初始化内存。
2. 只能用于slice, map和chan这3个类型，不能用于其它类型。
3. 返回对象实例

new

1. 分配内存。内存里存的值是对应类型的[零值](https://zhuanlan.zhihu.com/lesson3)。只有一个参数。
2. 参数是分配的内存空间所存储的数据类型，Go语言里的任何类型都可以是new的参数，比如int， 数组，结构体，甚至函数类型都可以。
3. 返回的是指针。

----

# channel实现原理和作用流程

来源：https://zhuanlan.zhihu.com/p/342903365

## 写数据

![img](https://pic2.zhimg.com/80/v2-b22c56fe24f0a41505d02bbeb4fc7a21_1440w.jpg)



## 读数据

![img](https://pic1.zhimg.com/80/v2-3182b668de3a5a88881322618abce790_1440w.jpg)

- 向已经关闭的channel中写入数据会发生Panic
- 关闭已经关闭的channel会发生Panic
- 关闭值为nil的channel会发生Panic 

---

# go内存模型

**Go内存模型指定了在何种条件下可以保证一个Goroutine中读取变量时能观察到在不同Gorouting中写入相同变量所产生的值**

主要概念：

1. 指令重排：通常是编译器或者运行时环境为了优化程序性能而采取的手段。
2. 同步、异步
3. 互斥：多方在同一时刻只有一方能进入临界区。互斥是一种特殊的同步。

## 问题出现

由于CPU指令重排和多级Cache的存在，在多核访问一个变量的场景中做同步就需要很多工作。

> 不同 CPU 架构（x86/amd64、ARM、Power 等）的处理方式也不一样，再加上编译器的优化也可能对指令进行重排，所以编程语言需要一个规范，来明确多线程同时访问同一个变量的可见性和顺序（ Russ Cox 在麻省理工学院 6.824 分布式系统 Distributed Systems 课程 的一课，专门介绍了相关的知识）。在编程语言中，这个规范被叫做内存模型。

## 内存模型的目的

1. 向语言开发者提供一种保证，以便在做设计和开发程序时，面对同一个数据同时被多个 goroutine 访问的情况，可以做一些串行化访问的控制，比如使用 Channel 或者 sync 包和 sync/atomic 包中的并发原语。
2. 允许编译器和硬件对程序做一些优化。这一点其实主要是为编译器开发者提供的保证，这样可以方便他们对 Go 的编译器做优化。

## 重排以及可见性问题

由于指令重排，代码并不一定会按照你写的顺序执行。
举个例子，当两个 goroutine 同时对一个数据进行读写时，假设 goroutine g1 对这个变量进行写操作 w，goroutine g2 同时对这个变量进行读操作 r，那么，如果 g2 在执行读操作 r 的时候，已经看到了 g1 写操作 w 的结果，那么，也不意味着 g2 能看到在 w 之前的其它的写操作。
(看到了w写，但可能看不到更早之前的写？)
这是一个反直观的结果，不过的确可能会存在。、

```go
var a string
var done bool

func setup() {
  a = "hello, world"
  done = true
}

func main() {
  go setup()
  for !done {
  }
  print(a)
}
```

主 goroutine main 即使观察到 done 变成 true 了，最后读取到的 a 的值仍然可能为空。
更糟糕的情况是，main 根本就观察不到另一个 goroutine 对 done 的写操作，这就会导致 main 程序一直被 hang 住。

**main为什么观察不到呢，他们不都共享内存区吗?**
**有一个猜测，在多核cpu中，由于cpu cache的存在，多个cpu对内存的读写，并不保证另外的cpu也能立即观察到**

## Happens-before保证

**第一个保证**：在一个 goroutine 内部，程序的执行顺序和它们的代码指定的顺序是一样的，即使编译器或者 CPU 重排了读写顺序，从行为上来看，也和代码指定的顺序一样。

**第一个保证的问题**：Go 只保证 goroutine 内部重排对读写的顺序没有影响

设计目的：如果两个 action（read 或者 write）有明确的 happens-before 关系，你就可以确定它们之间的执行顺序（或者是行为表现上的顺序）。

### Happens-before的定义

> 如果事件 e1 happens before 事件 e2，那么，我们就可以说事件 e2 在事件 e1 之后发生（happens after）。如果 e1 不是 happens before e2， 同时也不 happens after e2，那么，我们就可以说事件 e1 和 e2 是同时发生的。
>
> （要么前，要么后，否则认为同时）

### 保证能观察

如果要保证对“变量 v 的读操作 R”能够观察到一个对“变量 v 的写操作 W”，并且 R 只能观察到 W 对变量 v 的写，没有其它对 v 的写操作

也就是说，我们要保证 R 绝对能观察到 W 操作的结果，那么就需要同时满足两个条件：

1. W happens before R；
2. 其它对 v 的写操作（w2、w3、w4, …） 要么 happens before W，要么 happens after R，绝对不会和 W、R 同时发生，或者是在它们之间发生。

**对于单个的 goroutine 来说，它有一个特殊的 happens-before 关系，happens-before 的关系和代码编写的顺序是一致的。**

在 goroutine 内部对一个局部变量 v 的读，一定能观察到最近一次对这个局部变量 v 的写。

如果要保证多个 goroutine  之间对一个共享变量的读写顺序，在 Go 语言中，可以使用并发原语为读写操作建立 happens-before 关系，这样就可以保证顺序了。

### 细节补充

1. 在 Go 中，对变量进行零值的初始化就是一个写操作。
2. 如果对超过机器 word（64bit、32bit 或者其它）大小的值进行读写，那么，就可以看作是对拆成 word 大小的几个读写无序进行。
3. Go 并不提供直接的 CPU 屏障（CPU fence）来提示编译器或者 CPU 保证顺序性，而是使用不同架构的内存屏障指令来实现统一的并发原语。

Go提供的happens-before 关系：https://blog.csdn.net/Zerore/article/details/120321985

1. init 函数，应用程序的初始化是在单一的 goroutine 执行的。如果包 p 导入了包 q，那么，q 的 init 函数的执行一定 happens before p 的任何初始化代码。
2. goroutine，启动 goroutine 的 go 语句的执行，一定 happens before 此 goroutine 内的代码执行。
3. Channel
4. Mutex/RWMutex
5. WaitGroup，Wait 方法等到计数值归零（wg.Done() ）之后才返回
6. once调用，对于 once.Do(f) 调用，f 函数的那个单次调用一定 happens before 任何 once.Do(f) 调用的返回。就是函数 f 一定会在 Do 方法返回之前执行。
7. atomic，**但Go 内存模型的官方文档并没有明确给出 atomic 的关系保证。**

---

# go内存逃逸分析

在Go里面定义了一个变量，到底是分配在堆上还是栈上，Go官方文档告诉我们，不需要管，他们会分析并管理，其实这个分析就是逃逸分析

**当一个对象的指针被多个方法或线程引用时，我们称这个指针发生了逃逸。**

变量在栈或是堆上分配内存，是由[编译器](https://so.csdn.net/so/search?q=编译器&spm=1001.2101.3001.7020)决定的

在 build 的时候，通过添加 -gcflags "-m" 编译参数就可以查看编译过程中的逃逸分析

## 基本原则

> 堆适合不可预知的大小的内存分配。但是为此付出的代价是分配速度较慢，而且会形成内存碎片。
>
> 堆分配带来的另一大问题是gc，gc会消费cpu的时间。减少内存逃逸则直接降低了gc处理的时间。
>
> 栈内存分配则会非常快，栈分配内存只需要两个CPU指令：“PUSH”和“RELEASE”分配和释放；
>
> 而堆分配内存首先需要去找到一块大小合适的内存块。之后要通过垃圾回收才能释放。

1. 如果函数外部没有引用，此对象优先处于栈中
2. 如果函数外部存在引用，此对象必处于堆中
3. 如果对象过大，有可能分配并处于堆上；有的编译器会LAZY未使用的局部对象，这样既不会在堆上也不会在栈上；或者优化只读的局部对象，LAZY掉未使用的部分，使得栈就可以容纳对象。
4. 如果不能确定对象的使用空间，有可能分配在堆上

## 案例

指针逃逸，i 会发生 **moved to heap**

```go
  1 package main
  2 
  3 func main() {
  4     test()
  5 }
  6 
  7 func test() *int {
  8     i := 1
  9     return &i
10 }
```

栈空间不足的逃逸，变量过大，则直接分配到堆上。

```go
func main() {
    stack()
}
 
func stack() {
    s := make([]int, 100000, 100000)
    s[0] = 1
}
```

动态类型逃逸，dynamic函数会被inline处理掉

所以变量 i 声明的地方由代码中的第8行被标识到第4行。因为dynamic函数的返回值interface{}是一个动态类型，在编译期不同确定动态类型的实际使用空间，因此 i 被逃逸分配到heap上了。

```go
  3 func main() {
  4     dynamic()
  5 }
  6 
  7 func dynamic() interface{} {
  8     i := 0
  9     return i
 10 }
```

闭包逃逸，f保存了a和b的引用，不仅变量a、b发生了逃逸，返回值函数对象也被分配到堆上了

```go
  3 func main() {
  4     f := fibonacci()
  5     for i := 0; i < 10; i++ {
  6         f()
  7     }
  8 }
  9 
 10 func fibonacci() func() int {
 11     a, b := 0, 1
 12     return func() int {
 13         a, b = b, a+b
 14         return a
 15     }
 16 }
```

---

# go map并发问题和解决

官方的[faq](https://golang.org/doc/faq#atomic_maps)里有说明，考虑到有性能损失，map没有设计成原子操作，在并发读写时会有问题。

来源：http://xusenqi.site/2020/03/08/golang%E4%B8%AD%E7%9A%84map%E5%B9%B6%E5%8F%91%E8%AF%BB%E5%86%99%E9%97%AE%E9%A2%98/

使用map的正确姿势：https://learnku.com/articles/27691

如果map由多[协程](https://so.csdn.net/so/search?q=协程&spm=1001.2101.3001.7020)同时读和写就会出现 fatal error:concurrent map read and map write的错误

多个协程同时写也会出现fatal error: concurrent map writes的错误

问题补充：https://blog.csdn.net/oguro/article/details/110494602

## 解决

1. 加[sync.RWMutex](https://golang.org/pkg/sync/#RWMutex)来保护map
2. 直接使用sync.Map，他在内部使用了大量的原子操作来存取键和值，并使用了 read 和 dirty 二个原生 map 作为存储介质，具体实现流程可阅读相关源码。

---

# map自定义键值的内部成员不能直接赋值

来源：https://blog.csdn.net/qq_40169189/article/details/113051486

---

# 判断channel已经关闭

来源：https://www.zhihu.com/question/450188866/answer/1792300859

**golang 的 channel 不应该直接对 channel 本身判断是否 close** 

```go
package main

import (
	"context"
	"fmt"
	"time"
)

var g_cnt = 1

func product(ctx context.Context, cancelFunc context.CancelFunc, c chan int) {
	for {
		select {
		case <-ctx.Done():
			fmt.Printf("product ctx.Done\n")
			return
		default:
			g_cnt++
			fmt.Printf("product default:%v\n", g_cnt)
			c <- g_cnt
			if g_cnt > 3 {
				fmt.Println("close channel && ctx cancel")
				close(c)
				cancelFunc()
			}
		}

	}
}
func speakMemo(ctx context.Context, c chan int) {
	for {
		select {
		case <-ctx.Done():
			fmt.Printf("ctx.Done\n")
			return
		case v, _ := <-c:
			fmt.Printf("read channel: %v\n", v)
		default:
			//fmt.Println("exec default func")
		}
	}
}

func main() {
	c := make(chan int)
	rootContext := context.Background()
	ctx, cancelFunc := context.WithCancel(rootContext)
	go product(ctx, cancelFunc, c)
	go speakMemo(ctx, c)
	time.Sleep(time.Second * 5)
}
```

---

# channel优雅关闭

来源：https://www.zhihu.com/question/450188866/answer/1792300859

**永远不要尝试在读端关闭 channel ；永远只允许一个 goroutine（比如，只用来执行关闭操作的一个 goroutine ）执行关闭操作；**



1. 关闭一个 channel 直接调用 close 即可，但是关闭一个已经关闭的 channel 会导致 panic，怎么办？panic-recover 配合使用即可。
2. 可以使用 `sync.Once` 来确保 `close` 只执行一次。
3. 设计事件同步，可以使用 `sync.WaitGroup` 来同步这个关闭事件

# GMP调度模型

来源：https://zhuanlan.zhihu.com/p/413970952

- G：Goroutine
- M：Machine，操作系统的执行线程，P最多可以创建10000个线程，最多只有GOMAXPROCS个活跃线程（与核数一致），这样不会频繁地切换线程上下文
- P：调度器，处理M与G的关系，调度线程上执行的G，可以让出那些等待资源（如网络、IO）的G，提高运行效率，同时提供M执行所需要的上下文环境以及资源

---

# gc过程

来源：https://zhuanlan.zhihu.com/p/92210761

---

# 当一个goroutine被网络IO阻塞住后，它对应的线程会被阻塞吗？

来源：https://www.zhihu.com/question/27207566/answer/524984768

答案是网络IO不会。

原因是标准库中的net包对网络IO做了封装，底层实际基于epoll机制，并不会block线程。

但是其他system call就会真的block线程了，比如文件IO

这时候线程会被block, 当前调度器上剩下的协程队列会被转移到新的线程中执行，至于新线程是怎么来的，可能是用的runtime线程池里的空闲线程，也可能是新创建的。 

---

# waitgroup+channel控制goroutine并发数量

来源：https://www.jianshu.com/p/d3257b6a4c4a

