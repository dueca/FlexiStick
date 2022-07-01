/* ------------------------------------------------------------------   */
/*      item            : FlexiLink.cxx
        made by         : Rene' van Paassen
        date            : 170722
        category        : body file
        description     :
        changes         : 170722 first version
        language        : C++
*/

#define FlexiLink_cxx
#include "FlexiLink.hxx"
#include <debug.h>

namespace flexistick {

  FlexiLink::FlexiLink() :
    intrusive_refcount(0),
    changed(true),
    receivers()
  {
    //
  }

  FlexiLink::~FlexiLink()
  {
    //
  }

  void FlexiLink::link(boost::intrusive_ptr<FlexiLink> target, unsigned idxt)
  {
    receivers.push_back(make_pair(idxt, target));
  }

  void FlexiLink::propagate(bool& v, unsigned idx)
  {
    static bool once = true;
    if (once) {
      W_MOD("propagated bool value unused");
      once = false;
    }
  }

  void FlexiLink::propagate(int& v, unsigned idx)
  {
    static bool once = true;
    if (once) {
      W_MOD("propagated int value unused");
      once = false;
    }
  }

  void FlexiLink::propagate(float& v, unsigned idx)
  {
    static bool once = true;
    if (once) {
      W_MOD("propagated float value unused");
      once = false;
    }
  }

  bool FlexiLink::haveState() const
  { return false; }

  void FlexiLink::takeSnapshot(dueca::smartstring::json_string_writer& writer)
  { assert(0); }

  void FlexiLink::loadSnapshot(JValue& doc)
  { assert(0); }
}
