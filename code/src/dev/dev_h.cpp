#include <iostream>

#include <boost/thread.hpp>

using namespace std;

void TestFunction() {
    for (int i = 0; i < 10; i++) {
        cout << i << " in test." << endl;
        boost::this_thread::yield();
    }
}

int main() {
    boost::thread t(&TestFunction);

    for (int i = 0; i < 10; i++) {
        cout << i << " in main." << endl;
    }

    return 0;
}