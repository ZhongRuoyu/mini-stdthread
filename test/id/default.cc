#include <thread.h>

#include <cassert>

int main(int, char **) {
    minithread::Thread::id id;
    assert(id == minithread::Thread::id());

    return 0;
}
