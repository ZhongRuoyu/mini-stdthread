#include <thread.h>

#include <cassert>
#include <iostream>

int main(int, char **) {
    minithread::Thread::id id1;
    minithread::Thread::id id2;
    minithread::Thread t([]() {});
    id2 = t.get_id();
    t.join();
    typedef std::hash<minithread::Thread::id> H;
    static_assert(noexcept(H()(id2)), "Operation must be noexcept");
    H h;
    assert(h(id1) != h(id2));

    return 0;
}
