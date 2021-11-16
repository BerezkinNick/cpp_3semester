#include <iostream>
#include <string>
#include <typeinfo>
typedef unsigned int uint;

class Handler {
public:
    virtual void* data() = 0;
    virtual void const* data() const = 0;
    virtual std::type_info const& get_type() const = 0;
    virtual ~Handler()
    {}
};

template <typename T>
class TrivialHandler : public Handler{
private:
    T value;
public:
    TrivialHandler(T data) : value{data}{}

    void* data() override {return static_cast<void*>(&value);}

    void const* data() const override {return static_cast<void const*>(&value);}

    std::type_info const& get_type() const {return typeid(T);}
};


class Any {
private:
    Handler* h;

public:
    Any() : h(nullptr){}

    template <typename T>
    Any(const T value): h{new TrivialHandler<T>(value)}{}

    ~Any() {
        delete h;
    }

    template <typename T>
    void replace(const Any& ref) {
        delete h;
        h = new TrivialHandler<T>(ref.as<T>());
    }

    template <typename T>
    void replace(const T& value) {
        delete h;
        h = new TrivialHandler<T>(value);
    }

    template<typename T>
    T& as()
    {
        TrivialHandler<T> & th = dynamic_cast<TrivialHandler<T>&>(*h);
        return *static_cast<T*>(th.data());
    }

    template<typename T>
    const T& as() const
    {
        TrivialHandler<T> const& th = dynamic_cast<TrivialHandler<T>const&>(*h);
        return *static_cast<const T*>(th.data());
    }

    template<typename T>
    bool contains() const {
        return typeid(T) == h->get_type();
    }
};

template <typename T>
class Grid {
private:
    Any* mem;
    size_t x_size, y_size;

    T get_average() {
        T S = 0;
        T amount;
        amount = x_size * y_size;
        for (size_t i = 0; i < amount; i++){
            S += (*this)(i / y_size, i % y_size);
        }
        return S / amount;
    }
public:
    Grid(size_t x_size, size_t y_size) : x_size(x_size), y_size(y_size) {
        mem = new Any [x_size * y_size];
        for (size_t i = 0; i < x_size * y_size; i++){
            T p = 0;
            mem[i].replace(p);
        }
    }

    Grid(const Grid& ref) : x_size(ref.x_size), y_size(ref.y_size) {
        mem = new Any [x_size * y_size];
        for (size_t i = 0; i < x_size * y_size; i++)
        {
            mem[i].replace<T>(ref.mem[i]);
        }
    }

    ~Grid() {
        delete[] mem;
    }

    size_t get_xsize() const {
        return x_size;
    }

    size_t get_ysize() const {
        return y_size;
    }

    Grid& operator = (const Grid& ref) {
        if (this == &ref){
            return *this;
        }

        delete[] this->mem;
        x_size = ref.get_xsize();
        y_size = ref.get_ysize();

        mem = new Any[x_size * y_size];
        for (size_t i = 0; i < x_size * y_size; i++)
        {
            mem[i].replace<T>(ref.mem[i]);
        }
        return *this;
    }

    T operator()(size_t x_idx, size_t y_idx) const {
        if (this->is_subgrid(x_idx, y_idx)) {
            return mem[x_idx * y_size + y_idx].as<Grid<T>>().get_average();
        }
        return mem[x_idx * y_size + y_idx].as<T>();
    }

    T& operator()(size_t x_idx, size_t y_idx) {
        if (this->is_subgrid(x_idx, y_idx)) {
            return mem[0].as<T>();
        }
        return mem[x_idx * y_size + y_idx].as<T>();
    }

    Grid& operator = (T value) {
        for (size_t i = 0; i < x_size * y_size; i++)
        {
            if (!this->is_subgrid(i / y_size, i % y_size)) {
                mem[i].replace(value);
            }
            else
            {
                mem[i].as<Grid<T>>() = value;
            }
        }
        return *this;
    }

    Grid& make_subgrid(size_t x_idx, size_t y_idx, size_t x_sub_size, size_t y_sub_size) {
        T p = 0;
        Grid<T> value2(x_sub_size, y_sub_size);
        auto value1 = this->is_subgrid(x_idx, y_idx) ? p : (*this)(x_idx, y_idx);
        value2 = value1;
        mem[x_idx * y_size + y_idx].replace<Grid<T>>(value2);
        return *this;
    }

    Grid& get_subgrid(size_t x_idx, size_t y_idx){
        return mem[x_idx * y_size + y_idx].as<Grid<T>>();
    }

    Grid const& get_subgrid(size_t x_idx, size_t y_idx) const {
        return mem[x_idx * y_size + y_idx].as<Grid<T>>();
    }

    bool is_subgrid(size_t x_idx, size_t y_idx) const {
        return mem[x_idx * y_size + y_idx].contains<Grid<T>>();
    }

    Grid& collapse_subgrid(size_t x_idx, size_t y_idx) {
        if (!this->is_subgrid(x_idx, y_idx)){
            return *this;
        }
        mem[x_idx * y_size + y_idx].replace<T>(mem[x_idx * y_size + y_idx].as<Grid<T>>().get_average());
        return *this;
    }
};

template <typename T>
std::istream& operator >> (std::istream& in, Grid<T>& grid) {
    for (size_t i = 0; i < grid.get_xsize() * grid.get_ysize(); i++){
        in >> grid(i / grid.get_ysize(), i % grid.get_ysize());
    }
    return in;
}

template <typename T>
std::ostream& operator << (std::ostream& out, Grid<T> const& grid) {
    for (size_t i = 0; i < grid.get_xsize(); i++){
        for (size_t j = 0; j < grid.get_ysize(); j++){
            out << grid(i, j) << '\t';
        }
        out << '\n';
    }
    out << '\n';
    return out;
}


template<typename T>
bool check(const T a, const T b)
{
    if (a == b){
        return true;
    }
    else{
        return false;
    }
}

class Tester
{
public:
    template <typename T>
    static void test_any(const T a, const T a_true, const std::string str)
    {
        const bool flag = check(a, a_true);
        if (flag){
            std::cout << str << " - true" << '\n';
        }
        else{
            std::cerr << str << " - false" << '\n';
        }
    }

    template <typename T>
    static void test_grid(const Grid<T>& grid, const T value_true)
    {
        bool b = 1;
        const size_t x_size = grid.get_xsize();
        const size_t y_size = grid.get_ysize();
        for (size_t i = 0; i < x_size; ++i){
            for (size_t j = 0; j < y_size; ++j){
                const T value = grid(i, j);
                if (!check(value, value_true)){
                    b = 0;
                }
            }
        }
        if (b == 1){
            std::cout <<"True" << '\n';
        }
        else{
            std::cerr << "False" << '\n';
        }
    }

};

void GridTest(Tester tester)
{
    std::cout << "Grid test" << '\n';

    Grid<int> A(1, 1);
    int value_A = 1;
    A = value_A;
    tester.test_grid(A, value_A);

    Grid<char> B(2, 2);
    char value_B = 2;
    B = value_B;
    tester.test_grid(B, value_B);

    Grid<float> C(3, 3);
    float value_C = 3;
    C = value_C;
    tester.test_grid(C, value_C);

    Grid<double> D(1, 2);
    double value_D = 0.5;
    D = value_D;
    tester.test_grid(D, value_D);
}

void SubGridTest(Tester test) {
    std::cout << "Subgrid test" << '\n';
    bool b = 1;
    Grid<float> grid(3, 3);
    grid = 10;
    grid(1, 1) = 1;
    grid.make_subgrid(1, 0, 5, 5);
    Grid<float>& subgrid = grid.get_subgrid(1, 0);
    subgrid = 0;
    subgrid(0, 0) = 50;
    if (!grid.is_subgrid(1, 0) || subgrid(1, 1) != 0 || subgrid(0, 0) != 50)
        b = 0;
    grid.collapse_subgrid(1, 0);
    if (grid(1, 0) != 2 || grid.is_subgrid(1, 0))
        b = 0;
    grid = 1;
    grid.make_subgrid(1, 0, 3, 3);
    grid.make_subgrid(1, 0, 2, 2);
    Grid<float>& subgrid2 = grid.get_subgrid(1, 0);
    if (subgrid2(0, 0) != 0 || subgrid2(1, 1) != 0) b = 0;

    if (b == 1) std::cout << "True"<<std::endl;
}


void AnyTest(Tester tester)
{
    std::cout << "Any test" << '\n';
    const char value_char = 'b';
    Any any(value_char);
    const char result_char = any.as<char>();
    tester.test_any(result_char, value_char, "Char");

    const int value_int = 2;
    any.replace(value_int);
    const int result_int = any.as<int>();
    tester.test_any(result_int, value_int, "Int");

    const float value_float = 1;
    any.replace(value_float);
    const float result_float = any.as<float>();
    tester.test_any(result_float, value_float, "Float");

    const double value_double = 1;
    any.replace(value_double);
    const double result_double = any.as<double>();
    tester.test_any(result_double, value_double, "Double");

    const uint value_uint = 1;
    any.replace(value_uint);
    const uint result_uint = any.as<uint>();
    tester.test_any(result_uint, value_uint, "Uint");
}


int main()
{   Tester tester;
    std::cout<<std::endl;

    AnyTest(tester);
    std::cout<<std::endl;

    GridTest(tester);
    std::cout<<std::endl;

    SubGridTest(tester);
    std::cout<<std::endl;

   /* Grid<float> z(2, 2);
    std::cin >> z;
    std::cout << z;

    Grid<int> p(3, 3);
    std::cin >> p;
    std::cout << p;*/

    return 0;
}