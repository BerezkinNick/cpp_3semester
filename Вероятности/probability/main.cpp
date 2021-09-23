#include <iostream>
#include <set>
#include <random>
#include <fstream>
std::ofstream out;

class State {
public:
    virtual bool contains(int s) const = 0;
    virtual ~State() = default;
};

class DiscreteState : public State {

private:
    int const state;

public:
    DiscreteState(int state): state(state) { }
    bool contains(int s) const override {
        return s == state;
    }
};

class SegmentState: public State {
private:
    int const beg, end;

public:
    SegmentState(): beg(0), end(-1) { }
    SegmentState(int beg, int end): beg(beg), end(end) {}
    bool contains(int s) const override {
        return s >= beg && s <= end;
    }
};


class SetState : public State {
private:
    std::set<int> const states;
public:
    SetState(): states() { }
    SetState(std::set<int> const &src): states(src) { }
    bool contains(int s) const override {
        return states.count(s) > 0;
    }
};

class Intersection: public State {
private:
    State* s1;
    State* s2;

public:
    Intersection(State* s1, State* s2) : s1(s1), s2(s2) { }

    bool contains(int s) const override{
        return s1->contains(s) && s2->contains(s);
    }
};

class Union: public State {
private:
    State* s1;
    State* s2;
public:
    Union(State* s1, State* s2) : s1(s1), s2(s2) { }

    bool contains(int s) const override{
        return s1->contains(s) || s2->contains(s);
    }
};


class Complement: public State {
private:
    State* ss;

public:
    Complement(State* ss): ss(ss){ }

    bool contains(int s) const override{
        return (! (ss->contains(s)));
    }
};


class Exception: public State {
private:
    State* s1;
    State* s2;

public:
    Exception(State* s1, State* sts): s1(s1), s2(sts){ }

    bool contains(int s) const override{
        return ( (s1->contains(s)) && ( ! ( s2->contains(s) ) ) );
    }
};

class CreateState {
public:

    static State* CreateUnion(State* s1, State* s2) {
        return new Union(s1, s2);
    }
    static State* CreateIntersection(State* s1, State* s2) {
        return new Intersection(s1, s2);
    }

    static State* CreateComplement(State* s) {
        return new Complement(s);
    }

    static State* CreateException(State* s1, State* s2) {
        return new Exception(s1, s2);
    }

};

class ProbabilityTest1 {
private:
    unsigned seed;
    int test_min, test_max;
    unsigned test_count;

public:
    ProbabilityTest1(unsigned seed, int test_min, int test_max, unsigned test_count): seed(seed), test_min(test_min),test_max(test_max), test_count(test_count) { }
    float operator()(State const &s) const {
        std::default_random_engine rng(seed);
        std::uniform_int_distribution<int> dstr(test_min,test_max);
        unsigned good = 0;
        for (unsigned cnt = 0; cnt != test_count; ++cnt)
            if (s.contains(dstr(rng))) ++good;
        return static_cast<float>(good)/static_cast<float>(test_count);
    }
};


class ProbabilityTest {
private:
    unsigned seed;
    int test_min, test_max;
    unsigned test_count;

public:
    ProbabilityTest(unsigned seed, int test_min, int test_max, unsigned test_count): seed(seed), test_min(test_min),test_max(test_max), test_count(test_count) { }
    float operator()(State* const &s) const {
        std::default_random_engine rng(seed);
        std::uniform_int_distribution<int> dstr(test_min,test_max);
        unsigned good = 0;
        for (unsigned cnt = 0; cnt != test_count; ++cnt)
            if (s->contains(dstr(rng))) ++good;
        return static_cast<float>(good)/static_cast<float>(test_count);
    }
};

int main(int argc, const char * argv[]) {
    const unsigned seed = 55;
    DiscreteState d(1);

    SegmentState ss(1,10);

    SegmentState ss2(1,5);
    SegmentState ss3(1,50);




    SetState sts1({2, 3, 20});
    SetState sts2({2, 3, 20, 30, 40, 50});
    SetState sts3({2, 3, 5, 30, 40, 50});

    State* U = CreateState::CreateUnion(&ss, &sts2);
    State* I = CreateState::CreateIntersection(&ss, &sts1);
    State* C = CreateState::CreateComplement(&ss);
    State* E = CreateState::CreateException(&ss, &sts3);


    std::ofstream out;

/*Тестируем:*/

    out.open("Segment1.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest1 tester(seed, 1, 100, i);
        out << i << ' ' << tester(ss2) << std::endl;
    }
    out.close();

    out.open("Segment2.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest1 tester(seed, 1, 100, i);
        out << i << ' ' << tester(ss3) << std::endl;
    }
    out.close();

    out.open("Segment.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest1 tester(seed, 1, 100, i);
        out << i << ' ' << tester(ss) << std::endl;
    }
    out.close();

    out.open("discrete.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest1 tester(seed, 1, 100, i);
        out << i << ' ' << tester(d) << std::endl;
    }
    out.close();

    out.open("Set.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest1 tester(seed, 1, 100, i);
        out << i << ' ' << tester(sts1) << std::endl;
    }
    out.close();

    out.open("Set2.txt");
    for (size_t i = 100; i < 100000; i += 100)
    {
        ProbabilityTest1 tester(seed, 1, 100, i);
        out << i << ' ' << tester(sts2) << std::endl;
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