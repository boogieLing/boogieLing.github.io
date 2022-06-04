# Linux

## CPU占用过高排查

来源：https://www.coonote.com/linux-note/linux-troubleshooting-high-cpu-load.html

线上故障主要会包括cpu、磁盘、内存以及网络问题，而大多数故障可能会包含不止一个层面的问题，所以进行排查时候尽量四个方面依次排查一遍

同时例如 jstack、jmap 等工具也是不囿于一个方面的问题的，基本上出问题就是df、free、top 三连，然后依次jstack、jmap伺候，具体问题具体分析即可。

---

## vmstat排查问题

来源：https://www.cnblogs.com/ggjucheng/archive/2012/01/05/2312625.html

---

## 火焰图

http://www.ruanyifeng.com/blog/2017/09/flame-graph.html

---

## 切换目录

切换目录除了可以cd之外，还可以利用目录栈。

**pushd, popd, dirs**

目录栈顶端永远是当前目录。

dirs显示目录栈的内容，常用<code>dirs -v</code>

pushd将某个目录压入目录栈

```shell
pushd DayTopic/
```

pushd可以不带参数，将目录栈最顶层的两个目录进行交换。

使用**`push + 目录`**的方式将这个几个路径添加到目录栈中；

然后，再用**`push +/-n`**在不用的目录间快速切换。

popd可以将栈顶目录出栈，也可以切换目录。

------

## 服务器

查看服务器上各个状态的统计数量：

```shell
netstat -ant | awk '/^tcp/ {++y[$NF]} END {for(w in y) print w, y[w]}'
```

单独查看TIME_WAIT：

```shell
ss -nat | grep TIME-WAIT
```

------

## GDB调试

g++编译总结：https://www.cnblogs.com/bugutian/p/4490902.html

GDB用以在Linux下调试可执行文件（不一定是C与C++所编写生成的文件）

**list**

用来列出所调试的程序的代码

list+linenumber

用以列出指定行数附近的代码

list+function

列出函数上下文的源程序

**run & quit**

一个运行一个退出GDB调试

**break**

设置断点

break filename:lineNumber 

在指定文件的指定行打断点

break filename:function 

在指定文件的指定函数入口打断点

**continue**

用来在遇到断点时继续执行代码至下一个断点

**next**

单步执行程序代码

单步执行不会进入函数内部

**step**

一般用来单步执行程序时进入函数内部

------

**disassemble**

用来查看汇编代码

**info f**

打印出更详细的当前栈层的信息

**info register**

打印出所有寄存器的值

**其他常用手段**

如果想很漂亮的输出结构体请设置

```shell
set print pretty on
```

查看指定的寄存器的值

```shell
p $eip
```

------

## 共享内存相关api 

1. 新建共享内存shmget
   shmget(key_t key,size_t size,int shmflg); 

2. 连接共享内存到当前进程的地址空间shmat 
   shmat(int shm_id,const void *shm_addr,int shmflg); 

3. 当前进程分离共享内存shmdt 

   int shmdt(const void *shmaddr); 

4. 控制共享内存shmctl 
   int shmctl(int shm_id,int command,struct shmid_ds *buf); 

---

## 查看硬盘空间

**df**

df -h 好看的格式，常用

df -i 查看分区的inode的情况，inode table是data area的索引表.

**du 更常用**

du -a . 列出当前所有文件和目录容量大小

du -s . 只显示总计大小

du -h . 好看的格式，带单位

------

## 查看CPU内存和系统版本

**cpu**

cat /proc/cpuinfo

top && dstat

**内存**

cat /proc/meminfo

free

**系统版本**

uname -r

cat /etc/rehat-release

-----

## 查看网络连接信息

netstat -anp

a-所有选项

n-拒绝别名

p-显示建立连接的程序名

**查看某个端口是否占用**

netstat -anp | grep 8080

**top也能查看，信息更加详细**

进入top之后 shift+m 就可以对占用进行排序

------

## 文件权限

rwx rwx r-x

第一个表示u-所有者，第二个表示g-所属组，第三个表示o-其他用户

r-x表示的是只有可读可执行，对于不能w，用的一个-来占位，**而不是省略不写**

存储情况：111111101

也可以转化为三个八进制数字：775

**umask -S**

查看 默认创建的文件具有什么权限

但是查看到的八进制码需要取反才能读到真正存储的权限

**chmod 修改、增删权限**

**chown 修改文件所属者**

**chagrp 修改文件所属组**

------

## grep

查找某个字符串

grep 'qwq' test.txt

不区分大小写

grep -i 'qwq' test.txt

带行号

grep -n 'qwq' test.txt

不要正则

grep -F 'qwq' test.txt

使用扩展正则

grep -E 'qwq|QWQ' test.txt

只统计，不显示内容

grep -c 'qwq' test.txt

------

## awk

一个强大的文本分析工具，相对于grep的查找、sed的编辑，awk都能做而且还能进行更强大的数据分析

**基本格式**

  awk  [options]  'pattern + action'  filename

-  options : -F : 指明输入时用到的字段分隔符 
  -v var=VALUE : 自定义变量 

- pattern: 正则表达式

- action：某些计算操作

- filename：文件名称

**变量赋值**

  awk ' { count++ ; print $0; } END { print "user count is " , count } ' /etc/passwd

-----

## Linux查看日志方法

tail、head、 cat、tac、sed、less、echo

------

## vim替换和查找字符串

vi/vim 中可以使用 :s 命令来替换字符串

:s/vivian/sky/ 替换当前行第一个 vivian 为 sky 

:s/vivian/sky/g 替换当前行所有 vivian 为 sky 

:.,$ s/str1/str2/g 用字符串 str2 替换正文当前行到末尾所有出现的字符串 str1 



/后跟查找的字符串。vim会显示文本中第一个出现的字符串。
?后跟查找的字符串。vim会显示文本中最后一个出现的字符串。

---

## linux下如何运行一个执行文件

元戎

详解：https://blog.csdn.net/yu132563/article/details/79446351

---

## Linux看门狗机制

扫盲：https://blog.csdn.net/ericstarmars/article/details/81750919

其实是搜redis看门狗方案搜出来的。