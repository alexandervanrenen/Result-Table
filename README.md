Result-Table
============

A small utility library to easily print results from a benchmark.

Sample
------

The following code creates the following table.

```c++
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
```

```bash
---------------------------------------------------------
| label    |    setup |    memory |       % |   counter | 
---------------------------------------------------------
| first    |    212us |   53.4KiB |   0.231 |           | 
---------------------------------------------------------
| second   |   21.2us |           |         |        1M | 
---------------------------------------------------------
```