#include <bits/stdc++.h>
using namespace std ;

class A
{
public:
	A() {
		this->foo() ;
	}
	virtual void foo() {
		printf("A virtual\n");	
	}
};
class B1:public virtual A
{
public:
	B1() {
		this->foo() ;
	}
	virtual void foo() {
		printf("B1 virtual\n");	
	}
};
class B2:public virtual A
{
public:
	B2() {
		this->foo() ;
	}
	virtual void foo() {
		printf("B2 virtual\n");	
	}
};
class C:public B1, public B2
{
public:
	C() {
		this->foo() ;
	}
	virtual void foo() {
		printf("C virtual\n");	
	}
};



signed main(int argc, char const *argv[])
{
	C c ;
	return 0;
}