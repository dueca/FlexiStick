/* ------------------------------------------------------------------   */
/*      item            : HIDStick.cxx
        made by         : Rene' van Paassen
        date            : 170920
        category        : body file
        description     :
        changes         : 170920 first version
        language        : C++
        copyright       : (c) 2017 TUDelft-AE-C&S
*/

#define HIDStick_cxx
#include "HIDStick.hxx"

namespace flexistick {

  HIDStick::HIDStick(const std::string& name) :
    HIDBase(name)
  {
    //
  }


  HIDStick::~HIDStick()
  {
    //
  }

  void HIDStick::propagate()
  {
#ifdef PDEBUG
    unsigned idx = 0;
#endif
    for (hats_t::iterator ih = hats.begin();
         ih != hats.end(); ih++) {
#ifdef PDEBUG
      if ((*ih)->changed) {
        PDEB("Propel hat " << idx);
      }
      idx++;
#endif
      (*ih)->propel();
    }
#ifdef PDEBUG
    idx = 0;
#endif
    for (axes_t::iterator ia = axes.begin();
         ia != axes.end(); ia++) {
#ifdef PDEBUG
      if ((*ia)->changed) {
        PDEB("Propel axis " << idx);
      }
      idx++;
#endif
      (*ia)->propel();
    }
#ifdef PDEBUG
    idx = 0;
#endif
    for (buttons_t::iterator ib = buttons.begin();
         ib != buttons.end(); ib++) {
#ifdef PDEBUG
      if ((*ib)->changed) {
        PDEB("Propel button " << idx);
      }
      idx++;
#endif
      (*ib)->propel();
    }
  }

  boost::intrusive_ptr<FlexiLink> HIDStick::getHat(unsigned idx)
  {
    if (idx >= hats.size()) { throw(jsinputnotpresent()); }
    return hats[idx];
  }

  boost::intrusive_ptr<FlexiLink> HIDStick::getAxis(unsigned idx)
  {
    if (idx >= axes.size()) { throw(jsinputnotpresent()); }
    return axes[idx];
  }

  boost::intrusive_ptr<FlexiLink> HIDStick::getButton(unsigned idx)
  {
    if (idx >= buttons.size()) { throw(jsinputnotpresent()); }
    return buttons[idx];
  }

  HIDStick::
  HatFilter::HatFilter(const char hatdir) :
    value(0xff)
  {
    switch (hatdir) {
    case 'u': filter = SDL_HAT_UP; break;
    case 'd': filter = SDL_HAT_DOWN; break;
    case 'l': filter = SDL_HAT_LEFT; break;
    case 'r': filter = SDL_HAT_RIGHT; break;
    }
  }

  void HIDStick::
  HatFilter::propagate(int& v, unsigned idx)
  {
    if (bool(v & filter) != value) {
      value = bool(v & filter);
      changed = true;
      PDEB("Hat conversion, " << v << " to " << value);
    }
    if (changed) {
      for (FlexiLink::t_flexilink::iterator ll = this->receivers.begin();
           ll != this->receivers.end(); ll++) {
        ll->second->propagate(value, ll->first);
      }
    }
  }

}
