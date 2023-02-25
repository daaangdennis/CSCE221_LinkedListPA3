#pragma once

#include <cstddef> // size_t
#include <iterator> // std::bidirectional_iterator_tag
#include <type_traits> // std::is_same, std::enable_if

template <class T>
class List {
    private:
    struct Node {
        Node *next, *prev;
        T data;
        explicit Node(Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev} {}
        explicit Node(const T& data, Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev}, data{data} {}
        explicit Node(T&& data, Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev}, data{std::move(data)} {}
    };

    template <typename pointer_type, typename reference_type>
    class basic_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = pointer_type;
        using reference         = reference_type;
    private:
        friend class List<value_type>;
        using Node = typename List<value_type>::Node;

        Node* node;

        explicit basic_iterator(Node* ptr) noexcept : node{ptr} {}
        explicit basic_iterator(const Node* ptr) noexcept : node{const_cast<Node*>(ptr)} {}

    public:
        basic_iterator() {node = nullptr;};
        basic_iterator(const basic_iterator&) = default;
        basic_iterator(basic_iterator&&) = default;
        ~basic_iterator() = default;
        basic_iterator& operator=(const basic_iterator&) = default;
        basic_iterator& operator=(basic_iterator&&) = default;

        reference operator*() const {
            return this->node->data;
        }
        pointer operator->() const {
            T* temp = &(this->node->data);
            return temp;
        }

        // Prefix Increment: ++a
        basic_iterator& operator++() {
            this->node = this->node->next;
            return *this;
        }
        // Postfix Increment: a++
        basic_iterator operator++(int) {
            Node* temp = this->node;
            this->node = this->node->next;
            return basic_iterator(temp);
        }
        // Prefix Decrement: --a
        basic_iterator& operator--() {
            this->node = this->node->prev;
            return *this;
        }
        // Postfix Decrement: a--
        basic_iterator operator--(int) {
            Node* temp = this->node;
            this->node = this->node->prev;
            return basic_iterator(temp);
        }

        bool operator==(const basic_iterator& other) const noexcept {
            return this->node == other.node;
        }
        bool operator!=(const basic_iterator& other) const noexcept {
            return this->node != other.node;
        }
    };

public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using iterator        = basic_iterator<pointer, reference>;
    using const_iterator  = basic_iterator<const_pointer, const_reference>;

private:
    Node head, tail;
    size_type _size;

public:
    List(): head(0), tail(0), _size(0){
        head.next = &tail;
        head.prev = &tail;
        tail.next = &head;
        tail.prev = &head;
    }
    List( size_type count, const T& value ): head(0), tail(0), _size(count) {
        Node *prevNode, *currentNode;

        head.next = &tail;
        head.prev = &tail;
        tail.next = &head;
        tail.prev = &head;

        if(count > 0)
        {
            prevNode = &head;

            for(size_t num = 0; num < count; num++)
            {
                currentNode = new Node(value);
                currentNode->prev = prevNode;
                prevNode->next = currentNode;
                prevNode = currentNode;
            }
            currentNode->next = &tail;
            tail.prev = currentNode;
        }
    }
    explicit List( size_type count ): head(0), tail(0), _size(count){
        Node *prevNode, *currentNode;

        head.next = &tail;
        head.prev = &tail;
        tail.next = &head;
        tail.prev = &head;

        if(count > 0)
        {
            prevNode = &head;

            for(size_t num = 0; num < count; num++)
            {
                currentNode = new Node(T{});
                currentNode->prev = prevNode;
                prevNode->next = currentNode;
                prevNode = currentNode;
            }
            currentNode->next = &tail;
            tail.prev = currentNode;
        }

    }
    List( const List& other ): head(0), tail(0), _size(other._size) {
        Node *prevNode, *currentNode;

        head.next = &tail;
        head.prev = &tail;
        tail.next = &head;
        tail.prev = &head;

        iterator currentSpot = iterator(other.head.next);
        prevNode = &head;
        while(currentSpot != other.end())
        {
            currentNode = new Node(*currentSpot);
            currentNode->prev = prevNode;
            prevNode->next = currentNode;
            prevNode = currentNode;
            currentSpot++;
        }
        currentNode->next = &tail;
        tail.prev = currentNode;
    }
    List( List&& other ): head(0), tail(0), _size(other._size) {
        if(_size > 0)
        {
            head.next = other.head.next;
            head.prev = &tail;
            tail.next = &head;
            tail.prev = other.tail.prev;

            head.next->prev = &head;
            tail.prev->next = &tail;
        }
        else
        {
            head.next = &tail;
            head.prev = &tail;
            tail.next = &head;
            tail.prev = &head;
        }
        
        //Set old linked list to empty state
        other.head.next = other.head.prev = &(other.tail);
        other.tail.next = other.tail.prev = &(other.head);
        other._size = 0;

    }
    ~List() {
        Node *prevNode, *currentNode = head.next;
        while(currentNode != &tail)
        {
            prevNode = currentNode;
            currentNode = currentNode->next;
            delete prevNode;
        }
        head.next = &tail;
        tail.prev = &head;

    }
    List& operator=( const List& other ) {
        Node *prevNode, *currentNode;
        if(this != &other)
        {
            this->~List();
            this->_size = other._size;
            
            iterator currentSpot = iterator(other.head.next);
            prevNode = &(this->head);
            while(currentSpot != other.end())
            {
                currentNode = new Node(*currentSpot);
                currentNode->prev = prevNode;
                prevNode->next = currentNode;
                prevNode = currentNode;
                currentSpot++;
            }
            currentNode->next = &(this->tail);
            this->tail.prev = currentNode;
        }
        return *this;
    }
    List& operator=( List&& other ) noexcept {
        if(this != &other)
        {
            this->~List();
            this->_size = other._size;
            if(_size > 0)
            {
                this->head.next = other.head.next;
                this->head.prev = &(this->tail);
                this->tail.next = &(this->head);
                this->tail.prev = other.tail.prev;

                this->head.next->prev = &(this->head);
                this->tail.prev->next = &(this->tail);
            }
            else
            {
                this->head.next = &(this->tail);
                this->head.prev = &(this->tail);
                this->tail.next = &(this->head);
                this->tail.prev = &(this->head);
            }
            
            //Set old linked list to empty state
            other.head.next = other.head.prev = &(other.tail);
            other.tail.next = other.tail.prev = &(other.head);
            other._size = 0;
        }
        return *this;
    }

    reference front() {
        // TODO
    }
    const_reference front() const {
        // TODO
    }
	
    reference back() {
        // TODO
    }
    const_reference back() const {
        // TODO
    }
	
    iterator begin() noexcept {
        if(_size == 0)
        {
            return iterator(&tail);
        }
        return iterator(head.next);
    }
    const_iterator begin() const noexcept {
        if(_size == 0)
        {
            return const_iterator(&tail);
        }
        return const_iterator(head.next);
    }
    const_iterator cbegin() const noexcept {
        if(_size == 0)
        {
            return const_iterator(&tail);
        }
        return const_iterator(head.next);
    }

    iterator end() noexcept {
        return iterator(&tail);
    }
    const_iterator end() const noexcept {
        return const_iterator(&tail);
    }
    const_iterator cend() const noexcept {
        return const_iterator(&tail);
    }

    bool empty() const noexcept {
        // TODO
    }

    size_type size() const noexcept {
        return _size;
    }

    void clear() noexcept {
        // TODO
    }

    iterator insert( const_iterator pos, const T& value ) {
        // TODO
    }
    iterator insert( const_iterator pos, T&& value ) {
        // TODO
    }

    iterator erase( const_iterator pos ) {
        // TODO
    }

    void push_back( const T& value ) {
        // TODO
    }
    void push_back( T&& value ) {
        // TODO
    }

    void pop_back() {
        // TODO
    }
	
    void push_front( const T& value ) {
        // TODO
    }
	void push_front( T&& value ) {
        // TODO
    }

    void pop_front() {
        // TODO
    }

    /*
      You do not need to modify these methods!
      
      These method provide the non-const complement 
      for the const_iterator methods provided above.
    */
    iterator insert( iterator pos, const T & value) { 
        return insert((const_iterator &) (pos), value);
    }

    iterator insert( iterator pos, T && value ) {
        return insert((const_iterator &) (pos), std::move(value));
    }

    iterator erase( iterator pos ) {
        return erase((const_iterator&)(pos));
    }
};


/*
    You do not need to modify these methods!

    These method provide a overload to compare const and 
    non-const iterators safely.
*/
 
namespace {
    template<typename Iter, typename ConstIter, typename T>
    using enable_for_list_iters = typename std::enable_if<
        std::is_same<
            typename List<typename std::iterator_traits<Iter>::value_type>::iterator, 
            Iter
        >{} && std::is_same<
            typename List<typename std::iterator_traits<Iter>::value_type>::const_iterator,
            ConstIter
        >{}, T>::type;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator==(const Iterator & lhs, const ConstIter & rhs) {
    return (const ConstIter &)(lhs) == rhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator==(const ConstIter & lhs, const Iterator & rhs) {
    return (const ConstIter &)(rhs) == lhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator!=(const Iterator & lhs, const ConstIter & rhs) {
    return (const ConstIter &)(lhs) != rhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator!=(const ConstIter & lhs, const Iterator & rhs) {
    return (const ConstIter &)(rhs) != lhs;
}