#include <iostream>
typedef unsigned int uint;

class Handler
{
public:
    virtual void* data() = 0;
    virtual const void* data() const = 0;
    virtual ~Handler() = default;

};

template<typename T>
class TrivialHandler: public Handler
{
private:
    T value;
public:
    TrivialHandler(const T data): value{data}{}

    void* data() override {return static_cast<void*>(&value);}

    void const* data() const override {return static_cast<void const*>(&value);}

    std::type_info const& type() const {return typeid(T);}
};

class Any
{
private:
    Handler* h;

public:
    template<typename T>
    Any(const T value): h{new TrivialHandler<T>(value)}{}

    template<typename T>
    void replace(const T& value){
        delete h;
        h = new TrivialHandler<T>(value);
    }

    template<typename T>
    const T& as() const
    {
        TrivialHandler<T> const& th = dynamic_cast<TrivialHandler<T>const&>(*h);
        return *static_cast<const T*>(th.data());
    }
};

template<typename T>
class Grid
{

public:
    Grid(size_t x_size, size_t y_size): x_size{x_size}, y_size{y_size}
    {
        mem = new T[x_size*y_size];
        for (size_t i = 0; i < x_size * y_size; ++i){
            mem[i] = 0;}
    }

    size_t get_xsize() const{
        return x_size;
    }

    size_t get_ysize() const{
        return y_size;
    }

    ~Grid(){
        delete[] mem;
    }

    bool is_subgrid() const {return is_subgrid;}

    Grid& make_subgrid(size_t x_idx, size_t y_idx, size_t x_sub_size, size_t y_sub_size)
    {
        Grid g(x_sub_size, y_sub_size);
        if (mem[y_idx*x_size + x_idx].is_subgrid() == false){
            g = mem[y_idx*x_size + x_idx].data();
        }
        mem[y_idx*x_size + x_idx] = g;
        return *this;
    }

    T operator()(size_t x_idx, size_t y_idx) const
    {
        T value = mem[y_idx * x_size + x_idx];
        return value;
    }

    T& operator()(size_t x_idx, size_t y_idx)
    {
        T& value = mem[y_idx * x_size + x_idx];
        return value;
    }

    Grid& operator = (T value)
    {
        for (size_t i = 0; i < x_size*y_size; ++i){
            mem[i] = value;
        }
        return *this;
    }

    friend std::ostream& operator << (std::ostream& out, Grid const& grid)
    {
        size_t x_size = grid.get_xsize();
        for (size_t i = 0; i < grid.x_size*grid.y_size; ++i){
            T value = grid.mem[i];
            out << value << '\t';
            if ((i + 1) % x_size == 0 && (i + 1) % x_size < grid.x_size * grid.y_size - 1){
                out << '\n';
            }
        }
        return out;
    }

    friend std::istream& operator >>(std::istream& in, Grid& grid)
    {
        for (size_t i = 0; i < grid.x_size * grid.y_size; ++ i){grid.mem[i] = 0;}
        T value;
        size_t i = 0;
        while(in.good() && i < grid.x_size*grid.y_size){
            in >> value;
            grid.mem[i] = value;
            ++ i;
        }
        return in;
    }

private:
    T* mem;
    size_t  x_size, y_size;
};

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
        bool flag = true;
        const size_t x_size = grid.get_xsize();
        const size_t y_size = grid.get_ysize();
        for (size_t i = 0; i < x_size; ++i){
            for (size_t j = 0; j < y_size; ++j){
                const T value = grid(i, j);
                if (!check(value, value_true)){
                    flag = false;
                }
            }
        }
        if (flag){
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

    Grid<float> A(1, 1);
    float value_A = 1;
    A = value_A;
    tester.test_grid(A, value_A);

    Grid<float> B(2, 2);
    float value_B = 2;
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

int main(int argc, char** argv)
{
    Tester tester;
    AnyTest(tester);
    std::cout<<std::endl;

    GridTest(tester);
    std::cout<<std::endl;

    Grid<float> g(2, 2);
    std::cin >> g;
    std::cout << g;

    Grid<int> p(3, 3);
    std::cin >> p;
    std::cout << p;

    return 0;
}
