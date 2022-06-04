# Python

## 多核不要使用多线程

Python为了数据安全，设计了GIL（全局解释器锁）。

而每个进程都会拥有一个GIL，进程是资源分配的最小单位。

而线程是CPU调度的最小单位，每个线程的执行方式：

1. 获取GIL
2. 执行代码直到sleep或者python虚拟机将其挂起
3. 释放GIL

某个线程想要执行就要先拿到GIL，但是如果采用多线程就会出现竞争GIL的情况。

会出现锁竞争、线程切换，消耗大量资源。

想要充分利用多核CPU，就要使用多进程，每个进程都有各自独立的GIL，互不干扰，可以做到真正意义上的并行执行。

还有一点，多核多线程比单核多线程更差，原因是单核下的多线程，每次释放GIL，唤醒的那个线程就能立刻取到GIL锁。但在多核下，某个CPU的线程释放GIL后，其他CPU的线程就会同时开始竞争，如果GIL又被原先的CPU拿到，其他CPU上被唤醒的线程就会一直醒着等到到切换时间后再进入待调度状态，而频繁的GIL释放/获取，会让大量的线程状态发生切换，但实际上他们根本不需要切换，这就是线程颠簸，严重影响效率。

---

## 爬虫面试总结

https://zhuanlan.zhihu.com/p/78180049

scarpy总结：https://www.jianshu.com/p/cecb29c04cd2

https://blog.csdn.net/ck784101777/article/details/104468780/

---

## PyQt5

https://blog.csdn.net/qq_40243295/article/details/105633221?utm_medium=distribute.pc_relevant.none-task-blog-baidujs_utm_term-0&spm=1001.2101.3001.4242



----

