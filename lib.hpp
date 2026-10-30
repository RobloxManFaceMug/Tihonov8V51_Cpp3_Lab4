#include <cstddef>
#include <utility>
#include <memory>

int factorial(int n);



template <typename T, typename Alloc = std::allocator<T>>
class ForwardList {
    private:
        struct Node
        {
            T data;
            Node *next;
            Node(const T &val, Node *nxt) : data(val), next(nxt) {}
        };

        using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        using NodeAllocTraits = std::allocator_traits<NodeAlloc>;

        Node *m_head;
        size_t m_size;
        NodeAlloc m_alloc;

        Node *create_node(const T &value, Node *next)
        {
            Node *p = NodeAllocTraits::allocate(m_alloc, 1);
            NodeAllocTraits::construct(m_alloc, p, value, next);
            return p;
        }

        void destroy_node(Node *p)
        {
            NodeAllocTraits::destroy(m_alloc, p);
            NodeAllocTraits::deallocate(m_alloc, p, 1);
        }

    public:
        struct iterator
        {
            Node *ptr;

            iterator &operator++()
            {
                ptr = ptr->next;
                return *this;
            }

            iterator operator++(int)
            {
                iterator t = *this;
                ptr = ptr->next;
                return t;
            }

            T &operator*() const { return ptr->data; }
            T *operator->() const { return &ptr->data; }
            bool operator==(const iterator &o) const { return ptr == o.ptr; }
            bool operator!=(const iterator &o) const { return ptr != o.ptr; }
        };

        struct const_iterator
        {
            const Node *ptr;

            const_iterator &operator++()
            {
                ptr = ptr->next;
                return *this;
            }

            const_iterator operator++(int)
            {
                const_iterator t = *this;
                ptr = ptr->next;
                return t;
            }

            const T &operator*() const { return ptr->data; }
            const T *operator->() const { return &ptr->data; }
            bool operator==(const const_iterator &o) const { return ptr == o.ptr; }
            bool operator!=(const const_iterator &o) const { return ptr != o.ptr; }
        };

        ForwardList() : m_head(nullptr), m_size(0), m_alloc() {}
        ~ForwardList() { clear(); }

        ForwardList(const ForwardList &other) : ForwardList()
        {
            for (Node *cur = other.m_head; cur; cur = cur->next)
            {
                push_back(cur->data);
            }
        }

        ForwardList(ForwardList &&other) noexcept: m_head(other.m_head), m_size(other.m_size), m_alloc(std::move(other.m_alloc))
        {
            other.m_head = nullptr;
            other.m_size = 0;
        }

        ForwardList &operator=(const ForwardList &other)
        {
            if (this != &other)
            {
                ForwardList tmp(other);
                std::swap(m_head, tmp.m_head);
                std::swap(m_size, tmp.m_size);
                std::swap(m_alloc, tmp.m_alloc);
            }
            return *this;
        }

        ForwardList &operator=(ForwardList &&other) noexcept
        {
            if (this != &other)
            {
                clear();
                m_head = other.m_head;
                m_size = other.m_size;
                m_alloc = std::move(other.m_alloc);
                other.m_head = nullptr;
                other.m_size = 0;
            }
            return *this;
        }

        void push_back(const T &value)
        {
            Node *node = create_node(value, nullptr);
            if (!m_head)
            {
                m_head = node;
            }
            else
            {
                Node *cur = m_head;
                while (cur->next)
                    cur = cur->next;
                cur->next = node;
            }
            ++m_size;
        }

        size_t size() const { return m_size; }
        bool empty() const { return m_size == 0; }

        void clear()
        {
            while (m_head)
            {
                Node *next = m_head->next;
                destroy_node(m_head);
                m_head = next;
            }
            m_size = 0;
        }

        iterator begin() { return iterator{m_head}; }
        iterator end() { return iterator{nullptr}; }
        const_iterator begin() const { return const_iterator{m_head}; }
        const_iterator end() const { return const_iterator{nullptr}; }
};