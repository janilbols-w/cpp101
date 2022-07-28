#include <iostream>
using namespace std;

int main()
{
	for(int i; i<10; i++){
		char A[128] = "thisIsHashChar";
		string s = A;
		std::hash<string> hash_val;
		uint64_t hash_uint64_val = hash_val(s);
		cout << A << " with hash_val = " << hash_val(s) << endl;
		cout << A << " with hash_uint64_val = " << hash_uint64_val << endl;
		uint64_t hash_uint64_val_shifted = hash_uint64_val << 2;
		cout << A << " with hash_uint64_val_shifted = " << hash_uint64_val_shifted << endl;
	}
	return 0;
}

