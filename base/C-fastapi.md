# fastapi

FastAPI是一款现代化、高性能的Web框架，用于构建基于Python3.6及以上的API

1. 非常高的性能，与NodeJS和Go不相上下，是最快的Python框架之一；
2. 开发快，使用简单
3. 自带可视化文档和postman功能 swagger ui
4. 

FastAPI的速度得益于使用了Starlette——一个轻量级的ASGI框架。

> ASGI，全称为Asynchronous Server Gateway Interface，为了规范支持异步的Python Web服务器、框架和应用之间的通信而定制，同时囊括了同步和异步应用的通信规范，并且向后兼容WSGI。由于最新的HTTP协议支持异步长连接，而传统的WSGI应用支持单次同步调用，即仅在接受一个请求后返回响应，从而无法支持HTTP长轮询或WebSocket连接。在Python3.5增加async/await特性之后，基于asyncio和协程的异步应用编程变得更加方便。ASGI协议规范就是用于asyncio框架的最低限度的底层服务器/应用程序接口。

**异步非阻塞I/O & 协程**

阻塞I/O，非阻塞I/O，I/O多路复用都属于同步I/O。

而异步I/O则不一样，当进程发起I/O操作之后，就直接返回再也不理睬了，直到kernel发送一个信号，告诉进程说I/O完成。

在这整个过程中，进程完全没有被阻塞。在非阻塞I/O中

虽然进程大部分时间都不会被阻塞，但是它仍然要求进程去主动的查询，并且当数据准备完成以后，也需要进程主动的再次调用recvfrom()来将数据拷贝到用户内存。

相对于线程，协程是程序级的I/O调度，是对一个线程进行分片，使得线程在代码块之间来回切换执行，而非逐行执行

因此能够支持更快的上下文切换。

协程本身并不能实现高并发，但与I/O切换结合后能够大大提高性能。

每当发生I/O，自动切换协程，让出CPU资源，即可减少高并发场景下服务的响应时间。

因此，结合async/await语法，将代码块定义为协程，使用异步服务器即可实现程序级I/O切换和协程调度。

**事件循环**

因为Python是单线程的，同一时间只能执行一个方法，所以当一系列的方法被依次调用的时候，Python会先解析这些方法，把其中的同步任务按照执行顺序排队到一个地方，这个地方叫做执行栈。

主线程之外，还存在一个"任务队列"(task queue)。

当遇到异步任务时，异步任务会被挂起，继续执行执行栈中任务，
等异步任务返回结果后，再按照执行顺序排列到"事件队列中"。

一旦"执行栈"中的所有同步任务执行完毕，系统就会读取"任务队列"，看看里面有哪些事件。

如果有，就将第一个事件对应的回调推到执行栈中执行，若在执行过程中遇到异步任务，则继续将这个异步任务排列到事件队列中。

主线程每次将执行栈清空后，就去事件队列中检查是否有任务，如果有，就每次取出一个推到执行栈中执行，这个过程是循环往复的，这个过程被称为Event Loop，即事件循环。

由于异步非阻塞框架基本为单线程运行，因此要利用协程实现事件循环。FastAPI推荐使用uvicorn来运行服务，uvicorn是基于uvloop和httptools构建的闪电般快速的ASGI服务器。

Python3.5+的标准库asyncio提供了事件循环用来实现协程，并引入了async/await关键字语法以定义协程。

同是异步非阻塞框架的Tornado通过yield生成器实现协程，它自身实现了一个事件循环，其在Python3之后也支持async/await关键字语法，以使用标准库asyncio。

**而FastAPI则是利用了uvloop，相对于asyncio，更进一步地提升了速度。**

uvloop是用Cython编写的，并建立在libuv之上。

libuv是一种高性能的、跨平台异步的I/O类库，nodejs也使用到了它。由于nodejs是如此的广泛和流行,可以知道libuv是快速且稳定的。

uvloop实现了所有的asyncio事件循环APIs。

高级别的Python对象包装了低级别的libuv结构体和函数方法。 

继承可以使得代码保持DRY(不要重复自己)，并确保任何手动的内存管理都可以与libuv的原生类型的生命周期保持同步。