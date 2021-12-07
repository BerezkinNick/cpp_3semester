#include <iostream>
#include <vector>
#include <limits>
#include <random>

template <typename T> class SkipList {
    struct SkipNode {
        T value;
        double key;
        std::vector<SkipNode*> forward_nodes;
        std::vector<SkipNode*> between_nodes;

        SkipNode(T value, double key = 0,
                 std::vector<SkipNode*> forward_nodes = {}, std::vector<SkipNode*> between_nodes = {}):
                value(value), key(key),
                forward_nodes(forward_nodes), between_nodes(between_nodes) {};

        SkipNode(const SkipNode& sn) = default;

        SkipNode& operator=(const SkipNode& sn) = default;

        ~SkipNode() = default;
    };

public:
    struct BidirectionalIterator {
        using pointer = SkipNode*;
        using reference = SkipNode&;

        explicit
        BidirectionalIterator(pointer ptr_sn) : ptr_sn(ptr_sn) {}

        BidirectionalIterator& operator++() {
            ptr_sn = ptr_sn->forward_nodes[0];
            return *this;
        }

        BidirectionalIterator operator++(int) {
            BidirectionalIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        BidirectionalIterator& operator--() {
            ptr_sn = ptr_sn->between_nodes[0];
            return *this;
        }

        BidirectionalIterator operator--(int) {
            BidirectionalIterator tmp = *this;
            --(*this);
            return tmp;
        }
        reference operator*() const {
            return *ptr_sn;
        }
        pointer operator->() {
            return ptr_sn;
        }

        void find(BidirectionalIterator begin, BidirectionalIterator end, double key) {
            BidirectionalIterator tmp_it = begin;
            while (tmp_it != end && tmp_it->key != key) {
                tmp_it++;
            }
            *this = tmp_it;
        }

        void lower_bound(BidirectionalIterator begin, BidirectionalIterator end, double key) {
            BidirectionalIterator tmp_it = begin;
            while (tmp_it != end && tmp_it->key < key) {
                tmp_it++;
            }
            *this = tmp_it;
        }

        void upper_bound(BidirectionalIterator begin, BidirectionalIterator end, double key) {
            BidirectionalIterator tmp_it = begin;
            while (tmp_it != end && tmp_it->key <= key) {
                tmp_it++;
            }
            *this = tmp_it;
        }

        friend bool operator!=(const BidirectionalIterator& a, const BidirectionalIterator& b) {
            return a.ptr_sn != b.ptr_sn;
        }
    private:
        pointer ptr_sn;
    };

    BidirectionalIterator begin() {
        return BidirectionalIterator(head->forward_nodes[0]);
    }
    BidirectionalIterator end() {
        return BidirectionalIterator(null);
    }

    struct reverse_iterator {
        using pointer           = SkipNode*;
        using reference         = SkipNode&;

        explicit
        reverse_iterator(pointer ptr_sn) : ptr_sn(ptr_sn) {}


        reverse_iterator& operator++() {
            ptr_sn = ptr_sn->between_nodes[0];
            return *this;
        }

        reverse_iterator operator++(int) {
            reverse_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        reverse_iterator& operator--() {
            ptr_sn = ptr_sn->forward_nodes[0];
            return *this;
        }

        reverse_iterator operator--(int) {
            reverse_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        reference operator*() const {
            return *ptr_sn;
        }
        pointer operator->() {
            return ptr_sn;
        }

        friend bool operator!=(const reverse_iterator& a, const reverse_iterator& b) {
            return a.ptr_sn != b.ptr_sn;
        }
    private:
        pointer ptr_sn;
    };

    reverse_iterator rbegin() {
        return reverse_iterator(null->between_nodes[0]);
    }
    reverse_iterator rend() {
        return reverse_iterator(head);
    }

private:
    SkipNode* null;
    SkipNode* head;
    int size_list;
    int Max_level = 4;
    const double probability = 0.5;

    static bool check_(double left, double right) {
        return (left < right);
    }

    int random_level() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> p(0, 1);
        int level = 1;
        while (probability >= p(gen) && level < Max_level) {level++;}
        return level;
    }

public:

    SkipList() {
        T tmp_value = 0;
        double tmp_key;
        std::vector<SkipNode*> tmp_forward_nodes, tmp_between_nodes;

        tmp_key = std::numeric_limits<double>::max();
        for (int i = 0; i < Max_level; i++) {
            tmp_forward_nodes.push_back(nullptr);
            tmp_between_nodes.push_back(head);
        }
        SkipNode *p_null = new SkipNode(tmp_value, tmp_key, tmp_forward_nodes, tmp_between_nodes);
        null = p_null;

        tmp_forward_nodes.clear();
        tmp_between_nodes.clear();

        tmp_key = std::numeric_limits<double>::min();
        for (int i = 0; i < Max_level; i++) {
            tmp_forward_nodes.push_back(null);
            tmp_between_nodes.push_back(nullptr);
        }
        SkipNode *p_head = new SkipNode(tmp_value, tmp_key, tmp_forward_nodes, tmp_between_nodes);
        head = p_head;

        for (int i = 0; i < Max_level; i++) { null->between_nodes[i] = head; }

        size_list = 0;
    }

    //конструктор копирования
    SkipList(const SkipList &sl) : head(sl.head), null(sl.null), size_list(sl.size_list), Max_level(sl.Max_level) {
        std::vector<SkipNode*> tmp_vect(Max_level);
        head = new SkipNode(sl.head->value, sl.head->key, tmp_vect, tmp_vect);
        null = new SkipNode(sl.null->value, sl.null->key, tmp_vect, tmp_vect);

        for (int i = 0; i < Max_level; i++) {
            head->forward_nodes[i] = null;
            head->between_nodes[i] = nullptr;
            null->forward_nodes[i] = nullptr;
            null->between_nodes[i] = head;
        }

        SkipNode *tmp_ptr = sl.head->forward_nodes[0];
        while (tmp_ptr != sl.null) {
            this->insert(tmp_ptr->value, tmp_ptr->key, tmp_ptr->forward_nodes.size_list());
            tmp_ptr = tmp_ptr->forward_nodes[0];
        }
    }

    //конструктор перемещения
    SkipList(SkipList &&sl) : head(sl.head), null(sl.null), size_list(sl.size_list), Max_level(sl.Max_level) {
        sl.head->~SkipNode();
        sl.null->~SkipNode();
        sl.size_list = 0;
        sl.Max_level = 0;

        for (int i = 0; i < Max_level; i++) {null->between_nodes[i]->forward_nodes[i] = null;}
    }

    //функция вставки
    void insert(double value, double key, int level = 0) {
        SkipNode *tmp_ptr = head;

        if (level == 0) {level = random_level();}
        std::vector<SkipNode*> tmp_Nodes(level);
        SkipNode *ptr_sn = new SkipNode(value, key, tmp_Nodes, tmp_Nodes);
        for (int i = Max_level - 1; i >= 0; i--) {
            while (check_(tmp_ptr->forward_nodes[i]->key, key)) {
                tmp_ptr = tmp_ptr->forward_nodes[i];
            }
            if (i < level) {
                ptr_sn->forward_nodes[i] = tmp_ptr->forward_nodes[i];
                ptr_sn->between_nodes[i] = tmp_ptr;
                tmp_ptr->forward_nodes[i]->between_nodes[i] = ptr_sn;
                tmp_ptr->forward_nodes[i] = ptr_sn;
            }
        }
        size_list++;
    }
    ~SkipList() {
        this->clear();
        head->~SkipNode();
        null->~SkipNode();
    }

    //количество вхождений ключа
    int count(double key) {
        SkipNode *tmp_ptr = head;
        for (int i = Max_level - 1; i >= 0; i--) {
            while (tmp_ptr->forward_nodes[i]->key < key) {
                tmp_ptr = tmp_ptr->forward_nodes[i];
            }
        }
        int count = 0;

        while (tmp_ptr->forward_nodes[0]->key == key) {
            tmp_ptr = tmp_ptr->forward_nodes[0];
            count++;
        }
        return count;
    }

    //наличие элементов в списке
    bool empty() const{
        return (size_list == 0);
    }

    //размер списка
    int size() const{
        return size_list;
    }

    //удаление всех элементов списка
    void clear() {
        if (!this->empty()) {
            SkipNode *tmp_ptr = head;
            SkipNode *check_ptr = head->forward_nodes[0];
            while (check_ptr != null) {
                tmp_ptr = check_ptr;
                check_ptr = check_ptr->forward_nodes[0];
                tmp_ptr->~SkipNode();
            }
            for (int i = 0; i < Max_level; i++) {
                head->forward_nodes[i] = null;
                null->between_nodes[i] = head;
            }
            size_list = 0;
        }
    }

    //удаление из списка по итератору
    void erase(BidirectionalIterator& it) {
        SkipNode tmp_sn = *it;
        for (int i = 0; i < tmp_sn.forward_nodes.size(); i++) {
            tmp_sn.forward_nodes[i]->between_nodes[i] = tmp_sn.between_nodes[i];
            tmp_sn.between_nodes[i]->forward_nodes[i] = tmp_sn.forward_nodes[i];
        }
        it++;
        tmp_sn.~SkipNode();
        size_list--;
    }

    //оператор копирующего присваивания
    SkipList& operator=(const SkipList &sl) {
        if (&sl == this) {return *this;}

        SkipList *tmp_sl = new SkipList(sl);
        head = tmp_sl->head;
        null = tmp_sl->null;
        size_list = tmp_sl->size_list;
        Max_level = tmp_sl->Max_level;
        return *this;
    }

    //оператор перемещающего присваивания
    SkipList& operator=(SkipList &&sl) {
        if (&sl == this) {return *this;}

        head = sl.head;
        null = sl.null;
        for(int i = 0; i < Max_level; i++) {null->between_nodes[i]->forward_nodes[i] = null;}

        sl.clear();
        sl.head->~SkipNode();
        sl.null->~SkipNode();
        sl.size_list = 0;
        sl.Max_level = 0;
    }
};

int main() {
    SkipList<int> list1;
    for (int i = 0; i < 4; i++) {
        list1.insert(22, 1);
        list1.insert(23, 2);
        list1.insert(24, 3);
        list1.insert(25, 4);
    }

    SkipList<int>::BidirectionalIterator it = list1.begin();

    std::cout << "find test:" << std::endl;
    it.find(list1.begin(), list1.end(), 4);
    if (it->value == 25) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    it.find(list1.begin(), list1.end(), 1);
    if (it->value == 22) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    it.find(list1.begin(), list1.end(), 2);
    if (it->value == 23) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    it.find(list1.begin(), list1.end(), 3);
    if (it->value == 24) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    std::cout << std::endl;

    std::cout << "upper_bound test:" << std::endl;
    it.upper_bound(list1.begin(), list1.end(), 1);
    if (it->value == 23) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    it.upper_bound(list1.begin(), list1.end(), 2);
    if (it->value == 24) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    it.upper_bound(list1.begin(), list1.end(), 3);
    if (it->value == 25) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    std::cout << std::endl;

    std::cout << "lower_bound test:" << std::endl;
    it.lower_bound(list1.begin(), list1.end(), 1);
    if (it->value == 22) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    it.lower_bound(list1.begin(), list1.end(), 2);
    if (it->value == 23) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    it.lower_bound(list1.begin(), list1.end(), 3);
    if (it->value == 24) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    it.lower_bound(list1.begin(), list1.end(), 4);
    if (it->value == 25) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    std::cout << std::endl;

    std::cout << "count test:"<< std::endl;
    bool flag = 0;
    for (int i = 1; i <= 3; ++i){
        if (list1.count(i) != 4) flag = 1;
    }
    if (flag == 0) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    std::cout << std::endl;

    std::cout << "erase test:"<< std::endl;
    int p = list1.size();
    for (int i = 0; i < 4; ++i){
        list1.erase(it);}
    if (list1.size() == p - 4) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    std::cout << std::endl;

    std::cout << "clear test:"<< std::endl;
    list1.clear();
    if (list1.size() == 0) std::cout << "True" << std::endl;
    else std::cout << "False" << std::endl;

    return 0;
}
