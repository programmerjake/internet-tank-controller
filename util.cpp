#include "util.h"
#include <chrono>
#include <iostream>
#include <cstdlib>

using namespace std;

#if 0 // testing balanced_tree
namespace
{
initializer init1([]()
{
    balanced_tree<float> t, t2;
    t.insert(1.0);
    t.insert(1.0);
    t.insert(5.0);
    t.insert(2.0);
    t.insert(4.0);
    t.insert(3.0);
    t.insert(6.0);
    cout << "tree :";
    for(auto i = t.begin(); i != t.end(); i++)
        cout << " " << *i;
    cout << endl;
    cout << "tree :";
    for(auto i = t.begin(); i != t.end();)
    {
        auto v = *i;
        i = t.erase(i);
        cout << " " << v;
        t.insert(v);
    }
    cout << endl;
    t2 = move(t);
    t = t2;
    cout << "tree range :";
    for(auto i = t.rangeBegin(2.0); i != t.rangeEnd(30); i++)
        cout << " " << *i;
    cout << endl;
    t.erase(3);
    cout << "tree :";
    for(auto i = t.begin(); i != t.end(); i++)
        cout << " " << *i;
    cout << endl;

    exit(0);
});
}
#endif // testing balanced_tree
#if 0 // testing solveCubic
namespace
{
void writeCubic(float a, float b, float c, float d)
{
    cout << a << " + " << b << "x + " << c << "x^2 + " << d << "x^3 = 0\n";
    float roots[3];
    int rootCount = solveCubic(a, b, c, d, roots);
    cout << rootCount << " root" << (rootCount != 1 ? "s" : "") << " :\n";
    for(int i = 0; i < rootCount; i++)
    {
        cout << roots[i] << " : error " << (a + b * roots[i] + c * (roots[i] * roots[i]) + d * (roots[i] * roots[i] * roots[i])) << endl;
    }
    cout << endl;
}

initializer init2([]()
{
    for(int i = 0; i < 20; i++)
    {
        writeCubic(rand() % 11 - 5, rand() % 11 - 5, rand() % 11 - 5, 0);
    }
    exit(0);
});
}
#endif // testing solveCubic

void * operator new(size_t sz) // for profiling
{
    return malloc(sz);
}

void operator delete(void * ptr) // for profiling
{
    free(ptr);
}
