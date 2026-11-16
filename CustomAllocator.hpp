#include <cstddef>
#include <memory>
#include <new>
#include <stdexcept>
#include <map>
#include <vector>
#include <iostream>
#include <limits>

template <typename T, size_t Block_size = 10>
class AwesomeAllocator {
private:
    struct Pool {
        struct freenode 
        {
            freenode *next;
        };

        std::size_t m_total_bytes_allocated = 0;
        std::size_t m_total_bytes_deallocated = 0;

        freenode *free_list = nullptr; 
        std::vector<void *> allocatedBlock;

        Pool() = default;

        ~Pool() 
        {   
            std::cout << "Allocated memory: " << m_total_bytes_allocated << " bytes" << std::endl;
            std::cout << "Deallocated memory: " << m_total_bytes_deallocated << " bytes" << std::endl;

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
                m_total_bytes_allocated += (n * sizeof(T));
                return p;
            }

            if (free_list == nullptr) 
            {
                const size_t allocatedSize = Block_size * sizeof(T);
                void *new_block = ::operator new(allocatedSize);
                allocatedBlock.push_back(new_block);

                m_total_bytes_allocated += allocatedSize;

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
                m_total_bytes_deallocated += n * sizeof(T);

                ::operator delete(p);
                return;
            }
            m_total_bytes_deallocated += sizeof(T);

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