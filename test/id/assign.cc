#include <thread.h>

#include <cassert>

int main(int, char **) {
    minithread::Thread::id id0;
    minithread::Thread::id id1;
    id1 = id0;
    assert(id1 == id0);
    minithread::Thread t([]() {});
    id1 = t.get_id();
    t.join();
    assert(id1 != id0);

    return 0;
}
