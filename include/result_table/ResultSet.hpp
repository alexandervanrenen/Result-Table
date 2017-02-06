#pragma once

#include <string>
#include <vector>
#include <memory>
#include <ios>

class ResultLine;

class ResultSet {
public:
    enum Orientation{kLeft, kRight};

    /// Constructor
    ResultSet(uint32_t padding = 3, uint32_t precision = 3, bool printUnits = true, bool useMetricPrefix = true, bool printBorder = false, Orientation orientation = Orientation::kLeft);
    ~ResultSet();

    /// Configure
    ResultSet& padding(uint32_t padding);
    ResultSet& precision(uint32_t precision);
    ResultSet& printUnits(bool printUnits);
    ResultSet& border(bool printBorder);
    ResultSet& useMetricPrefix(bool metricPrefix);
    ResultSet& orientation(Orientation leftAligned);

    /// Add a new line
    void addResultLine(const ResultLine& resultLine);
    friend ResultSet& operator << (ResultSet& resultSet, const ResultLine& resultLine);

    /// Output the result set
    std::string str() const;
    friend std::ostream& operator << (std::ostream& os, const ResultSet& resultSet);

private:
    std::vector<std::unique_ptr<ResultLine>> resultLines;

    /// Configuration values (not really constants anymore .. )
    uint32_t kPadding;
    uint32_t kPrecision; // The first x = 3 digits of the results matter. 4444 -> 4440
    bool kPrintUnits;
    bool kUseMetricPrefix;
    bool kPrintBorder;
    Orientation kOrientation;

    void serialize(std::ostream& os) const;
};
