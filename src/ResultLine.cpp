#include "ResultLine.hpp"
#include "ResultField.hpp"

using namespace std;

ResultLine::ResultLine(const string& tag)
        : tag(tag)
        , label(label)
{
}

ResultLine::ResultLine(const string& tag, const string& label)
        : tag(tag)
        , label(label)
{
}

ResultLine::ResultLine(const ResultLine& other)
{
   tag = other.tag;
   label = other.label;
   resultFields.reserve(other.resultFields.size());
   for(unsigned i=0; i<other.resultFields.size(); i++)
      resultFields.push_back(other.resultFields[i]->toUnique());
}

void ResultLine::addField(const ResultField& resultField)
{
   resultFields.push_back(resultField.toUnique());
}

void ResultLine::addFields(const ResultLine& other)
{
   for(auto& iter : other.resultFields) {
      resultFields.push_back(iter->toUnique());
   }
}

ResultLine& operator << (ResultLine& resultLine, const ResultField& resultField)
{
   resultLine.addField(resultField);
   return resultLine;
}

const vector<unique_ptr<ResultField>>& ResultLine::getFields() const
{
   return resultFields;
}

const string& ResultLine::getTag() const
{
   return tag;
}

const string& ResultLine::getLabel() const
{
   return label;
}

void ResultLine::setLabel(const string& label)
{
   this->label = label;
}

const string& ResultLine::getTextForVisualization() const
{
   return label;
}
