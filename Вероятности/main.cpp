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

class CWG: public State { // непрерывные с пропусками
private:
    std::vector<SegmentState> ss;
    std::vector<DiscreteState> d;
public:
    CWG(std::vector<SegmentState> ss, std::vector<DiscreteState> d) : ss(ss), d(d){}
    bool contains(int s) const override {
        for (auto const& a : ss) {
            if (a.contains(s)) {
                return true;
            }
        }
        for (auto const& a: d) {
            if (a.contains(s)) {
                return false;
            }
        }
        return false;
    }
};

class CWA: public State { // непрерывные с дополнениями
private:
    std::vector<SegmentState> ss;
    std::vector<DiscreteState> dd;
public:
    CWA(std::vector<SegmentState> ss, std::vector<DiscreteState> dd) : ss(ss), dd(dd) { }

    bool contains(int s) const override {
        for (auto const& a: dd) {
            if (a.contains(s)) {
                return true;
            }
        }
        for (auto const& a: ss) {
            if (a.contains(s)) {
                return true;
            }
        }
        return false;
    }
};

class CWGA: public State { // непрерывные с пропусками и дополнениями
private:
    std::vector<SegmentState> ss;
    std::vector<DiscreteState> dd;
    std::vector<DiscreteState> d;
public:
    CWGA(std::vector<SegmentState> ss, std::vector<DiscreteState> dd, std::vector<DiscreteState> gaps) : ss(ss), dd(dd), d(d) { }

    bool contains(int s) const override {
        for (auto const& a : d) {
            if (a.contains(s)) {
                return false;
            }
        }
        for (auto const& a: dd) {
            if (a.contains(s)) {
                return true;
            }
        }
        for (auto const& a: ss) {
            if (a.contains(s)) {
                return true;
            }
        }
        return false;
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
    static State* CreateCWG(std::vector<SegmentState> cont, std::vector<DiscreteState> gaps) {
        return new CWG(cont, gaps);
    }
    static State* CreateCWA(std::vector<SegmentState> cont, std::vector<DiscreteState> adds) {
        return new CWA(cont, adds);
    }
    static State* CreateCWGA(std::vector<SegmentState> cont, std::vector<DiscreteState> adds, std::vector<DiscreteState> gaps) {
        return new CWGA(cont, adds, gaps);
    }
    
        static void del(State* ptr) {
        delete ptr;
    }

};

class ClassTest {
public:
    bool static SegmentStateTest() {
        std::default_random_engine rng(55);
        std::uniform_int_distribution<int> dstr(0, 1000);
        int min = dstr(rng);
        SegmentState ss(min, 1000);
        for (int i = 1; i <= 1000; i++) {
            if ((i <= 1000) && ss.contains(i) && !((i >= min))) {
                std::cout<<"SegmentStateTest: "<<"False"<<'\n';
                return false;
            }
        }
        std::cout<<"SegmentStateTest: "<<"True"<<'\n';
        return true;
    }

    bool static DiscreteStateTest() {
        std::default_random_engine rng(55);
        std::uniform_int_distribution<int> dstr(0, 1000);
        int value = dstr(rng);
        DiscreteState d(value);
        for (int i = 0; i <= 1000; i++) {
            if (d.contains(i) && !(i == value)) {
                std::cout<<"DiscreteStateTest: "<<"False"<<'\n';
                return false;
            }
        }
        std::cout<<"DiscreteStateTest: "<<"True"<<'\n';
        return true;
    }


    bool static CWGTest() {
        DiscreteState d1(2);
        DiscreteState d2(30);
        DiscreteState d3(92);
        SegmentState ss1(0, 10);
        SegmentState ss2(15, 50);
        SegmentState ss3(90, 100);
        std::vector<SegmentState> cont = {ss1, ss2, ss3};
        std::vector<DiscreteState> gaps = {d1, d2, d3};
        CWG cg(cont, gaps);

        std::vector<int> tests = {1, 3, 90};
        std::vector<bool> results;
        std::vector<bool> v = {true, true, true};

        for (auto& a: tests) {
            results.push_back(cg.contains(a));
        }

        if (results == v) {
            std::cout<<"CWGTest: "<<"True"<<'\n';
            return true;
        }
        std::cout<<"CWGTest: "<<"False"<<'\n';
        return false;
    }

    bool static CWATest() {
        DiscreteState d1(15);
        DiscreteState d2(35);
        DiscreteState d3(70);
        SegmentState s1(0, 10);
        SegmentState s2(20, 30);
        SegmentState s3(40, 100);
        std::vector<SegmentState> cont = { s1, s2, s3 };
        std::vector<DiscreteState> adds = { d1, d2, d3 };
        CWA ca(cont, adds);

        std::vector<int> tests = {11, 35, 45, 100};
        std::vector<bool> results;
        std::vector<bool> v = {false, true, true, true};

        for (auto& a: tests) {
            results.push_back(ca.contains(a));
        }

        if (results == v) {
            std::cout<<"CWATest: "<<"True"<<'\n';
            return true;
        }
        std::cout<<"CWATest: "<<"False"<<'\n';
        return false;
    }

    bool static CWGATest() {
        DiscreteState d1(11);
        DiscreteState d2(25);
        DiscreteState d3(70);
        DiscreteState d4(1);
        DiscreteState d5(29);
        DiscreteState d6(42);
        SegmentState s3(40, 65);
        SegmentState s2(18, 30);
        SegmentState s1(0, 10);


        std::vector<SegmentState> ss = { s1, s2, s3 };
        std::vector<DiscreteState> dd = { d1, d2, d3 };
        std::vector<DiscreteState> d = { d4, d5, d6 };
        CWGA cga(ss, dd, d);
        std::vector<int> tests = {14, 24, 70, 1, 29, 41, 100, 7, 79, 21, 99};
        std::vector<bool> results;
        std::vector<bool> v = {true, true, true, false, false, false, false, true, false, true, false};

        for (auto& a: tests) {
            results.push_back(cga.contains(a));
        }

        if (results == v) {
            std::cout<<"CWGATest: "<<"True"<<'\n';
            return true;
        }
        std::cout<<"CWGATest: "<<"True"<<'\n';
        return false;
    }

    bool static SetStateTest() {
        SetState ss({ 1, 5, 15, 51, 73, 90});

        std::vector<int> tests = {1, 2, 5, 60, 73, 90, 99};
        std::vector<bool> results;
        std::vector<bool> v = {true, false, true, false, true, true, false};

        for (auto& a: tests) {
            results.push_back(ss.contains(a));
        }

        if (results == v) {
            std::cout<<"SetStateTest: "<<"True"<<'\n';
            return true;
        }
        std::cout<<"SetStateTest: "<<"False"<<'\n';
        return false;

    }

    bool static UnionTest() {
        std::default_random_engine rng(55);
        std::uniform_int_distribution<int> dstr(0, 1000);
        int t1 = dstr(rng); int t2 = dstr(rng);
        int t3 = dstr(rng); int t4 = dstr(rng);
        DiscreteState d1(t1); DiscreteState d2(t2);
        SegmentState s1(fmin(t1, t2), fmax(t1, t2));
        SegmentState s2(fmin(t3, t4), fmax(t3, t4));
        Union U1(&d1, &d2);
        Union U2(&s1, &s2);

        for (int i = 0; i <= 100; i++) {
            if (U1.contains(i) && !(i == t1 || i == t2)) {
                return false;
            }

            if (U2.contains(i) && !( (i >= fmin(t1, t2) && i <= fmax(t1, t2))
            || (i >= fmin(t3, t4) && i <= fmax(t3, t4))) ) {
                std::cout<<"UnionTest: "<<"False"<<'\n';
                return false;
            }
        }
        std::cout<<"UnionTest: "<<"True"<<'\n';
        return true;
    }

    bool static IntersectionTest() {
        std::default_random_engine rng(65);
        std::uniform_int_distribution<int> dstr(0, 100);
        int t1 = dstr(rng); int t2 = dstr(rng);
        int t3 = dstr(rng); int t4 = dstr(rng);
        DiscreteState d1(t1); DiscreteState d2(t2);
        SegmentState s1(fmin(t1, t2), fmax(t1, t2));
        SegmentState s2(fmin(t3, t4), fmax(t3, t4));
        Intersection U1(&d1, &d2);
        Intersection U2(&s1, &s2);

        for (int i = 0; i <= 100; i++) {
            if (U1.contains(i) && !(i == t1 && i == t2)) {
                std::cout<<"IntersectionTest: "<<"False"<<'\n';
                return false;
            }

            if (U2.contains(i) && !((i >= fmin(t1, t2) && i <= fmax(t1, t2)) && (i >= fmin(t3, t4) && i <= fmax(t3, t4)))) {
                std::cout<<"IntersectionTest: "<<"False"<<'\n';
                return false;
            }
        }
        std::cout<<"IntersectionTest: "<<"True"<<'\n';
        return true;
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
    ClassTest::SegmentStateTest();
    ClassTest::SetStateTest();
    ClassTest::DiscreteStateTest();
    ClassTest::IntersectionTest();
    ClassTest::UnionTest();
    ClassTest::CWATest();
    ClassTest::CWGTest();
    ClassTest::CWGATest();


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

/*Ð¢ÐµÑÑ‚Ð¸Ñ€ÑƒÐµÐ¼:*/

    out.open("Segment1.txt");
    for (size_t i = 100; i < 1000000; i += 100)
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
    
    CreateState::del(U);
    CreateState::del(I);
    CreateState::del(C);
    CreateState::del(E);

    return 0;
}
