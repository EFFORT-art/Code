#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

int main() {
    const int total = 100;
    char bar[102] = {0}; // 初始化为0
    const char* label = "|/-\\";
    for (int i = 0; i <= total; ++i) {
        // 在控制台输出：进度条字符串、百分比、旋转字符
        cout << "\r[" << bar << "][" << i << "%][" << label[i % 4] << "]";
        cout.flush();
        if (i < total) {
            bar[i] = '=';
            bar[i+1] = '>';
        } else {
            bar[i] = '=';
            // 最后i==total, 不需要箭头
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    cout << endl;
    return 0;
}
