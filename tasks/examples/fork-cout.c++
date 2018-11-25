#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    cout << "A";
    if (fork()) cout << "AA";
}
