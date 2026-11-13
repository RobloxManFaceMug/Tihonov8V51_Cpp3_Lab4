#include <cstddef>
#include <memory>
#include <new>
#include <stdexcept>
#include <map>
#include <vector>
#include <iostream>
#include <limits>
#include "lib.hpp"

template <typename T, size_t Block_size = 10>
class AwesomeAllocator {
private:
    struct Pool {
        struct freenode 
        {
            freenode *next;
        };

        std::size_t total_bytes_allocated = 0;
        std::size_t total_bytes_deallocated = 0;

        freenode *free_list = nullptr; 
        std::vector<void *> allocatedBlock;

        Pool() = default;

        ~Pool() 
        {   
            std::cout << "Allocated memory: " << total_bytes_allocated << " bytes" << std::endl;
            std::cout << "Deallocated memory: " << total_bytes_deallocated << " bytes" << std::endl;

            for (void *block : allocatedBlock) {
                ::operator delete(block);
            }
        }

        void *allocate(size_t n) {
            if (n == 0) {
                return nullptr;
            }
            
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
                throw std::bad_alloc(); 
            }

            if (n != 1) 
            {
                void *p = ::operator new(n * sizeof(T));
                allocatedBlock.push_back(p);
                total_bytes_allocated += (n * sizeof(T));
                return p;
            }

            if (free_list == nullptr) 
            {
                size_t allocatedSize = Block_size * sizeof(T);
                void *new_block = ::operator new(allocatedSize);
                allocatedBlock.push_back(new_block);

                total_bytes_allocated += allocatedSize;

                char *ptr = static_cast<char *>(new_block);
                for (size_t i = 0; i < Block_size; ++i)
                {
                    freenode *node = reinterpret_cast<freenode *>(ptr + i * sizeof(T));
                    node->next = free_list;
                    free_list = node;
                }
            }
            freenode *node = free_list;
            free_list = free_list->next;
            return node;
        }

        void deallocate(void *p, size_t n) {
            if (n != 1) 
            {
                total_bytes_deallocated += n * sizeof(T);

                ::operator delete(p);
                return;
            }
            total_bytes_deallocated += sizeof(T);

            freenode *node = static_cast<freenode *>(p);
            node->next = free_list;
            free_list = node;
        }
    };

    std::shared_ptr<Pool> pool_;

public:
    using value_type = T;

    template <typename U, size_t B>
    friend class AwesomeAllocator;

    template <typename U>
    struct rebind 
    {
        using other = AwesomeAllocator<U, Block_size>;
    };

    AwesomeAllocator() : pool_(std::make_shared<Pool>()) {} 

    template <typename U>
    AwesomeAllocator(const AwesomeAllocator<U, Block_size> &other) noexcept : pool_(other.pool_) {}

    T *allocate(std::size_t n) 
    {
        return static_cast<T *>(pool_->allocate(n));
    }

    void deallocate(T *p, std::size_t n) 
    {
        pool_->deallocate(p, n);
    }

    template <typename U>
    bool operator==(const AwesomeAllocator<U, Block_size> &other) const noexcept {
        return pool_ == other.pool_;
    }

    template <typename U>
    bool operator!=(const AwesomeAllocator<U, Block_size> &other) const noexcept {
        return !(*this == other);
    }
};


int main() {
    using MapPoolAlloc = AwesomeAllocator<std::pair<const int, int>, 10>;

    // task 1
    std::map<int,int> ExampleMap;

    // task 2
    for (int i = 0; i < 10; ++i)
        ExampleMap[i] = factorial(i);

    // task 3
    std::map<int, int, std::less<int>, MapPoolAlloc> CustomAllocMap;

    // task 4
    for (int i = 0; i < 10; ++i)
        CustomAllocMap[i] = factorial(i);

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