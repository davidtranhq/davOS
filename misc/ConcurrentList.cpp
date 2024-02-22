#include <iostream>
#include <memory>

template <typename T>
class ConcurrentList {
public:
    class Iterator {
        using Node = typename ConcurrentList<T>::Node;
    public: 
        Iterator(Node *ptr) : node {ptr} {}

        auto operator->() { return node; }
    private:
        Node *node;
    };

    template <typename V>
    auto insert(Iterator it, V &&val) -> void {
        if (it == end()) {
            emplace_back();
            return;
        }

        if (it->isHead()) {
            emplace_front(val);
        } else {
            // insert between it->prev and it->next
            auto newNode = std::make_unique<Node>(val, std::move(it->prev->next), it->prev);
            newNode->next->prev = newNode.get();
            newNode->prev->next = std::move(newNode);
        }
    }

    auto erase(Iterator it) -> void {
        if (it->isTail()) {
            pop_back();
        } else if (it->isHead()) {
            pop_front();
        } else {
            it->next->prev = it->prev;
            it->prev->next = std::move(it->next);
        }
    }


    template <typename V>
    auto emplace_back(V &&val) -> void {
        if (empty()) {
            insert_first(val);
        } else {
            tail->next = std::make_unique<Node>(val, nullptr, tail);
            tail = tail->next.get();
        }
    }

    template <typename V>
    auto emplace_front(V &&val) -> void {
        if (empty()) {
            insert_first(val);
        } else {
            head = std::make_unique<Node>(val, std::move(head), nullptr);
            head->next->prev = head.get();
        }
    }

    auto pop_back() {
        if (tail->prev) {
            tail = tail->prev;
            tail->next = nullptr;
        } else {
            head = nullptr;
            tail = nullptr;
        }
    }
    
    auto pop_front() {
        if (head->next) {
            head = std::move(head->next);
        } else {
            head = nullptr;
            tail = nullptr;
        }
    }

    auto front() -> T & {
        return head->value;
    }

    auto back() -> T & {
        return tail->value;
    }

    auto begin() {
        return Iterator {head.get()};
    }

    auto end() {
        return Iterator {nullptr};
    }

    constexpr auto empty() const noexcept { return !head; }

    auto print() const noexcept {
        auto curr = head;
        while (!curr) {
            std::cout << curr->value << ' ';
            curr = curr->next;
        }
    }

private:
    template <typename V>
    auto insert_first(V &&val) -> void {
        head = std::make_unique<Node>(val, nullptr, nullptr);
        tail = head.get();
    }

    struct Node {
        T value;
        std::unique_ptr<Node> next {nullptr};
        Node *prev {nullptr};
    };

    std::unique_ptr<Node> head {nullptr};
    Node *tail {nullptr};
};

auto print_list(const auto &list) {
    std::cout << "List: ";
    list.print();
    std::cout << '\n';
}

int main() {
    auto list = ConcurrentList<int> {};
    print_list(list);

    list.emplace_back(1);
    list.emplace_back(2);
    list.emplace_front(3);

    print_list(list);


}