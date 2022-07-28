#include <iostream>
#include <set>
#include <map>
#include<list>
class obj{
public:
    obj(int* in_data){
        data_ptr = in_data;
        std::cout << "in_data( " << in_data << ", "<< *in_data << ")" << std::endl;
        std::cout << "data( " << data_ptr << ", "<< *data_ptr << ")" << std::endl;
    }
    void update(int* new_data){
        int** data_ptr_ptr = const_cast<int**>(&data_ptr);
        *data_ptr_ptr = new_data;
        std::cout<<"update - new_data("<< new_data << ", " << *new_data << ")" << std::endl;
        std::cout<<"update - data_ptr("<< data_ptr << ", " << *data_ptr << ")" << std::endl;
    }
private:
    const int* data_ptr;
};

class obj_B{
public:
    obj_B(int* in_data){
        data_ptr = in_data;
        std::cout << "in_data( " << in_data << ", "<< *in_data << ")" << std::endl;
        std::cout << "data( " << data_ptr << ", "<< *data_ptr << ")" << std::endl;
    }
    void update(int* new_data){
        int* data_ptr_ptr = const_cast<int*>(data_ptr);
        data_ptr_ptr = new_data;
        std::cout<<"update - new_data("<< new_data << ", " << *new_data << ")" << std::endl;
        std::cout<<"update - data_ptr("<< data_ptr << ", " << *data_ptr << ")" << std::endl;
    }
private:
    const int* data_ptr;
};

int main() {
    int d1 = 10;
    obj_B O{&d1};
    int d2 = 100;
    O.update(&d2);
    return 0;
}

