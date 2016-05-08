#include <iostream>

class parent{
	protected:
		int _val;
	public:
		parent(int val);
		virtual int getVal(void);
};

parent::parent(int val){
	this->_val = val;
}

int parent::getVal(void){
	return this->_val;
}


class child: public parent{
	public:
		child(int val);
		virtual int getVal(void);
};

child::child(int val): parent(val){
	
}

int child::getVal(void){
	return this->_val + 1000;
}

using namespace std;

int main(void){
	
	child instance(5);
	cout << instance.getVal() << endl;
	
	return 0;
}

