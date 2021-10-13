#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <clocale>

using namespace std;

typedef unsigned char uchar;
typedef unsigned int uint;

class Polinom
{
private:
    int coeff, power;
    int* const ptr_coeff;
    int* const ptr_power;
    int* is_N_read;
    uint size, resize;
    string str;
    bool is_power_read, check_coeff, coeff_ready_, sign_check, is_x_read_;
    vector <int> coeff_vector;

    static uint get_lenght(const int N)
    {
        if (N == 0){
            return 1;
        }
        int copy_N = N;
        uint lenght = 0;
        if (copy_N < 0){
            copy_N *= -1;
            lenght ++;
        }
        while(copy_N > 0){
            copy_N /= 10;
            lenght ++;
        }
        return lenght;
    }

    void resize_vector()
    {
        if (size <= power){
            uint new_size = resize * power;
            uint new_size_checked = std::max(size, new_size);
            coeff_vector.resize(new_size_checked);
            size = new_size_checked;
        }
    }

    void prev_coeff() {
        if (coeff_ready_) {
            if (!check_coeff)
                coeff = 1;
            if (!sign_check)
                coeff = -coeff;
        }
        if (is_x_read_ && !is_power_read) {
            power = 1;
        }
        if (!is_x_read_ && !is_power_read) {
            power = 0;
        }
        coeff_ready_ = true;
        if (coeff_ready_) {
            resize_vector();
            coeff_vector[power] += coeff;
        }
    }

public:
    Polinom(const string polinom): size{0}, resize{2},
                                   coeff{0}, power{0},
                                   ptr_coeff{&coeff}, ptr_power{&power}, is_N_read{ptr_coeff},
                                   coeff_vector{0},
                                   is_power_read{false}, check_coeff{false}, coeff_ready_{false}, sign_check{true}, is_x_read_{false},
                                   str{polinom}
    {
        size = str.size();
        coeff_vector.resize(size);
        for (uchar symbol: str){
            if (symbol == '+'){
                prev_coeff();
                sign_check = true;
                coeff = 0;
                power = 0;
                is_power_read= false;
                check_coeff = false;
                is_x_read_ = false;
                is_N_read = ptr_coeff;
            }
            if (symbol == '-'){
                prev_coeff();
                sign_check = false;
                coeff = 0;
                power = 0;
                is_power_read= false;
                check_coeff = false;
                is_x_read_ = false;
                is_N_read = ptr_coeff;
            }
            if (symbol >= '0' && symbol <= '9'){
                uint N = symbol - '0';
                *is_N_read = *is_N_read * 10 + N;
                if (is_N_read == ptr_coeff){
                    check_coeff = true;
                }
            }
            if (symbol == 'x'){
                coeff_ready_ = true;
                is_x_read_ = true;
            }
            if (symbol == '^'){
                is_power_read = true;
                is_N_read = ptr_power;
            }
        }
        prev_coeff();
    }

    friend istream& operator>>(istream& s, Polinom& polinom)
    {
        getline(s, polinom.str);
        return s;
    }

    friend ostream& operator<<(ostream& out, Polinom const& polinom)
    {
        uint power = 0;
        for (int coefficient: polinom.coeff_vector){
            if (coefficient != 0){
                out << coefficient << "x^" << power << endl;
            }
            power ++;
        }
        return out;
    }

    vector<int> get_coefficients() const
    {
        return coeff_vector;
    }

    void print()
    {
        ostringstream coefficients, powers;
        for (int power = size - 1; power >= 0; power --){
            int coefficient = coeff_vector[power];
            if (coefficient != 0){
                uint digits_leght_coefficient = get_lenght(coefficient);
                uint digits_leght_power = get_lenght(power);
                uint digits_leght_powerwith_x = digits_leght_power + 2;
                if (digits_leght_powerwith_x >= digits_leght_coefficient){
                    uint digits_d = digits_leght_powerwith_x - digits_leght_coefficient;
                    uint spaces_amount = digits_d / 2;
                    uint p = digits_d % 2;
                    powers << "|x^" << power;
                    coefficients << '|';
                    coefficients << setw(spaces_amount);
                    coefficients << "";
                    coefficients << setw(1);
                    if (p != 0){
                        coefficients << ' ';
                    }
                    coefficients << coefficient;
                    coefficients << setw(spaces_amount);
                    coefficients << "";
                }
                else{
                    uint digits_d = digits_leght_coefficient - digits_leght_powerwith_x;
                    uint spaces_amount = digits_d / 2;
                    uint p = digits_d % 2;
                    coefficients << '|' << coefficient;
                    powers << '|';
                    powers << setw(spaces_amount);
                    powers << "";
                    powers << setw(1);
                    if (p != 0){
                        powers << ' ';
                    }
                    powers << "x^" << power;
                    powers << setw(spaces_amount);
                    powers << "";
                }
                coefficients << setw(1);
                powers << setw(1);
            }
        }
        coefficients << '|';
        powers << '|';
        string coeff_vector_print = coefficients.str();
        string powers_to_print = powers.str();
        cout << powers_to_print << endl;
        cout << coeff_vector_print << endl;
    }
};


class Polinom_test
{
private:
    static bool flag(const vector<int> coefficients, const vector<int> correct_coefficients)
    {
        uint size = correct_coefficients.size();
        for (uint power = 0; power < size; power ++){
            if (coefficients[power] != correct_coefficients[power]){
                return false;
            }
        }
        return true;
    }

public:
    void test(const string strpolinom, const vector<int> correct_coefficients) const
    {
        Polinom polinom(strpolinom);
        vector <int> coefficiensts = polinom.get_coefficients();
        if(flag(coefficiensts, correct_coefficients))
            cout <<  "True" << endl;
        else cout <<  "False" << endl;

    }
};

int main()
{   Polinom_test polinom_test;
    polinom_test.test("1", vector<int> {1});
    polinom_test.test("-10", vector<int> {-10});
    polinom_test.test("5-2+3", vector<int> {6});
    polinom_test.test("x", vector<int> {0, 1});
    polinom_test.test("-2x", vector<int> {0, -2});
    polinom_test.test("5x-2x+3x", vector<int> {0, 6});
    polinom_test.test("3x^3", vector<int> {0, 0, 0, 3});
    polinom_test.test("x^2-2x^2", vector<int> {0, 0, -1});
    polinom_test.test("x^2-2x^2+3x^2", vector<int> {0, 0, 2});
    polinom_test.test("10+3x-4x+5x^2-7x^2", vector<int> {10, -1, -2});
    cout<<"3x^2+10x+16-3"<<endl;
    Polinom polinom("3x^2+10x+16-3");
    polinom.print();
    return 0;
}
