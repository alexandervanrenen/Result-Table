#include "include/ResultSet.hpp"
#include "include/ResultField.hpp"
#include "include/ResultLine.hpp"
#include <string>
#include <iostream>

using namespace std;

int main()
{
    ResultLine first("first");
    first.addField(TimeField("setup", chrono::nanoseconds(212322)));
    first.addField(ByteField("memory", 54648));
    first.addField(RatioField("%", 0.23123));

    ResultLine second("second");
    second.addField(TimeField("setup", chrono::nanoseconds(21232)));
    second.addField(ValueField("counter", 1000000));

    ResultSet set;
    set.padding(2).border(true).orientation(ResultSet::Orientation::kRight);
    set.addResultLine(first);
    set.addResultLine(second);
    cout << set << endl;

    return 0;
}