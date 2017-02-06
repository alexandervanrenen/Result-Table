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

struct Column {
     string tag;
     uint64_t width;
};

void ResultSet::serialize(ostream& os) const
{
    Column lableColumn{"", 0};
    vector<Column> tableLayout;

    // Find all columns and calculate their minimum length
    for(uint i=0; i<resultLines.size(); i++) {
        lableColumn.width = max(lableColumn.width, static_cast<uint64_t>(resultLines[i]->getTag().size()));
        for(auto field=resultLines[i]->getFields().begin(); field!=resultLines[i]->getFields().end(); field++) {
            // Find colum with same tag
            auto column = tableLayout.begin();
            while(column!=tableLayout.end() && column->tag!=(*field)->getTag()) column++;
            if(column == tableLayout.end())
                tableLayout.push_back(Column{(*field)->getTag(), max((*field)->getTag().size(), (*field)->getValue(kPrecision).size())}); else
                column->width = max(column->width , static_cast<uint64_t>((*field)->getValue(kPrecision).size()));
        }
    }

    // Get total width
    uint32_t totalWidth = 0 + (kPrintBorder?4:0) + kPadding;
    totalWidth += lableColumn.width;
    for(auto column=tableLayout.begin(); column!=tableLayout.end(); column++)
        totalWidth += column->width + kPadding + (kPrintBorder?3:0);

    // Print the header
    if(kPrintBorder) os << string(totalWidth, '-') << endl;
    os << (kPrintBorder?"| ":"") << left << setw(lableColumn.width+kPadding) << "label" << (kPrintBorder?" | ":"") << (kOrientation==kLeft?left:right);
    for(auto column=tableLayout.begin(); column!=tableLayout.end(); column++)
        os << setw(column->width+kPadding) << column->tag << (kPrintBorder?" | ":"");
    os << endl;

    // Print each line of the content
    for(auto line=resultLines.begin(); line!=resultLines.end(); line++) {
        // Print label of this line
        if(kPrintBorder) os << string(totalWidth, '-') << endl;
        os << left << (kPrintBorder?"| ":"") << setw(lableColumn.width+kPadding) << (*line)->getTag() << (kPrintBorder?" | ":"") << (kOrientation==kLeft?left:right);

        // Print results of this line
        for(auto column=tableLayout.begin(); column!=tableLayout.end(); column++) {
            auto result = (*line)->getFields().begin();
            while(result!=(*line)->getFields().end() && (*result)->getTag()!=column->tag) result++;
            if(result == (*line)->getFields().end())
                os << setw(column->width+kPadding) << " " << (kPrintBorder?" | ":""); else
                os << setw(column->width+kPadding) << (*result)->getValue(kPrecision) << (kPrintBorder?" | ":"");
        }
        os << endl;
    }
    if(kPrintBorder) os << string(totalWidth, '-') << endl;
}
