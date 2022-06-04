#include <bits/stdc++.h>
using namespace std ;

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

signed main(int argc, char const *argv[])
{

	return 0;
}