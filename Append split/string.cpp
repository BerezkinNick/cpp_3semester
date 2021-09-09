#include <iostream>
#include <vector>

class DemoStr {
public:
    DemoStr() : data_{nullptr}, length_{0}, capacity_{length_}{
    }

    DemoStr(DemoStr const& other) : length_{other.length_}, capacity_{length_}{
        allocate(other.length_);
        copy_str(other.data_, data_, other.length_);
    }

    DemoStr(char const* raw_, size_t length) : length_{length}, capacity_{length_}{
        allocate(length);
        copy_str(raw_, data_, length);
    }

    explicit DemoStr(int value) {
        length_ = get_value_length(value);
        capacity_ = length_;
        allocate(length_);
        auto it = &data_[length_ - 1];
        while (value > 0) {
            *it = value % 10 + '0';
            --it;
            value /= 10;
        }
    }

    void print() const {
        print(std::cout);
    }

    void print(std::ostream& out) const {
        for (auto i = 0u; i < length_; ++i)
            out << data_[i];
    }

    char at(size_t index) const {
        return data_[index];
    }

    bool equal(DemoStr const& other) const {
        if (other.length_ != length_)
            return false;
        for (auto i = 0u; i < length_; ++i)
            if (data_[i] != other.data_[i])
                return false;

        return true;
    }

    void read_from(std::istream& in) {
        char tmp;
        while (in >> tmp) {
            if (length_ >= capacity_)
                reallocate((capacity_ == 0) ? 1 : capacity_ * 2);
            data_[length_] = tmp;
            ++length_;
        }
        reformat();
    }

    void shrink_to_fit() {
        reallocate(length_);
    }

    void clear() {
        delete[] data_;
        data_ = nullptr;
        length_ = capacity_ = 0;
    }

    size_t size() {
        return length_;
    }

    ~DemoStr() {
        clear();
    }

protected:
    void reallocate(size_t n) {
        auto old_data = data_;
        data_ = new char[n];
        capacity_ = n;
        copy_str(old_data, data_, std::min(n, length_));
        delete[] old_data;
    }

    void reformat() {
        for (auto i = 0u; i < length_; ++i)
            data_[i] = (i % 2 == 1) ? tolower(data_[i]) :
                       toupper(data_[i]);
    }

    size_t get_value_length(int value) {
        auto len = 0ull;
        while (value > 0) {
            value /= 10;
            ++len;
        }
        return len;
    }

    void allocate(size_t n) {
        data_ = new char[n];
    }

    void copy_str(char const* str, char* dst, size_t n) {
        for (auto i = 0u; i < n; ++i)
            dst[i] = str[i];
    }

    void set_length(size_t n) {
        length_ = n;
    }

    char* data() {
        return data_;
    }

    //friend std::ostream& operator << (std::ostream& out, DemoStr const& v);
private:
    char* data_ = nullptr;
    size_t length_ = 0, capacity_ = 0;
};

std::ostream& operator << (std::ostream& out, DemoStr const& v) {
    v.print(out);
    return out;
}

std::istream& operator >> (std::istream& in, DemoStr& v) {
    v.clear();
    v.read_from(in);
    return in;
}

class NewStr: public DemoStr {
public:
    using DemoStr::DemoStr;
    void append(NewStr xstr) {
        size_t new_size = this->size() + xstr.size();
        this -> reallocate(new_size);
        copy_str(xstr.data(), this->data() + this->size(), xstr.size());
        this -> set_length(new_size);
    }
    std::vector<NewStr> split(char c) {
        std::vector<NewStr> symbols;
        NewStr s;
        for (size_t i = 0; i < this->size(); ++i) {
            if ((i == this->size() - 1) || (this->data()[i] == c)) {
                symbols.push_back(s);
                s.clear();
            }
            else {
                NewStr p(&this->data()[i], 1);
                s.append(p);
            }
        }
        return symbols;
    }
};

int main(int argc, char** argv) {
    char str1[] = "Hello, ";
    NewStr str(str1, sizeof(str1) - 1);
    char str2[] = "world!";
    NewStr str_appended(str2, sizeof(str2) - 1);
    str.append(str_appended);
    str.print();
    std::cout << "\n";
    std::vector<NewStr> parts;
    for (auto const& v : str.split('o'))
        std::cout << v << "\n";
    std::cout << std::endl;
    return 0;
}