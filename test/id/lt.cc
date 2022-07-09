#include <thread.h>

#include <cassert>

int main(int, char **) {
    minithread::Thread::id id0;
    minithread::Thread::id id1;
    minithread::Thread::id id2;
    minithread::Thread t([]() {});
    id2 = t.get_id();
    t.join();
    assert(!(id0 < id1));
    assert((id0 <= id1));
    assert(!(id0 > id1));
    assert((id0 >= id1));
    assert(!(id0 == id2));
    if (id0 < id2) {
        assert((id0 <= id2));
        assert(!(id0 > id2));
        assert(!(id0 >= id2));
    } else {
        assert(!(id0 <= id2));
        assert((id0 > id2));
        assert((id0 >= id2));
    }

    return 0;
}
