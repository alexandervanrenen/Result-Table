#include "ResultField.hpp"
#include <sstream>
#include <cmath>
#include <iostream>

using namespace std;

ResultField::ResultField(const string tag)
: tag(tag)
{
}

ResultField::~ResultField()
{
}

string ResultField::getTag() const
{
    return tag;
}

namespace {

uint64_t applyPrecision(uint64_t input, uint32_t precision)
{
    uint32_t digits = log10(input) + 1;
    if(digits <= precision)
        return input;
    uint32_t invalidDigits = pow(10, digits - precision);
    return (uint64_t)((double)input/invalidDigits+.5f)*invalidDigits;
}

double applyPrecisionDouble(double input, uint32_t precision)
{
    return (double)applyPrecision(input*pow(10,precision), precision) / pow(10,precision);
}

}

string TimeField::getValue(uint32_t precision, bool useMetricPrefix, bool printUnits) const
{
    ostringstream os;

    uint64_t timeSpan = (useMetricPrefix ? applyPrecision(nanoseconds.count(), precision) : nanoseconds.count());

    // Convert to right unit
    if(timeSpan < 1000ll || !useMetricPrefix)
        os << timeSpan << (printUnits ? "ns" : "");
    else if(timeSpan < 1000ll * 1000ll)
        os << timeSpan/1000.0f << (printUnits ? "us" : "");
    else if(timeSpan < 1000ll * 1000ll * 1000ll)
        os << timeSpan / 1000.0f / 1000.0f << (printUnits ? "ms" : "");
    else if(timeSpan < 60l * 1000ll * 1000ll * 1000ll)
        os << timeSpan / 1000.0f / 1000.0f / 1000.0f << (printUnits ? "s" : "");
    else if(timeSpan < 60l * 60l * 1000ll * 1000ll * 1000ll)
        os << timeSpan/1000.0f / 1000.0f / 1000.0f / 60.0f << (printUnits ? "m" : "");
    else
        os << timeSpan/1000.0f / 1000.0f / 1000.0f / 60.0f / 60.0f<< (printUnits ? "h" : "");

    return os.str();
}

unique_ptr<ResultField> TimeField::toUnique() const
{
    return unique_ptr<ResultField>(new TimeField(tag, nanoseconds));
}

ValueField::ValueField(const string& tag, uint64_t value)
: ResultField(tag)
, value(value)
{
}

string ValueField::getValue(uint32_t precision, bool useMetricPrefix, bool printUnits) const
{
    ostringstream os;

    uint64_t rounded = (useMetricPrefix ? applyPrecision(value, precision) : value);

    // Convert to right unit
    if(rounded < 1000ll || !useMetricPrefix)
        os << rounded;
    else if(rounded < 1000ll * 1000ll)
        os << rounded / 1000.0f << (printUnits ? "K" : "");
    else if(rounded < 1000ll * 1000ll * 1000ll)
        os << rounded / 1000.0f / 1000.0f << (printUnits ? "M" : "");
    else if(rounded < 1000ll * 1000ll * 1000ll * 1000ll)
        os << rounded / 1000.0f / 1000.0f / 1000.0f << (printUnits ? "G" : "");
    else if(rounded < 1000ll * 1000ll * 1000ll * 1000ll * 1000ll)
        os << rounded / 1000.0f / 1000.0f / 1000.0f / 1000.0f << (printUnits ? "T" : "");
    else if(rounded < 1000ll * 1000ll * 1000ll * 1000ll * 1000ll * 1000ll)
        os << rounded / 1000.0f / 1000.0f / 1000.0f / 1000.0f / 1000.0f << (printUnits ? "P" : "");

    return os.str();
}

unique_ptr<ResultField> ValueField::toUnique() const
{
    return unique_ptr<ResultField>(new ValueField(tag, value));
}

RatioField::RatioField(const string& tag, double ratio)
: ResultField(tag)
, ratio(ratio)
{
}

string RatioField::getValue(uint32_t precision, bool useMetricPrefix, bool printUnits) const
{
    ostringstream os;
    os << applyPrecisionDouble(ratio, precision);
    return os.str();
}

unique_ptr<ResultField> RatioField::toUnique() const
{
    return unique_ptr<ResultField>(new RatioField(tag, ratio));
}

ByteField::ByteField(const string& tag, uint64_t bytes, Encoding encoding)
: ResultField(tag)
, bytes(bytes)
, encoding(encoding)
{
}

string ByteField::getValue(uint32_t precision, bool useMetricPrefix, bool printUnits) const
{
    ostringstream os;

    if(encoding == Encoding::kIEC) {
        if(bytes < 1024ll || !useMetricPrefix)
            os << bytes;
        else if(bytes < 1024ll * 1024ll)
            os << applyPrecisionDouble(bytes / 1024.0f, precision) << (printUnits ? "KiB" : "");
        else if(bytes < 1024ll * 1024ll * 1024ll)
            os << applyPrecisionDouble(bytes / 1024.0f / 1024.0f, precision) << (printUnits ? "MiB" : "");
        else if(bytes < 1024ll * 1024ll * 1024ll * 1024ll)
            os << applyPrecisionDouble(bytes / 1024.0f / 1024.0f / 1024.0f, precision) << (printUnits ? "GiB" : "");
        else if(bytes < 1024ll * 1024ll * 1024ll * 1024ll * 1024ll)
            os << applyPrecisionDouble(bytes / 1024.0f / 1024.0f / 1024.0f / 1024.0f, precision) << (printUnits ? "TiB" : "");
        else if(bytes < 1024ll * 1024ll * 1024ll * 1024ll * 1024ll * 1024ll)
            os << applyPrecisionDouble(bytes / 1024.0f / 1024.0f / 1024.0f / 1024.0f / 1024.0f, precision) << (printUnits ? "PiB" : "");
    } else {
        if(bytes < 1000ll || !useMetricPrefix)
            os << applyPrecision(bytes, precision);
        else if(bytes < 1000ll * 1000ll)
            os << applyPrecision(bytes, precision) / 1000.0f << (printUnits ? "KB" : "");
        else if(bytes < 1000ll * 1000ll * 1000ll)
            os << applyPrecision(bytes, precision) / 1000.0f / 1000.0f << (printUnits ? "MB" : "");
        else if(bytes < 1000ll * 1000ll * 1000ll * 1000ll)
            os << applyPrecision(bytes, precision) / 1000.0f / 1000.0f / 1000.0f << (printUnits ? "GB" : "");
        else if(bytes < 1000ll * 1000ll * 1000ll * 1000ll * 1000ll)
            os << applyPrecision(bytes, precision) / 1000.0f / 1000.0f / 1000.0f / 1000.0f << (printUnits ? "TB" : "");
        else if(bytes < 1000ll * 1000ll * 1000ll * 1000ll * 1000ll * 1000ll)
            os << applyPrecision(bytes, precision) / 1000.0f / 1000.0f / 1000.0f / 1000.0f / 1000.0f << (printUnits ? "PB" : "");
    }

    return os.str();
}

unique_ptr<ResultField> ByteField::toUnique() const
{
    return unique_ptr<ResultField>(new ByteField(tag, bytes, encoding));
}
