#include <bits/stdc++.h>
using namespace std ;

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
unsigned int HeapOnly::_size = 0 ;
static HeapOnly *test = HeapOnly::create(10) ;

signed main(int argc, char const *argv[])
{
	return 0;
}