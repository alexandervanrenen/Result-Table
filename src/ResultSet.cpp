#include "ResultSet.hpp"
#include "ResultLine.hpp"
#include "ResultField.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;

ResultSet::ResultSet(const string& label, uint32_t padding, uint32_t precision, bool printUnits, bool useMetricPrefix, bool printBorder, Orientation orientation)
: kPadding(padding)
, kPrecision(precision)
, kPrintUnits(printUnits)
, kUseMetricPrefix(useMetricPrefix)
, kPrintBorder(printBorder)
, kOrientation(orientation)
, kLabel(label)
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

ResultSet& ResultSet::printUnits(bool printUnits)
{
    kPrintUnits = printUnits;
    return *this;
}

ResultSet& ResultSet::border(bool printBorder)
{
    kPrintBorder = printBorder;
    return *this;
}

ResultSet& ResultSet::useMetricPrefix(bool useMetricPrefix)
{
    kUseMetricPrefix = useMetricPrefix;
    return *this;
}

ResultSet& ResultSet::orientation(Orientation orientation)
{
    kOrientation = orientation;
    return *this;
}

ResultSet& ResultSet::label(const std::string& label)
{
    kLabel = label;
    return *this;
}

void ResultSet::setLabel(const std::string& label)
{
    kLabel = label;
}

const std::string& ResultSet::getLabel() const
{
    return kLabel;
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

ResultLine& ResultSet::getResultLineByTag(const std::string& tag)
{
    // Reuse exiting line
    for(auto& iter : resultLines)
        if(iter->getTag() == tag)
            return *iter;

    // Create new line
   resultLines.push_back(unique_ptr<ResultLine>(new ResultLine(tag)));
   return *resultLines.back();
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
     string label;
     uint64_t width;
};

void ResultSet::serialize(ostream& os) const
{
    // Sanity check
    if(!kPrintUnits && kUseMetricPrefix) {
        cout << "can not hide units and use metric prefixes." << endl;
        throw;
    }

    Column labelColumn{kLabel, kLabel.length()};
    vector<Column> tableLayout;

    // Find all columns and calculate their minimum length
    for(uint i=0; i<resultLines.size(); i++) {
        labelColumn.width = max(labelColumn.width, static_cast<uint64_t>(resultLines[i]->getTextForVisualization().size()));
        for(auto field=resultLines[i]->getFields().begin(); field!=resultLines[i]->getFields().end(); field++) {
            // Find colum with same tag
            auto column = tableLayout.begin();
            while(column!=tableLayout.end() && column->label!=(*field)->getTag()) column++;
            if(column == tableLayout.end())
                tableLayout.push_back(Column{(*field)->getTag(), max((*field)->getTag().size(), (*field)->getValue(kPrecision, kUseMetricPrefix, kPrintUnits).size())}); else
                column->width = max(column->width , static_cast<uint64_t>((*field)->getValue(kPrecision, kUseMetricPrefix, kPrintUnits).size()));
        }
    }

    // Get total width
    uint32_t totalWidth = 0 + (kPrintBorder?4:0) + kPadding;
    totalWidth += labelColumn.width;
    for(auto column=tableLayout.begin(); column!=tableLayout.end(); column++)
        totalWidth += column->width + kPadding + (kPrintBorder?3:0);

    // Print the header
    if(kPrintBorder) os << string(totalWidth, '-') << endl;
    os << (kPrintBorder?"| ":"") << left << setw(labelColumn.width+kPadding) << labelColumn.label << (kPrintBorder?" | ":"") << (kOrientation==kLeft?left:right);
    for(auto column=tableLayout.begin(); column!=tableLayout.end(); column++)
        os << setw(column->width+kPadding) << column->label << (kPrintBorder?" | ":"");
    os << endl;

    // Print each line of the content
    for(auto line=resultLines.begin(); line!=resultLines.end(); line++) {
        // Print label of this line
        if(kPrintBorder) os << string(totalWidth, '-') << endl;
        os << left << (kPrintBorder?"| ":"") << setw(labelColumn.width+kPadding) << (*line)->getTextForVisualization() << (kPrintBorder?" | ":"") << (kOrientation==kLeft?left:right);

        // Print results of this line
        for(auto column=tableLayout.begin(); column!=tableLayout.end(); column++) {
            auto result = (*line)->getFields().begin();
            while(result!=(*line)->getFields().end() && (*result)->getTag()!=column->label) result++;
            if(result == (*line)->getFields().end())
                os << setw(column->width+kPadding) << " " << (kPrintBorder?" | ":""); else
                os << setw(column->width+kPadding) << (*result)->getValue(kPrecision, kUseMetricPrefix, kPrintUnits) << (kPrintBorder?" | ":"");
        }
        os << endl;
    }
    if(kPrintBorder) os << string(totalWidth, '-') << endl;
}
