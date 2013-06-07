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

string TimeField::getValue(uint32_t precision) const
{
    ostringstream os;

    uint64_t timeSpan = applyPrecision(nanoseconds.count(), precision);

    // Convert to right unit
    if(timeSpan < 1000ll)
        os << timeSpan << "ns";
    else if(timeSpan < 1000ll * 1000ll)
        os << timeSpan/1000.0f << "us";
    else if(timeSpan < 1000ll * 1000ll * 1000ll)
        os << timeSpan / 1000.0f / 1000.0f << "ms";
    else if(timeSpan < 60l * 1000ll * 1000ll * 1000ll)
        os << timeSpan / 1000.0f / 1000.0f / 1000.0f << "s";
    else if(timeSpan < 60l * 60l * 1000ll * 1000ll * 1000ll)
        os << timeSpan/1000.0f / 1000.0f / 1000.0f / 60.0f << "m";
    else
        os << timeSpan/1000.0f / 1000.0f / 1000.0f / 60.0f / 60.0f<< "h";

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

string ValueField::getValue(uint32_t precision) const
{
    ostringstream os;

    uint64_t rounded = applyPrecision(value, precision);

    // Convert to right unit
    if(rounded < 1000ll)
        os << rounded;
    else if(rounded < 1000ll * 1000ll)
        os << rounded / 1000.0f << "K";
    else if(rounded < 1000ll * 1000ll * 1000ll)
        os << rounded / 1000.0f / 1000.0f << "M";
    else if(rounded < 1000ll * 1000ll * 1000ll * 1000ll)
        os << rounded / 1000.0f / 1000.0f / 1000.0f << "G";
    else if(rounded < 1000ll * 1000ll * 1000ll * 1000ll * 1000ll)
        os << rounded / 1000.0f / 1000.0f / 1000.0f / 1000.0f << "T";
    else if(rounded < 1000ll * 1000ll * 1000ll * 1000ll * 1000ll * 1000ll)
        os << rounded / 1000.0f / 1000.0f / 1000.0f / 1000.0f / 1000.0f << "P";

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

string RatioField::getValue(uint32_t precision) const
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

string ByteField::getValue(uint32_t precision) const
{
    ostringstream os;

    if(encoding == Encoding::kIEC) {
        if(bytes < 1024ll)
            os << bytes;
        else if(bytes < 1024ll * 1024ll)
            os << applyPrecisionDouble(bytes / 1024.0f, precision) << "KiB";
        else if(bytes < 1024ll * 1024ll * 1024ll)
            os << applyPrecisionDouble(bytes / 1024.0f / 1024.0f, precision) << "MiB";
        else if(bytes < 1024ll * 1024ll * 1024ll * 1024ll)
            os << applyPrecisionDouble(bytes / 1024.0f / 1024.0f / 1024.0f, precision) << "GiB";
        else if(bytes < 1024ll * 1024ll * 1024ll * 1024ll * 1024ll)
            os << applyPrecisionDouble(bytes / 1024.0f / 1024.0f / 1024.0f / 1024.0f, precision) << "TiB";
        else if(bytes < 1024ll * 1024ll * 1024ll * 1024ll * 1024ll * 1024ll)
            os << applyPrecisionDouble(bytes / 1024.0f / 1024.0f / 1024.0f / 1024.0f / 1024.0f, precision) << "PiB";
    } else {
        if(bytes < 1000ll)
            os << applyPrecision(bytes, precision);
        else if(bytes < 1000ll * 1000ll)
            os << applyPrecision(bytes, precision) / 1000.0f << "KB";
        else if(bytes < 1000ll * 1000ll * 1000ll)
            os << applyPrecision(bytes, precision) / 1000.0f / 1000.0f << "MB";
        else if(bytes < 1000ll * 1000ll * 1000ll * 1000ll)
            os << applyPrecision(bytes, precision) / 1000.0f / 1000.0f / 1000.0f << "GB";
        else if(bytes < 1000ll * 1000ll * 1000ll * 1000ll * 1000ll)
            os << applyPrecision(bytes, precision) / 1000.0f / 1000.0f / 1000.0f / 1000.0f << "TB";
        else if(bytes < 1000ll * 1000ll * 1000ll * 1000ll * 1000ll * 1000ll)
            os << applyPrecision(bytes, precision) / 1000.0f / 1000.0f / 1000.0f / 1000.0f / 1000.0f << "PB";
    }

    return os.str();
}

unique_ptr<ResultField> ByteField::toUnique() const
{
    return unique_ptr<ResultField>(new ByteField(tag, bytes, encoding));
}
