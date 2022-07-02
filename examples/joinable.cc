#include <chrono>
#include <iostream>
#include <thread>

#include <thread.h>

void foo() { std::this_thread::sleep_for(std::chrono::seconds(1)); }

int main() {
    minithread::Thread t;
    std::cout << "before starting, joinable: " << std::boolalpha << t.joinable()
              << '\n';

    t = minithread::Thread(foo);
    std::cout << "after starting, joinable: " << t.joinable() << '\n';

    t.join();
    std::cout << "after joining, joinable: " << t.joinable() << '\n';
}
