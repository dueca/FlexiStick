/* ------------------------------------------------------------------   */
/*      item            : HIDStick.hxx
        made by         : Rene van Paassen
        date            : 170920
        category        : header file
        description     :
        changes         : 170920 first version
        language        : C++
        copyright       : (c) 2017 TUDelft-AE-C&S
*/

#ifndef HIDStick_hxx
#define HIDStick_hxx

#include <vector>
#include <string>
#include <set>
#include "FlexiLink.hxx"
#ifdef PDEBUG
#include <iostream>
#define PDEB(A) std::cerr << A << std::endl;
#else
#define PDEB(A)
#endif

#include "HIDBase.hxx"

namespace flexistick {

  /** generic base class for devices that read data */
  struct HIDStick: public HIDBase
  {
    /** Define type for storing hat reading results */
    typedef HIDInputValue<int>     HIDInputHat;

    /** Define type for storing axis reading results */
    typedef HIDInputValue<float>   HIDInputAxis;

    /** Define type for storing button reading results */
    typedef HIDInputValue<bool>    HIDInputButton;

    /** List type for hat inputs */
    typedef std::vector<boost::intrusive_ptr<HIDInputHat> >    hats_t;

    /** Hat devices */
    hats_t hats;

    /** List type for axis inputs */
    typedef std::vector<boost::intrusive_ptr<HIDInputAxis> >   axes_t;

    /** Axes source devices */
    axes_t axes;

    /** List type for button inputs */
    typedef std::vector<boost::intrusive_ptr<HIDInputButton> > buttons_t;

    /** Button source devices */
    buttons_t buttons;

    /** Constructor */
    HIDStick(const std::string& name);

    /** Destructor */
    ~HIDStick();

    /** Run any collected changes through the conversion system */
    void propagate();

    /** find a hat */
    boost::intrusive_ptr<FlexiLink> getHat(unsigned idx);
    /** link an axis */
    boost::intrusive_ptr<FlexiLink> getAxis(unsigned idx);
    /** link a button */
    boost::intrusive_ptr<FlexiLink> getButton(unsigned idx);

    /** Filter an up/down/left/right value from hat */
    struct HatFilter: public FlexiLink {
      /** Filter */
      uint8_t filter;
      /** Value */
      bool value;
      /** Propagate function */
      void propagate(int& v, unsigned idx);
      /** Constructor */
      HatFilter(const char hatdir);
    };

  };
}
#endif
