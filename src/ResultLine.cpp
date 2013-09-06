#include "ResultLine.hpp"
#include "ResultField.hpp"

using namespace std;

ResultLine::ResultLine(const string& lineLable)
: lineLable(lineLable)
{
}

ResultLine::ResultLine(const ResultLine& other)
{
   lineLable = other.lineLable;
   resultFields.reserve(other.resultFields.size());
   for(uint i=0; i<other.resultFields.size(); i++)
      resultFields.push_back(other.resultFields[i]->toUnique());
}

void ResultLine::addField(const ResultField& resultField)
{
   resultFields.push_back(resultField.toUnique());
}

ResultLine& operator << (ResultLine& resultLine, const ResultField& resultField)
{
   resultLine.addField(resultField);
   return resultLine;
}

const string ResultLine::getTag() const
{
   return lineLable;
}

const vector<unique_ptr<ResultField>>& ResultLine::getFields() const
{
   return resultFields;
}
