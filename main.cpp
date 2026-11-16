#include <memory>
#include <stdexcept>
#include <map>
#include <vector>
#include <iostream>
#include "CustomMap.hpp"
#include "CustomAllocator.hpp"

int main() {
    
    auto factorial2 = [](int n){if(n<=1) {return 1;} int result = n; for (int i = (n-1); i > 1; i--) {result *= i;} return result;};

    using MapPoolAlloc = AwesomeAllocator<std::pair<const int, int>, 10>;

    // task 1
    std::map<int,int> ExampleMap;

    // task 2
    for (int i = 0; i < 10; ++i)
        ExampleMap[i] = factorial2(i);

    // task 3
    std::map<int, int, std::less<int>, MapPoolAlloc> CustomAllocMap;

    // task 4
    for (int i = 0; i < 10; ++i)
        CustomAllocMap[i] = factorial2(i);

    // task 5
    std::cout << "---=== map with custom allocator ===---" << std::endl;
    for (const auto& [key,value] : CustomAllocMap)
       std::cout << key << " " << value << std::endl;
    
    // task 6
    ForwardList<int> ExampleContainer;

    // task 7
    for (int i = 0; i < 10; i++)
        ExampleContainer.push_back(i);
    
    // task 8
    using ListPoolAlloc = AwesomeAllocator<int, 10>;
    ForwardList<int, ListPoolAlloc> CustomAllocCustomCont;

    // task 9
    for (int i = 0; i < 10; i++)
        CustomAllocCustomCont.push_back(i);

    // task 10
    std::cout << "---=== custom cont with custom alloc ===---" << std::endl;
    for (auto i = CustomAllocCustomCont.begin(); i != CustomAllocCustomCont.end(); i++)
        std::cout << *i << std::endl;

    return 0;
}