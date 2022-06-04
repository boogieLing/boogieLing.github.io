#include <bits/stdc++.h>
using namespace std ;


template <typename T>
class RCPtr 
{
public:
	RCPtr(T *realPtr=nullptr):_pointee(realPtr) {
		this->init() ;
		printf("RCPtr constructor %p.\n", _pointee);
	}
	RCPtr(const RCPtr &rhs):_pointee(rhs._pointee) {
		this->init() ;
		printf("RCPtr copy constructor %p.\n", _pointee);
	}
	~RCPtr() {
		if (nullptr!=this->_pointee) {
			this->_pointee->removeReference() ;
		}
		cout<<"RCPtr destructor."<<'\n' ;
	}
	RCPtr &operator=(const RCPtr &rhs) {
		if (this->_pointee!=rhs._pointee) {
			if (nullptr!=this->_pointee) {
				//所管理的pointee不为空，说明触发了写时复制
				this->_pointee->removeReference() ;
			}
			printf("==change target now %p==\n", rhs._pointee);
			this->_pointee = rhs._pointee ;
			this->init() ;
			cout<<"RCPtr operator=."<<'\n' ;
		}
		return *this ;
	}
	T *operator->()const {
		return this->_pointee ;
	}
	T &operator*() {
		return this->_pointee ;
	}
private:
	T *_pointee ;
	void init() ;
};
template <typename T>
void RCPtr<T>::init() {
	if (nullptr==_pointee) {
		return ;
	}
	if (false==_pointee->isShareable()) {
		_pointee = new T(*_pointee) ;
	}
	_pointee->addReference() ;
}

class RCObj 
{
public:
	void addReference() {
		++this->_refCount ;
		cout<<"now add: "<<this->_refCount<<'\n' ;
	}
	void removeReference() {
		--this->_refCount ;
		cout<<"now rmv: "<<this->_refCount<<'\n' ;
		if (this->_refCount==0) {
			printf("RCObj delete %p.\n", this);
			delete this ;
		}
		cout<<"RCObj clear done."<<'\n' ;
	}
	void makeUnshareable() {
		this->_shareable = false ;
	}
	bool isShareable() const {
		return this->_shareable ;
	}
	int cnt() {
		return this->_refCount ;
	}
protected:
	RCObj():_refCount(0),_shareable(true) {
		cout<<"RCObj constructor."<<'\n' ;
	}
	RCObj(const RCObj &rhs):_refCount(0),_shareable(true) {
		cout<<"RCObj copy constructor."<<'\n' ;
	}
	RCObj &operator=(const RCObj &rhs) {
		cout<<"RCObj operator=."<<'\n' ;
		return *this ;
	}
	virtual ~RCObj() {
		cout<<"RCObj destructor."<<'\n' ;
	}
	
private:
	int _refCount ;
	bool _shareable ;
};

class String
{
public:
	String (const char *val="") ;
	void print() {
		cout<<this->_value->data<<" cnt: "<<this->_value->cnt()<<'\n' ;
	}
    class CharProxy {
    public:
        CharProxy(String& str, int index):_theString(str),_charIndex(index) {

        }
        ~CharProxy() {
        	cout<<"CharProxy destructor"<<'\n' ;
        }
        CharProxy& operator=(const CharProxy& rhs) {
        	cout<<"--judge lhs--"<<'\n' ;
        	if (this->_theString._value->isShareable()) {
        		//触发写时复制
        		this->_theString._value = new StringValue(this->_theString._value->data) ;
        	}
        	this->_theString._value->makeUnshareable() ;
        	this->_theString._value->data[this->_charIndex] = rhs._theString._value->data[rhs._charIndex] ;
        	return *this ;
        }
        CharProxy& operator=(char c) {
        	cout<<"--judge lhs--"<<'\n' ;
        	if (this->_theString._value->isShareable()) {
        		//触发写时复制
        		this->_theString._value = new StringValue(this->_theString._value->data) ;
        	}
        	this->_theString._value->makeUnshareable() ;
        	this->_theString._value->data[this->_charIndex] = c ;
        	return *this ;
        }
        operator char() const {
        	cout<<"--judge rhs--"<<'\n' ;
        	return this->_theString._value->data[this->_charIndex] ;
        }

    private:
        String& _theString;
        int _charIndex;
    };
	friend class CharProxy;
	/*
	//原本的设计，无法区分出左值与右值
	char &operator[](int index) ;
	const char &operator[](int index) const ;
	*/
	CharProxy operator[](int index) ;
	const CharProxy operator[](int index) const ;
	
private:
	class StringValue: public RCObj {
	public:
		char *data ;
		StringValue(const char *val);
		StringValue(const StringValue &rhs);
		void init(const char *iniVal);
		~StringValue();
	};
	RCPtr<StringValue> _value ;
};

String::StringValue::StringValue(const char *val) {
	this->init(val) ;
	cout<<"StringValue constructor."<<'\n' ;
}
String::StringValue::StringValue(const String::StringValue &rhs) {
	this->init(rhs.data) ;
	cout<<"StringValue copy constructor."<<'\n' ;
}
void String::StringValue::init(const char *iniVal) {
	data = new char[strlen(iniVal)+1] ;
	strcpy(data, iniVal) ;
	printf("StringValue constructor %p.\n", data);
}
String::StringValue::~StringValue() {
	delete data ;
	cout<<"StringValue destructor."<<'\n' ;
}
String::String (const char *val):_value(new StringValue(val)) {
	cout<<"String constructor."<<'\n' ;
}

/*
//原本的设计，无法区分出左值与右值

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
*/
const String::CharProxy String::operator[](int index) const{
    return CharProxy(const_cast<String&>(*this), index);
}
String::CharProxy String::operator[](int index){
    return CharProxy(*this, index);
}

signed main()
{
	String str1 = "abcdefghijklmn" ;
	cout<<'\n' ;
	/*String str2 = str1 ;
	str2.print() ;
	cout<<'\n' ;
	str1[2] = '5' ;
	cout<<'\n' ;
	str1.print() ;
	str2.print() ;
	cout<<'\n' ;*/
	cout<<str1[2]<<'\n' ;
	cout<<'\n' ;
	String str2 = str1 ;
	str2.print() ;
	cout<<'\n' ;
	str1[1] = 'q' ;
	cout<<'\n' ;
	str1[2] = '5' ;
	cout<<'\n' ; 
	char *ch = &str1[2];
}