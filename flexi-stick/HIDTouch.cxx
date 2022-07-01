/* ------------------------------------------------------------------   */
/*      item            : HIDTouch.cxx
        made by         : Rene' van Paassen
        date            : 170920
	category        : body file 
        description     : 
	changes         : 170920 first version
        language        : C++
        copyright       : (c) 2017 TUDelft-AE-C&S
*/

#define HIDTouch_cxx
#include "HIDTouch.hxx"

namespace flexistick {

  HIDTouch::HIDTouch(const std::string& name) :
    HIDBase(name),
    relative(false)
  {
    //
  }

  HIDTouch::~HIDTouch()
  {
    // 
  }

  void HIDTouch::propagate()
  {
#ifdef PDEBUG
    unsigned idx = 0;
#endif
    for (auto &finger: fingers) {
#ifdef PDEBUG
      if (finder.changed) {
        PDEB("Propel finger " << finger.idx);
      }
#endif
      finger.x->propel();
      finger.y->propel();
      finger.pressure->propel();
    }
  }

  boost::intrusive_ptr<FlexiLink> HIDTouch::getFingerX(unsigned idx)
  {
    if (idx >= fingers.size()) { throw(jsinputnotpresent()); }
    return fingers[idx].x;
  }

  boost::intrusive_ptr<FlexiLink> HIDTouch::getFingerY(unsigned idx)
  {
    if (idx >= fingers.size()) { throw(jsinputnotpresent()); }
    return fingers[idx].y;
  }

  boost::intrusive_ptr<FlexiLink> HIDTouch::getFingerPressure(unsigned idx)
  {
    if (idx >= fingers.size()) { throw(jsinputnotpresent()); }
    return fingers[idx].pressure;
  }

  boost::intrusive_ptr<FlexiLink> HIDTouch::getFingerStatus(unsigned idx)
  {
    if (idx >= fingers.size()) { throw(jsinputnotpresent()); }
    return fingers[idx].state;
  }

  void HIDTouch::newData(const SDL_TouchFingerEvent& ev) {
    if (ev.type == SDL_FINGERUP) {
      fingers[ev.fingerId].state->update(false);
      return;
    }
    if (ev.type == SDL_FINGERDOWN) {
      fingers[ev.fingerId].state->update(true);
    }
    if (relative) {
      fingers[ev.fingerId].x->update(ev.dx);
      fingers[ev.fingerId].y->update(ev.dy);
    }
    else {
      fingers[ev.fingerId].x->update(ev.x);
      fingers[ev.fingerId].y->update(ev.y);
    }
    fingers[ev.fingerId].pressure->update(ev.pressure);
  }
    
}
 
  
