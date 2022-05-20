#include <iostream>
#include "avl_tree.h"

using namespace std;

int main() {
    Set<int> s;
    s.insert(2);
    s.insert(2);
    s.insert(2);
    cout << (int)s.size() << "\n";
}
