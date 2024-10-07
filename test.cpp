#include <iostream>
#include <limits>

bool fitsInInt(long value) {
	return value >= std::numeric_limits<int>::min() && value <= std::numeric_limits<int>::max();
}

int main() {
	long testValue1 = 0x7FFFFFFF; // max int value
	long testValue2 = 0x80000000; // exceeds 32-bit int range
	
	std::cout << "Test Value 1 fits in int " << fitsInInt(testValue1) << std::endl;	// should print 1 (true)
	std::cout << "Test Value 2 fits in int " << fitsInInt(testValue2) << std::endl;	// should print 0 (false)
}
