#include "task.h"
#include <iostream>

using namespace coro;
using namespace std;

Task<> second() {
    std::string *p = new std::string("bla bla");
    cout << "There ";
    co_await coroutine::suspend_always{};
    cout << *p << " ";
    cout << "General ";
    delete p;
}

Task<> first() {
    cout << "Hello ";
    auto x = second();
    while (not x.isDone()) {
      co_await x;
    }
    cout << "Kenobi\n";
}

int main() {
    auto task = first();
    while (task.resume()) {}
}