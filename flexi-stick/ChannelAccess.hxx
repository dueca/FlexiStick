/* ------------------------------------------------------------------   */
/*      item            : ChannelAccess.hxx
        made by         : Rene van Paassen
        date            : 170722
        category        : header file
        description     :
        changes         : 170722 first version
        language        : C++
*/

#ifndef ChannelAccess_hxx
#define ChannelAccess_hxx

#include <dusime.h>
#include "FlexiLink.hxx"
#include <CommObjectWriter.hxx>
#include <CommObjectElementWriter.hxx>
#include <boost/scoped_ptr.hpp>

#ifdef PDEBUG
#define PDEB(A) std::cerr << A << std::endl;
#else
#define PDEB(A)
#endif

namespace flexistick {

  /** Channel access, opens a channel for writing. */
  struct ChannelAccess {

    /** Reference count */
    unsigned intrusive_refcount;

    /** Write access token */
    ChannelWriteToken token;

    /** Data class name */
    std::string dclass;

    /** Remember whether event */
    bool streamtype;

    /** Remember changed after last write */
    bool changed;

    /** Master's id */
    const GlobalId& master_id;

    /** Call to get it */
    inline const GlobalId& getId() const { return master_id; }

#if defined(DataRecorder_hxx)
    /** Data recorder/replayer, if so configured */
    boost::scoped_ptr<DataRecorder> recorder;
#endif

    /** Define a link between data and members of the written object */
    struct WriteLinkBase: public FlexiLink {
      /** Pointer to the parent */
      ChannelAccess* parent;
      /** Name of the data member */
      std::string mname;
      /** Array index, if applicable */
      int         idx;
      /** constructor */
      WriteLinkBase(ChannelAccess* p, const std::string& vname, int idx);
      /** write the data */
      virtual void write(DCOWriter& w) = 0;
      /** Destructor */
      virtual ~WriteLinkBase() {};
    };

    /** Define a writer for a specific channel data value */
    template<class D>
    class WriteLink: public WriteLinkBase {

      /** Location for calculated result */
      D value;

    public:
      /** constructor */
      WriteLink(ChannelAccess* p, const std::string& vname, int idx);

      /** write the data */
      void write(DCOWriter& w) {
        boost::any v = value;
        if (idx >= 0) {
          w[mname.c_str()].write(v, idx);
          PDEB("Var write " << mname << "[" << idx << "] = " << value);
        }
        else {
          w[mname.c_str()].write(v);
          PDEB("Var write " << mname << " = " << value);
        }
      }

      /** Accept a new value */
      void propagate(bool& v, unsigned idx);

      /** Accept a new value */
      void propagate(int& v, unsigned idx);

      /** Accept a new value */
      void propagate(float& v, unsigned idx);

      /** destructor */
      virtual ~WriteLink() {};
    };

    /** Construct a writelink matching the datatype in the channel
        object */
    WriteLinkBase* createWriteLink(const std::string& vname, int idx);

    /** Type for list of links */
    typedef std::list<boost::intrusive_ptr<WriteLinkBase> > t_writelink;

    /** List of data writing links */
    t_writelink writelink;

    /** Constructor. */
    ChannelAccess(const GlobalId& master,
                  const std::string& nameset, const std::string& dclass,
                  const std::string& label, bool eventtype,
                  const std::string& entity=std::string());

    /** Write the data */
    bool write(const TimeSpec& ts, SimulationState mode);

    /** Check token */
    bool isPrepared();
  };

  template<typename D>
  ChannelAccess::
  WriteLink<D>::WriteLink(ChannelAccess* p,
                          const std::string& vname, int idx) :
    WriteLinkBase(p, vname, idx),
    value(0)
  {
    //
  }


  template<class D>
  void ChannelAccess::WriteLink<D>::propagate(bool& v, unsigned idx)
  { value = v; parent->changed = true;
    PDEB("Writelink " << mname << " index " << idx <<
         " in:" << v << " res:" << value); }

  template<class D>
  void ChannelAccess::WriteLink<D>::propagate(int& v, unsigned idx)
  { value = v; parent->changed = true;
    PDEB("Writelink " << mname << " index " << idx <<
         " in:" << v << " res:" << value); }

  template<class D>
  void ChannelAccess::WriteLink<D>::propagate(float& v, unsigned idx)
  { value = v; parent->changed = true;
    PDEB("Writelink " << mname << " index " << idx <<
         " in:" << v << " res:" << value); }

}
#endif
