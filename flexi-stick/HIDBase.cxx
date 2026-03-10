/* ------------------------------------------------------------------   */
/*      item            : HIDBase.cxx
        made by         : Rene' van Paassen
        date            : 170920
        category        : body file
        description     :
        changes         : 170920 first version
        language        : C++
        copyright       : (c) 2017 TUDelft-AE-C&S
*/

#define HIDBase_cxx
#include "HIDBase.hxx"

namespace flexistick {

HIDBase::HIDBase(const std::string &name) :
  intrusive_refcount(0),
  name(name)
{
    //
}

HIDBase::~HIDBase()
{
    //
}

template<>
void HIDBase::HIDInputValue<int>::takeSnapshot(dueca::smartstring::json_string_writer& writer)
{
  writer.Int(value);
}

template<>
void HIDBase::HIDInputValue<int>::loadSnapshot(const JValue& doc)
{
  value = doc.GetInt();
  changed = true;
}

template<>
void HIDBase::HIDInputValue<float>::takeSnapshot(dueca::smartstring::json_string_writer& writer)
{
  writer.Double(value);
}

template<>
void HIDBase::HIDInputValue<float>::loadSnapshot(const JValue& doc)
{
  value = doc.GetFloat();
  changed = true;
}

template<>
void HIDBase::HIDInputValue<bool>::takeSnapshot(dueca::smartstring::json_string_writer& writer)
{
  writer.Bool(value);
}

template<>
void HIDBase::HIDInputValue<bool>::loadSnapshot(const JValue& doc)
{
  value = doc.GetBool();
  changed = true;
}

const char *jsinputnotpresent::what() const throw()
{
  return "HIDInput ax/btn/hat/finger not present";
}

} // namespace flexistick
