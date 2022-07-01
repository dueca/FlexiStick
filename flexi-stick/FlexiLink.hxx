/* ------------------------------------------------------------------   */
/*      item            : FlexiLink.hxx
        made by         : Rene van Paassen
        date            : 170722
        category        : header file
        description     :
        changes         : 170722 first version
        language        : C++
*/

#ifndef FlexiLink_hxx
#define FlexiLink_hxx

#include <boost/intrusive_ptr.hpp>
#include <dueca/smartstring.hxx>
#include <list>
typedef rapidjson::GenericValue<rapidjson::UTF8<> > JValue;

namespace flexistick {

  /** helper function for refcount */
  template <class T> void intrusive_ptr_add_ref(T* t)
  { t->intrusive_refcount++; }

  /** helper function for refcount */
  template <class T> void intrusive_ptr_release(T* t)
  { if (--(t->intrusive_refcount) == 0) { delete t; } }

  /** Base for link between joystick data and channel or calibration */
  struct FlexiLink {

    /** Reference count */
    unsigned intrusive_refcount;

    /** Has the link's value changed? */
    bool changed;

    /** List definition for targets that this link propagates to */
    typedef std::list<std::pair<unsigned,boost::intrusive_ptr<FlexiLink> > >
    t_flexilink;

    /** objects that receive changed values */
    t_flexilink receivers;

    /** Communicate a true/false value */
    virtual void propagate(bool& v, unsigned idx);

    /** Communicate a raw integer value */
    virtual void propagate(int& v, unsigned idx);

    /** Communicate a float value */
    virtual void propagate(float& v, unsigned idx);

    /** Constructor */
    FlexiLink();

    /** Link this to a new client
        @param target new client/user of data
        @param idxt   index used by the client to distinguish links */
    virtual void link(boost::intrusive_ptr<FlexiLink> target, unsigned idxt=0);

    /** Destructor */
    virtual ~FlexiLink();

    /** Have state */
    virtual bool haveState() const;

    /** Code state into json */
    virtual void takeSnapshot(dueca::smartstring::json_string_writer& writer);

    /** Load state from json */
    virtual void loadSnapshot(JValue& doc);
  };
}

#endif
