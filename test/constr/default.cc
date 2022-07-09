#include <thread.h>

#include <cassert>

int main(int, char **) {
    minithread::Thread t;
    assert(t.get_id() == minithread::Thread::id());

    return 0;
}
