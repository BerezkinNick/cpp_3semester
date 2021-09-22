#include <iostream>
#include <set>
#include <random>
#include <fstream>

std::ofstream out;

class State {
public:
    virtual bool contains(int s) const {
        return false;
    }
};

class DiscreteState : public State {

private:
    int const state;

public:
    DiscreteState(int state): state(state) { }
    virtual bool contains(int s) const {
        return s == state;
    }
};

class SegmentState: public State {
private:
    int const beg, end;

public:
    SegmentState(): beg(0), end(-1) { }
    SegmentState(int beg, int end): beg(beg), end(end) {}
    virtual bool contains(int s) const {
        return s >= beg && s <= end;
    }
};



class SetState : public State {
private:
    std::set<int> const states;
public:
    SetState(): states() { }
    SetState(std::set<int> const &src): states(src) { }
    virtual bool contains(int s) const {
        return states.count(s) > 0;
    }
};

class Intersection: public State {
private:
    SegmentState ss;
    SetState sts;

public:
    Intersection(SegmentState ss, SetState sts): ss(ss), sts(sts) { }

    bool contains(int s) const {
        return (ss.contains(s) && sts.contains(s));
    }
};

class Union: public State {
private:
    SegmentState ss;
    SetState sts;

public:
    Union(SegmentState ss, SetState sts): ss(ss), sts(sts) { }

    bool contains(int s) const {
        return (ss.contains(s) || sts.contains(s));
    }
};

class Complement: public State {
private:
    SegmentState ss;

public:
    Complement(SegmentState ss): ss(ss){ }

    bool contains(int s) const {
        return (! (ss.contains(s)));
    }
};

class Exception: public State {
private:
    SegmentState ss;
    SetState sts;

public:
    Exception(SegmentState ss, SetState sts): ss(ss), sts(sts){ }

    bool contains(int s) const {
        return ( (ss.contains(s)) && ( ! ( sts.contains(s) ) ) );
    }
};

class ProbabilityTest {
private:
    unsigned seed;
    int test_min, test_max;
    unsigned test_count;

public:
    ProbabilityTest(unsigned seed, int test_min, int test_max, unsigned test_count): seed(seed), test_min(test_min),test_max(test_max), test_count(test_count) { }
    float operator()(State const &s) const {
        std::default_random_engine rng(seed);
        std::uniform_int_distribution<int> dstr(test_min,test_max);
        unsigned good = 0;
        for (unsigned cnt = 0; cnt != test_count; ++cnt)
            if (s.contains(dstr(rng))) ++good;
        return static_cast<float>(good)/static_cast<float>(test_count);
    }
};

int main(int argc, const char * argv[]) {
    const unsigned seed = 55;
    DiscreteState d(1);

    SegmentState ss(1,10);

    SetState sts1({2, 3, 20});
    SetState sts2({2, 3, 20, 30, 40, 50});
    SetState sts3({2, 3, 5, 30, 40, 50});

    Intersection I(ss, sts1);

    Union U(ss, sts2);

    Complement C(ss);

    Exception E(ss, sts3);

    std::ofstream out;

/*Тестируем:*/
   out.open("Segment.txt");
    for (size_t i = 100; i < 100000; i += 100)
{
    ProbabilityTest tester(seed, 1, 100, i);
    out << i << ' ' << tester(ss) << std::endl;
}
    out.close();

    out.open("Set.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest tester(seed, 1, 100, i);
        out << i << ' ' << tester(sts1) << std::endl;
    }
    out.close();

    out.open("Intersection.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest tester(seed, 1, 100, i);
        out << i << ' ' << tester(I) << std::endl;
    }
    out.close();

    out.open("Union.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest tester(seed, 1, 100, i);
        out << i << ' ' << tester(U) << std::endl;
    }
    out.close();

    out.open("Complement.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest tester(seed, 1, 100, i);
        out << i << ' ' << tester(C) << std::endl;
    }
    out.close();

    out.open("Exception.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest tester(seed, 1, 100, i);
        out << i << ' ' << tester(E) << std::endl;
    }
    out.close();

    return 0;
}