// thread::get_id / this_thread::get_id
#include <iostream>       // std::cout
#include <thread>         // std::thread, std::thread::id, std::this_thread::get_id
#include <chrono>         // std::chrono::seconds
 
std::thread::id main_thread_id = std::this_thread::get_id();

void is_main_thread(int rank) {
  if ( main_thread_id == std::this_thread::get_id() )
    std::cout << "This is the main thread."<< std::this_thread::get_id() <<"\n";
  else
    std::cout << "This is not the main thread."<< std::this_thread::get_id() <<"\n";
}

int main() 
{
  std::thread th_0 (is_main_thread, 0);
  std::thread th_1 (is_main_thread, 1);
  std::cout << "th.get_id()" << th_0.get_id() << "\n";
  std::cout << "th.get_id()" << th_1.get_id() << "\n";
  th_0.join();
  th_1.join();
}
