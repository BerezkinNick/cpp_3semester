#include <iostream>
#include <vector>
#include <deque>
typedef unsigned int uint;

template <typename container, typename T>
const T transform_reduce(const typename container::iterator it_begin, const typename container::iterator it_end, const T& init, const T(*UnaryOp)(const T&), const T(*BinaryOp)(const T&, const T&)){
    T element;
    T result = init;
    for(typename container::iterator it = it_begin; it != it_end; it = std::next(it)){
        result = BinaryOp(result, UnaryOp(*it));
    }
    return result;
}

template<typename T>
const T id(const T& a){
    return a;
}

template<typename T>
const T sum(const T& a, const T& b){
    const T sum = a + b;
    return sum;
}

template<typename T>
const T multiply(const T& a, const T& b){
    const T product = a * b;
    return product;
}

class Test{
public:
    template<typename T>
    static void check(const T& result, const T& correct_result, const std::string& message){
        if (result == correct_result) std::cout << message << " - True" << std::endl;
        else std::cout << message << " - False" << std::endl;
    }
};


int main(){
    Test tester;
    std::vector<double> v{1, 2, 3, 4, 5};
    std::deque<uint> d{1, 2, 3, 4, 5};
    const double correct_sum = 15;
    const uint correct_product = 120;
    const double (*const id_double)(const double &) = &id<double>;
    const uint (*const id_uint)(const uint &) = &id<uint>;
    const double (*const sum_)(const double &, const double &) = &sum<double>;
    const uint (*const pt_multiply)(const uint &, const uint &) = &multiply<uint>;
    const double result_sum = transform_reduce<std::vector<double>, double>(v.begin(), v.end(), 0, id_double, sum_);
    const uint result_product = transform_reduce<std::deque<uint>, uint>(d.begin(), d.end(), 1, id_uint, pt_multiply);
    tester.check(result_sum, correct_sum, "Sum of elements of vector");
    tester.check(result_product, correct_product, "Product of elements of deque");
    return 0;
}
