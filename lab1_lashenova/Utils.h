#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <limits>
#include <string>
#include <chrono>
#include <fstream>

using namespace std;
#define INPUT_LINE(in, str) getline(in>>ws, str); \
						cerr << str << endl

class redirect_output_wrapper
{
    ostream& stream;
    streambuf* const old_buf;
public:
    redirect_output_wrapper(ostream& src)
        :old_buf(src.rdbuf()), stream(src)
    {
    }

    ~redirect_output_wrapper() {
        stream.rdbuf(old_buf);
    }
    void redirect(ostream& dest)
    {
        stream.rdbuf(dest.rdbuf());
    }
};

template <typename T>
T GetCorrectNumber(T min, T max)
{
    T x;
    while ((cin >> x).fail()	// check type
        || cin.peek() != '\n'	// is buffer empty (int/float check)
        || x < min || x > max)		// check range
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Type number (" << min << "-" << max << "):";
    }
    cerr << x << endl;
    return x;
}
#endif // UTILS_H
