/* ------------------------------------------------------------------   */
/*      item            : Counter.cxx
        made by         : Rene' van Paassen
        date            : 170721
        category        : body file
        description     :
        changes         : 170721 first version
        language        : C++
*/

#define Counter_cxx
#include "Counter.hxx"
#include <algorithm>

#ifdef PDEBUG
#include <iostream>
#define PDEB(A) std::cerr << A << std::endl;
#else
#define PDEB(A)
#endif

namespace flexistick {

  Counter::Counter() :
    FlexiLink(),
    minval(0),
    defval(0),
    maxval(0),
    value(0),
    cup(false),
    cdown(false)
  {
    //
  }

  Counter::~Counter()
  {
    //
  }

  void Counter::defineCounter(int cmin, int cmax, int ccenter)
  {
    minval = cmin;
    maxval = cmax;
    defval = ccenter;
    value = defval;
  }

  void Counter::propagate(bool& v, unsigned idx)
  {
    PDEB("Counter conversion, idx " << idx << " val " << v
         << " oldstate " << value);
    switch(idx) {
    case 0:
      if (v && !cdown) {
        value = std::max(minval, value - 1);
        changed = true;
      }
      cdown = v;
      break;
    case 1:
      if (v && !cup) {
        value = std::min(maxval, value + 1);
        changed = true;
      }
      cup = v;
      break;
    case 2:
      if (v) {
        value = defval;
        changed = true;
      }
      break;
    default:
      assert(0);
    }
    if (changed) {
      PDEB("Counter newstate " << value);
      for (t_flexilink::iterator ll = receivers.begin();
           ll != receivers.end(); ll++) {
        ll->second->propagate(value, ll->first);
      }
      changed = false;
    }
  }

  void Counter::takeSnapshot(dueca::smartstring::json_string_writer& writer)
  {
    writer.StartObject();
    writer.Key("value");
    writer.Double(value);
    writer.Key("cup");
    writer.Bool(cup);
    writer.Key("cdown");
    writer.Bool(cdown);
    writer.EndObject();
  }

  void Counter::loadSnapshot(JValue& doc)
  {
    assert(doc.IsObject());
    for (JValue::ConstMemberIterator it = doc.MemberBegin();
         it != doc.MemberEnd(); ++it) {
      if (it->name.GetString() == std::string("value")) {
        value = it->value.GetDouble();
      }
      else if (it->name.GetString() == std::string("cup")) {
        cup = it->value.GetBool();
      }
      else if (it->name.GetString() == std::string("cdown")) {
        cdown = it->value.GetBool();
      }
    }
  }
}
