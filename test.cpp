#include <iostream>
#include <vector>
#include "unique_ptr.h"

using namespace mem;


void foo(const UniquePtr<int> &ptr) {
    std::cout << "foo(" << *ptr << ")" << std::endl;
}

int main() {

    // UniquePtr<int> p1(new int(10));
    // UniquePtr<int> p2(new int(23));

    // RvalueUniquePtr<int> rp1(p1.move());

    // p2 = {rp1};


    // foo(p2);

    std::vector<UniquePtr<int>> data;
    UniquePtr<int> p1(new int(12));

    // data.push_back(p1); // ce | std::vector<UniquePtr<int>>
    // data.push_back(p1.move()); // ce | std::vector<UniquePtr<int>>
    // data.push_back(p1); // ce | std::vector<RvalueUniquePtr<int>>
    // data.push_back(p1.move()); // ok! | std::vector<RvalueUniquePtr<int>>


    /*

    */
    // data.push_back(p1); // ce | std::vector<UniquePtr<int>>
    // data.push_back(p1.move()); // ce | std::vector<UniquePtr<int>>

    Move(p1);

    return 0;
}