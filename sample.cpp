#include "include/ResultSet.hpp"
#include "include/ResultField.hpp"
#include "include/ResultLine.hpp"
#include <string>
#include <iostream>

using namespace std;

int main()
{
    ResultLine first("first");
    first << TimeField("setup", chrono::nanoseconds(212322))
          << ByteField("memory", 54648)
          << RatioField("%", 0.23123);

    ResultLine second("second");
    second << TimeField("setup", chrono::nanoseconds(21232))
           << ValueField("counter", 1000000);

    ResultSet set;
    set.padding(2).border(true).orientation(ResultSet::Orientation::kRight);
    set << first << second;
    cout << set << endl;

    return 0;
}