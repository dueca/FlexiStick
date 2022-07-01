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

  HIDBase::HIDBase(const std::string& name) :
    intrusive_refcount(0),
    name(name)
  {
    //
  }

  HIDBase::~HIDBase()
  {
    // 
  }
  
  const char* jsinputnotpresent::what() const throw()
  { return "HIDInput ax/btn/hat/finger not present"; }

}
