#include <chrono>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <deque>

typedef unsigned int uint;

class TimeMeasure{
private:
    std::chrono::high_resolution_clock::time_point start, stop;
public:
    void Start(){
        start = std::chrono::high_resolution_clock::now();
    }

    void Stop(){
        stop = std::chrono::high_resolution_clock::now();
    }

    size_t elapsed() const{
        auto elapsed_time = stop - start;
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count();
    }
};

template <typename T>
class Tester
{
private:
    const uint delta_, N_max_, N_min_;
    static const typename T::iterator it_end(T& my_vector){
        const typename T::iterator it_end = my_vector.end();
        return it_end;
    }
    static const typename T::iterator it_mid(T& my_vector){
        typename T::iterator it = my_vector.begin();
        typename T::iterator it_end = my_vector.end();
        uint mid = std::distance(it, it_end) / 2;
        std::advance(it, mid);
        return it;
    }

    void test(const uint p, const std::string& file, const typename T::iterator it(T&)) const{
        T cont;
        TimeMeasure timer;
        std::ofstream out;
        out.open(file);
        for (uint N = N_min_; N < N_max_; N += delta_){
            cont.clear();
            cont.resize(N);
            timer.Start();
            for (uint j = 0; j < p; ++ j) {
                const typename T::iterator iter = it(cont);
                cont.insert(iter, 0);}
            timer.Stop();
            size_t el = timer.elapsed();
            double tel = static_cast<const double>(el);
            out << N << '\t' << tel << std::endl;
            std::cout << N << std::endl;}
        out.close();
    }

public:
    Tester(const uint delta, const uint N_min, const uint N_max): delta_{delta}, N_min_{N_min}, N_max_{N_max}{}
    void test_mid(const std::string& file){
        test(1000, file, it_mid);
    }
    void test_back(const std::string& file) const{
        test(1000, file, it_end);
    }
};

int main(){
    Tester<std::vector<uint>> vector_tester(100000, 1000000, 5000000);
    Tester<std::deque<uint>> deque_tester(100000, 1000000, 5000000);
    vector_tester.test_back("vector_back.txt");
    vector_tester.test_mid("vector_mid.txt");
    deque_tester.test_back("deque_back.txt");
    deque_tester.test_mid("deque_mid.txt");
    return 0;
}
