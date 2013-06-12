#include "ResultSet.hpp"
#include "ResultLine.hpp"
#include "ResultField.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;

ResultSet::ResultSet(uint32_t padding, uint32_t precision, bool useUnits, bool printBorder, Orientation orientation)
: kPadding(padding)
, kPrecision(precision)
, kUseUnits(useUnits)
, kPrintBorder(printBorder)
, kOrientation(orientation)
{
}

ResultSet::~ResultSet()
{
}

ResultSet& ResultSet::padding(uint32_t padding)
{
    kPadding = padding;
    return *this;
}

ResultSet& ResultSet::precision(uint32_t precision)
{
    kPrecision = precision;
    return *this;
}

ResultSet& ResultSet::useUnits(bool useUnits)
{
    kUseUnits = useUnits;
    return *this;
}

ResultSet& ResultSet::border(bool printBorder)
{
    kPrintBorder = printBorder;
    return *this;
}

ResultSet& ResultSet::orientation(Orientation orientation)
{
    kOrientation = orientation;
    return *this;
}

void ResultSet::addResultLine(const ResultLine& resultLine)
{
    resultLines.push_back(unique_ptr<ResultLine>(new ResultLine(resultLine)));
}

ResultSet& operator << (ResultSet& resultSet, const ResultLine& resultLine)
{
    resultSet.addResultLine(resultLine);
    return resultSet;
}

string ResultSet::str() const
{
    ostringstream os;
    serialize(os);
    return os.str();
}

ostream& operator << (ostream& os, const ResultSet& resultSet)
{
    resultSet.serialize(os);
    return os;
}

void ResultSet::serialize(ostream& os) const
{
    struct Column {
        string tag;
        uint64_t width;
    };
    Column lableColumn{"", 0};
    vector<Column> tableLayout;

    // Find all columns and calculate their minimum length
    for(auto& line : resultLines) {
        lableColumn.width = max(lableColumn.width, line->getTag().size());
        for(auto& field : line->getFields()) {
            auto column = find_if(tableLayout.begin(), tableLayout.end(),[&field](const Column& column){return field->getTag()==column.tag;});
            if(column == tableLayout.end())
                tableLayout.push_back(Column{field->getTag(), max(field->getTag().size(), field->getValue(kPrecision).size())}); else
                column->width = max(column->width , field->getValue(kPrecision).size());
        }
    }

    // Get total width
    uint32_t totalWidth = 0 + (kPrintBorder?4:0) + kPadding;
    totalWidth += lableColumn.width;
    for(auto& column : tableLayout)
        totalWidth += column.width + kPadding + (kPrintBorder?3:0);

    // Print the header
    if(kPrintBorder) os << string(totalWidth, '-') << endl;
    os << (kPrintBorder?"| ":"") << left << setw(lableColumn.width+kPadding) << "label" << (kPrintBorder?" | ":"") << (kOrientation==kLeft?left:right);
    for(auto& column : tableLayout)
        os << setw(column.width+kPadding) << column.tag << (kPrintBorder?" | ":"");
    os << endl;

    // Print each line of the content
    for(auto& line : resultLines) {
        // Print label of this line
        if(kPrintBorder) os << string(totalWidth, '-') << endl;
        os << left << (kPrintBorder?"| ":"") << setw(lableColumn.width+kPadding) << line->getTag() << (kPrintBorder?" | ":"") << (kOrientation==kLeft?left:right);

        // Print results of this line
        for(auto& column : tableLayout) {
            auto result = find_if(line->getFields().begin(), line->getFields().end(),[&column](const unique_ptr<ResultField>& field){return field->getTag()==column.tag;});
            if(result == line->getFields().end())
                os << setw(column.width+kPadding) << " " << (kPrintBorder?" | ":""); else
                os << setw(column.width+kPadding) << (*result)->getValue(kPrecision) << (kPrintBorder?" | ":"");
        }
        os << endl;
    }
    if(kPrintBorder) os << string(totalWidth, '-') << endl;
}
