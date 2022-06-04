# Effective C++

## 拒绝隐式转换

```cpp
template <typename T>
class A1 {
public:
	A1 (int n):_arr(n) {
		for (int i=0 ; i<n ; ++i) {
			this->_arr[i] = i ;
		}
	}
	T &operator[](int index) {
		return this->_arr[index] ;
	}
	bool operator==(const A1<int> &rhs) {
		printf("A1 opeartor==\n");
		return true ;
	}
private:
	vector<T> _arr ;
} ;


signed main(int argc, char const *argv[])
{
	A1<int>a(10) ;
	A1<int>b(10) ;
	if (a == b[5]) {
	}
	return 0;
}
```

上述判断处不会给出任何警告，顺利通过编译，因为A1有一个单参构造函数，编译器允许以b[5]临时构建出一个A1 obj来与a进行比较。

解决方法

1. 使用explicit 关键字修饰单参构造函数，但是要求C++11版本
2. 使用代理类，让编译器无法进行隐式类型转换。
    原理：编译器只能对参数进行一次隐式转换。

代理类方法：

```cpp
template <typename T>
class A1 {
public:
	class A1sz {
	public:
		A1sz(int n):_thesz(n) {}
		int size() const {
			return this->_thesz ;
		}
	private:
		int _thesz ;
	} ;
	A1 (A1sz n):_arr(n.size()) {
		for (int i=0 ; i<n.size() ; ++i) {
			this->_arr[i] = i ;
		}
	}
	T &operator[](int index) {
		return this->_arr[index] ;
	}
	bool operator==(const A1<int> &rhs) {
		printf("A1 opeartor==\n");
		return true ;
	}
private:
	vector<T> _arr ;
} ;
```

对于构造函数，允许一次隐式转换，从int转为A1sz，但是对于operator==就不允许从int转为A1sz，再从A1sz转为A1了。

---

## 动态二维数组

代理类的初级应用

```cpp
template <typename T>
class B2D {
public:
	class B1D {
	public:
		B1D(int x=1) {
			this->_ptr = operator new[](x*sizeof(T)) ;
			this->_arr = static_cast<T *>(this->_ptr) ;
			for (int i=0 ; i<x ; ++i) {
				new(&(this->_arr[i])) T(i) ;
			}
		}
		T &operator[](int index) {
			return this->_arr[index] ;
		}
	private:
		T *_arr ;
		void *_ptr ;
	} ;

	B2D(int y, int x) {
		this->_ptr = operator new[](y*sizeof(B1D)) ;
		this->_arr = static_cast<B1D *>(this->_ptr) ;
		for (int i=0 ; i<y ; ++i) {
			new(&(this->_arr[i])) B1D(x) ;
		}
	}
	B1D &operator[](int index) {
		return this->_arr[index] ;
	}
private:
	B1D *_arr ;
	void *_ptr ;
};
```

---

## 引用计数+写时复制+延迟表达

类图如下，代码详见reference_count.cpp

![](https://ling_boogie.gitee.io/peasonal_pic/cpp_base/proxy1.png)

重点：

1. 内容的存储实际上是在StringValue，**RCPtr的作用是控制StringValue的计数器跳动**，最终对String来说资源通过RCPtr存取

    1. 技巧：**不给String提供任何拷贝构造函数**，而RCPtr拥有一个拷贝构造的话，编译器不会触发bitwise，从而会合成出一个有用的隐式拷贝构造函数以调用RCPtr的拷贝构造。这样既能做到语义上的String拷贝，**也能保证计数器正常跳动**。

    2. RCPtr的默认构造函数，拷贝构造函数，赋值运算符都需要一个有效的 <code>T *_pointee</code> 内存指针，而且每次都会触发一次init()，使得计数器加一

        表示这个<code>T *_pointee</code>**用以赋值**次数加一。（注意不是复制）

    3. 写时复制。

        1. 调用入口，CharProxy的=运算符

            ```cpp
            if (this->_theString._value->isShareable()) {
            //触发写时复制
            	this->_theString._value = new StringValue(this->_theString._value->data) ;
            }
            this->_theString._value->makeUnshareable() ;
            ```

        2. new 产生一个StringValue临时对象

        3. 需要调用RCPtr的=运算符，但在那之前需要进行隐式转换，于是先默认构造出一个临时的RCPtr，**此临时对象拿着新开辟的空间。**

        4. 临时的RCPtr构建完毕，此时计数为1

        5. 进入=运算符，发现临时对象的内存指针不空，**说明触发了写时复制，减少原本持有的一个计数，表示一名成员放弃控制。**

            ```cpp
            if (this->_pointee!=rhs._pointee) {
            	if (nullptr!=this->_pointee) {
            	//所管理的pointee不为空，说明触发了写时复制
            		this->_pointee->removeReference() ;
            	}
            	this->_pointee = rhs._pointee ;
            	this->init() ;
            }
            return *this ;
            ```

        6. 如果计数为0，那么原StringValue需要发生析构。

        7. **接管临时对象手上新开辟的内存空间，计数器加1，此时的this和临时对象都拿着同一内存**，合理。

        8. =运算符重载函数完成，临时对象销毁，计数器减1。

        9. 接管完成

        10. 细节：设定不再share

2. 延迟表达

    ```cpp
    const char &String::operator[](int index) const {
    	cout<<"--judge rhs--"<<'\n' ;
    	return this->_value->data[index] ;
    }
    char &String::operator[](int index) {
    	cout<<"--judge lhs--"<<'\n' ;
    	if (this->_value->isShareable()) {
    		//写时复制技术
    		this->_value = new StringValue(this->_value->data) ;
    	}
    	this->_value->makeUnshareable() ;
    	return this->_value->data[index] ;
    }
    ```

    原本的设计，**调用哪个函数只取决于调用对象是否const**，不能决议出左值和右值。

    所以把推导出是左值还是右值的操作进行延迟

3. 延迟表达具体设计

    1. ```cpp
        const String::CharProxy String::operator[](int index) const{
            return CharProxy(const_cast<String&>(*this), index);
        }
        String::CharProxy String::operator[](int index){
            return CharProxy(*this, index);
        }
        ```

        目前不管是哪个函数都将返回一个CharProxy，**将决议操作延迟到使用CharProxy处**

    2. 如果是<code>cout<<str1[2]<<'\n' ;</code>，此时str1[2]本是一个CharProxy，但是由于我们重载了char()隐式转换函数，编译器判定转换可行，于是成功判定为右值。

    3. 如果是<code>str1[1] = 'q' ;</code>，此时str1[1]也是一个CharProxy，但是由于我们把**char()转换函数设定为const，编译器将认为没有合适的隐式转换，同时CharProxy存在=运算符的重载函数，并且入参类型正确**，于是就成功地调用了=运算符，判定为左值。

    4. 这只是实现了最基本的左右值判定，对于<code> char *ch = &str[2];</code>这样的操作，将会在编译期报错，因为我们把char()设计为了const。**解决方法是再重载一个&运算符。**



---

## 开辟对象数组的正确姿势

错误姿势：

```cpp
typedef int* PEP;
int main(){
	PEP *arr = new PEP[10] ;
	for (int i=0 ; i<10 ; ++i) {
		arr[i] = new int(i) ;
		//printf("%d\n", arr[i]);
	}
	return 0;
}
```

 问题：

1. 存在的是指针数组，栈unwinding的时候不会调用析构函数，**程序员必须记得循环调用对应的析构函数来释放空间**
2. 需要的内存总量比较大，需要空间放置指针，而且**构造的时候会产生临时对象**。

正确姿势：

```cpp
#include <iostream>
using namespace std;

class Proxy
{
public:
	class A {
	public:
		int val ;
		A(int n) {
			this->val = n ;
			printf("qaq\n");
		}
		~A() {
			printf("qwq\n");
		}
	};
	Proxy(int n):_sz(n) {
		this->_ptr = operator new[](n*sizeof(A)) ;
		this->_arr = static_cast<A *>(this->_ptr) ;
		for (int i=0 ; i<n ; ++i) {
			new(&(this->_arr[i])) A(i) ;
            //placement new[]
			//printf("%d\n", (this->_arr[i]).val);
		}	
	}
	~Proxy() {
		for (int i=0 ; i<10 ; ++i) {
			this->_arr[i].~A() ;
		}
		operator delete[](this->_ptr) ;
	}
	A& operator [](int index) {
		return this->_arr[index%_sz] ;
	}
private:
	void *_ptr ;
	A *_arr ;
	const int _sz ;
};

int main(){
	Proxy t1(10) ;
	for (int i=0 ; i<11 ; ++i) {
		printf("%d\n", t1[i].val);
	}
	return 0;
}
```

目的：

1. 没有产生额外的指针占用空间，构造时也没有临时对象产生。
2. 允许在构造时传递一系列的参数值。
3. 允许在获得空间的同时传入参数给构造函数，使得这样在缺少无参构造的情况下仍能成功。
4. **不需要手动释放空间**。

---

## 是否需要提供无参构造

如果没有无参构造（需要显式给定参数以构造），那么在下列三种情况中可以会发生问题

1. 想要获得一个对象数组，但是对于数组而言，给数组指定一系列参数，**并元素在获得空间的同时传入参数以调用需参构造函数**，这种实现是比较困难的，上一个议题解决了这样的问题。
2. 对于被模板“包裹”的类（比如vector\<className\> arr），很明显没有办法指定参数来将className初始化，如果className缺乏一个无参构造函数，**那么对于设计不佳的模板而言，后果将是毁灭性的**（vector设计得很好，他应付了这种情况，虽然我也不知道怎么应付的，盲猜也是用了placement new）。
3. 对于虚拟继承链，假如virtual base class没有一个无参构造函数。我们都是知道虚拟继承的base需要在末端class才完成构造，也就是说末端class才是需要传入virtual base需要的参数，但是现在virtual base缺乏一个无参构造，这样的话链中所涉及到的所有class都需要传入参数（尽管他们并不需要调用virtual base的构造函数），这就带来了额外的开销。

鉴于种种情况，还可能会提供无意义的无参构造函数，但是这种设计会影响class的效率，有无你需要在使用到某些data member的时候测试他的合法性。假如你的无参构造函数无法提供“成功、准确地初始化相应的data”的保证，就不要提供。
**这样，编译器会在你忘记初始化的时候给予你一个报错，而不是在运行时给予你一个crash。**

---

## 单例模式实现

保证一个类仅有一个实例，并提供一个访问它的全局访问点，该实例被所有程序模块共享。

方式：

根据单例对象创建时间，可分为两种模式：饿汉模式 + 懒汉模式

**饿汉模式**

定义：指全局的单例实例在类装载时构建。

```c++
class Singleton
{
public:
	static Singleton& GetInstance();
private:
	Singleton(){}
	Singleton(const Singleton&);
	Singleton& operator= (const Singleton&);
private:
	static Singleton m_Instance;
};
Singleton Singleton::m_Instance;//类外定义

Singleton& Singleton::GetInstance()
{
	return m_Instance;
}
//函数调用
Singleton& instance = Singleton::GetInstance();
```

**懒汉模式**

定义：指全局的单例实例在第一次被使用时构建。

注意：由于实例是在使用时才被创建，因此应该注意多线程的问题。

```c++
template <typename T>
class Singleton_WY
{
private:
    struct object_creator
    {
        object_creator()
        {
            Singleton_WY<T>::instance();
        }
        inline void do_nothing() const {}
    };
    //利用类的静态对象object_creator的构造初始化,在进入main之前已经调用了instance
    //从而避免了多次初始化的问题
    static object_creator create_object_;
public:
    static T *instance()
    {
        static T obj;
        //do_nothing 是必要的，do_nothing的作用有点意思，
        //如果不加create_object_.do_nothing();这句话，在main函数前面
        //create_object_的构造函数都不会被调用，instance当然也不会被调用，
        //我的估计是模版的延迟实现的特效导致，如果没有这句话，编译器也不会实现
    // Singleton_WY<T>::object_creator,所以就会导致这个问题
        create_object_.do_nothing();
        return &obj;
    }
};
//因为create_object_是类的静态变量，必须有一个通用的声明
template <typename T>  typename Singleton_WY<T>::object_creator Singleton_WY<T>::create_object_;

```

能保证多线程下不重复初始化，又能让相互调用安全。

1. do_nothing()是必须的，因为**模版有延迟实现的特效**，如果没有do，编译器可能就不实现了
2. 将所有的instance初始化放在了main函数前面，好处是避免了多线程多次初始化的麻烦，但也限制了初始化的多样性。
3. 依靠类static变量的方式，无法按需启动，回收。
4. 每次do_nothing是无谓的消耗

```c++
//测试的例子
class Object_WY_1
{
    //其实使用友元帮助我们可以让Object_2B的构造函数是protected的，从而真正实现单子的意图
    friend class Singleton_WY<Object_WY_1>;
    //注意下面用protected，大家无法构造实例
protected:
    Object_WY_1();
    ~Object_WY_1(){};
public:
    void do_something();
protected:
    int data_wy_1_;
};

class Object_WY_2
{
    friend class Singleton_WY<Object_WY_2>;
protected:
    Object_WY_2();
    ~Object_WY_2(){};
public:
    void do_something();
protected:
    int data_wy_2_;
};

//CPP代码
Object_WY_1::Object_WY_1():
    data_wy_1_(1)
{
    printf("Object_WY_1::Object_WY_1() this:[%p] data_2b_1_ [%d].\n",this,data_wy_1_);
    Singleton_WY<Object_WY_2>::instance()->do_something();
};

void Object_WY_1::do_something()
{
    data_wy_1_+= 10000;
    printf("Object_2B_1::do_something() this:[%p] data_2b_1_ [%d].\n",this,data_wy_1_);

}

Object_WY_2::Object_WY_2():
    data_wy_2_(2)
{
    printf("Object_WY_2::Object_WY_2() this:[%p] data_2b_2_ [%d].\n",this,data_wy_2_);
    Singleton_WY<Object_WY_1>::instance()->do_something();
};

void Object_WY_2::do_something()
{
    data_wy_2_+= 10000;
    printf("Object_WY_2::do_something() this:[%p] data_2b_2_ [%d].\n",this,data_wy_2_);
}
```

---

## 限制堆上创建

```cpp
class HeapOnly
{
public:
	static HeapOnly *create(int n) {
		if (0==_size) {
			return new HeapOnly(n) ;
		} else {
			++_size ;
			return nullptr ;
		}
	}
	void destory() {
		delete this ;
	}
protected:
	HeapOnly(int n) {
		++_size ;
		_val = n ;
	}
	~HeapOnly() {
		_size = 0 ;
	}

private:
	static unsigned int _size ;
	int _val ;
};
```

---

## 











