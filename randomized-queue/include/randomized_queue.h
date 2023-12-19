#pragma once

#include <algorithm>
#include <random>
#include <utility>
#include <vector>

template <class T>
class randomized_queue
{
    struct random_struct
    {
        random_struct()
            : m_rand_engine(std::random_device{}())
        {
        }
        int get_rand(std::size_t from, std::size_t to) const
        {
            std::uniform_int_distribution distribution(from, to);
            return distribution(m_rand_engine);
        }

        mutable std::mt19937 m_rand_engine;
    };

    template <bool is_const>
    class Iterator
    {
        using queue_type = std::conditional_t<is_const,
                                              const randomized_queue,
                                              randomized_queue>;

        friend class randomized_queue;

        queue_type * m_queue = nullptr;
        std::size_t m_current;
        std::vector<std::size_t> m_permutation;

        void generate()
        {
            std::iota(m_permutation.begin(), m_permutation.end(), 0);
            std::shuffle(m_permutation.begin(), m_permutation.end(), m_queue->random_generator.m_rand_engine);
            m_permutation.push_back(m_permutation.size());
        }
        Iterator(queue_type & queue,
                 std::size_t current)
            : m_queue(&queue)
            , m_current(current)
            , m_permutation(queue.m_data.size())
        {
            generate();
        }
        Iterator(queue_type & queue)
            : m_queue(&queue)
            , m_current(queue.m_data.size())
            , m_permutation(queue.m_data.size())
        {
            generate();
        }

    public:
        using value_type = std::conditional_t<is_const, const T, T>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using pointer = value_type *;
        using reference = value_type &;

        Iterator() = default;
        reference operator*() const
        {
            return m_queue->m_data[m_permutation[m_current]];
        }

        pointer operator->() const
        {
            return &(m_queue->m_data)[m_permutation[m_current]];
        }

        Iterator & operator++()
        {
            m_current++;
            return *this;
        }

        Iterator operator++(int)
        {
            auto tmp = *this;
            operator++();
            return tmp;
        }

        friend bool operator==(const Iterator & first, const Iterator & second)
        {
            return first.m_queue == second.m_queue && first.m_permutation[first.m_current] == second.m_permutation[second.m_current];
        }

        friend bool operator!=(const Iterator & first, const Iterator & second)
        {
            return !(first == second);
        }
    };

    std::vector<T> m_data;
    random_struct random_generator;

public:
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    randomized_queue() = default;

    iterator begin() { return iterator(*this, 0); };
    iterator end() { return iterator(*this); };
    const_iterator begin() const { return const_iterator(*this, 0); };
    const_iterator end() const { return const_iterator(*this); };
    const_iterator cbegin() const { return begin(); };
    const_iterator cend() const { return end(); };

    bool empty() const
    {
        return size() == 0;
    }

    std::size_t size() const
    {
        return m_data.size();
    }

    template <typename S>
    void enqueue(S && item)
    {
        m_data.push_back(std::forward<S>(item));
    }

    T const & sample() const
    {
        return m_data[random_generator.get_rand(0, size() - 1)];
    }

    T dequeue()
    {
        std::swap(m_data[random_generator.get_rand(0, size() - 1)], m_data.back());
        auto tmp = std::move(m_data.back());
        m_data.pop_back();
        return tmp;
    };
};