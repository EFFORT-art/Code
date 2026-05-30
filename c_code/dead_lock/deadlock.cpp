#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;

mutex mtx1;
mutex mtx2;

int thread_handler1()
{
    lock_guard<mutex> guard1(mtx1);
    this_thread::sleep_for(chrono::milliseconds(500));
    lock_guard<mutex> guard2(mtx2);

    cout << "do thread_handler1 over!" << endl;
    return 0;
}

int thread_handler2()
{
    lock_guard<mutex> guard1(mtx2);
    this_thread::sleep_for(chrono::milliseconds(500));
    lock_guard<mutex> guard2(mtx1);

    cout << "do thread_handler1 over!" << endl;
    return 0;
}

void func()
{
	int g_data=0;
	  while (true) {
        g_data++;
    }
}

int thread_handler3()
{
    this_thread::sleep_for(chrono::milliseconds(500));
    func();
}

/*
g++ -g -o run
./run
*/
int main()
{
    thread t1(thread_handler1);
    thread t2(thread_handler2);
    thread t3(thread_handler3);

    t1.join();
    t2.join();
    t3.join();
    cout << "do main over!" << endl;
}
