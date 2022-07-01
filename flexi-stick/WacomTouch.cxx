/* ------------------------------------------------------------------   */
/*      item            : WacomTouch.cxx
        made by         : Rene' van Paassen
        date            : 170920
	category        : body file 
        description     : 
	changes         : 170920 first version
        language        : C++
        copyright       : (c) 2017 TUDelft-AE-C&S
*/

#define WacomTouch_cxx

#ifdef WACOMTOUCH
#include "WacomTouch.hxx"

namespace flexistick {

  WacomTouch::WacomTouch(const std::string& name, unsigned nfingers) :
    HIDBase(name)
  {
    fingers.resize(nfingers);
  }

  WacomTouch::~WacomTouch()
  {
    // 
  }

  void WacomTouch::propagate()
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
    }
  }

  boost::intrusive_ptr<FlexiLink> WacomTouch::getFingerX(unsigned idx)
  {
    if (idx >= fingers.size()) { throw(jsinputnotpresent()); }
    return fingers[idx].x;
  }

  boost::intrusive_ptr<FlexiLink> WacomTouch::getFingerY(unsigned idx)
  {
    if (idx >= fingers.size()) { throw(jsinputnotpresent()); }
    return fingers[idx].y;
  }

  boost::intrusive_ptr<FlexiLink> WacomTouch::getFingerStatus(unsigned idx)
  {
    if (idx >= fingers.size()) { throw(jsinputnotpresent()); }
    return fingers[idx].state;
  }

  int WacomTouch::newData(const WacomMTFingerCollection* ev) {
   
    WacomMTFinger *fd = ev->Fingers;
    for (unsigned f = ev->FingerCount; f--; ) {
      switch(fd->TouchState) {
      case WMTFingerStateUp:
	fingers[fd->FingerID].state->update(false);
	fingers[fd->FingerID].x->update((fd->X - offsetX) / scaleX);
	fingers[fd->FingerID].y->update((fd->Y - offsetY) / scaleY);
	break;
	
      case WMTFingerStateDown:
	fingers[fd->FingerID].state->update(true);
      case WMTFingerStateHold:
	fingers[fd->FingerID].x->update((fd->X - offsetX) / scaleX);
	fingers[fd->FingerID].y->update((fd->Y - offsetY) / scaleY);
	break;

      case WMTFingerStateNone:
	break;
      }
      fd++;
    }
    return 0;
  }
    
}
 
#endif  
