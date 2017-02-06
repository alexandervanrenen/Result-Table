#pragma once

#include <string>
#include <memory>
#include <chrono>

class ResultField {
public:
    ResultField(const std::string tag);
    virtual ~ResultField();

    std::string getTag() const;
    virtual std::string getValue(uint32_t precision) const = 0;

    virtual std::unique_ptr<ResultField> toUnique() const = 0;

protected:
    std::string tag;
};

/// Gets printed in the optimal time unit
class TimeField : public ResultField {
public:
    template<class TimeType>
    TimeField(const std::string& tag, TimeType time) : ResultField(tag), nanoseconds(std::chrono::duration_cast<std::chrono::nanoseconds>(time)) {}

    virtual std::string getValue(uint32_t precision) const;
    virtual std::unique_ptr<ResultField> toUnique() const;

private:
    std::chrono::nanoseconds nanoseconds;
};

class ValueField : public ResultField {
public:
    ValueField(const std::string& tag, uint64_t value);

    virtual std::string getValue(uint32_t precision) const;
    virtual std::unique_ptr<ResultField> toUnique() const;

private:
    uint64_t value;
};

class RatioField : public ResultField {
public:
    RatioField(const std::string& tag, double ratio);

    virtual std::string getValue(uint32_t precision) const;
    virtual std::unique_ptr<ResultField> toUnique() const;

private:
    double ratio;
};

class ByteField : public ResultField {
public:
    enum Encoding {kSI, kIEC};

    ByteField(const std::string& tag, uint64_t bytes, Encoding encoding = kIEC);

    virtual std::string getValue(uint32_t precision) const;
    virtual std::unique_ptr<ResultField> toUnique() const;

private:
    uint64_t bytes;
    Encoding encoding;
};
