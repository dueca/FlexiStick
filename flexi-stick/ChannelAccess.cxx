/* ------------------------------------------------------------------   */
/*      item            : ChannelAccess.cxx
        made by         : Rene' van Paassen
        date            : 170722
        category        : body file
        description     :
        changes         : 170722 first version
        language        : C++
*/

#define ChannelAccess_cxx
#include "ChannelAccess.hxx"
#include <DataClassRegistry.hxx>
#include <CommObjectMemberAccess.hxx>
#include <debug.h>

static const char* classname = "flexistick::ChannelAccess";

namespace dueca {
  template<> const char* getclassname<flexistick::ChannelAccess>()
  { return classname; }
  template<>
  const char* getclassname<const flexistick::ChannelAccess>()
  { return classname; }
  template<>
  const char* getclassname(const  flexistick::ChannelAccess&)
  { return classname; }

}


namespace flexistick {




  ChannelAccess::ChannelAccess(const dueca::GlobalId& master,
                               const std::string& nameset,
                               const std::string& dclass,
                               const std::string& label,
                               bool eventtype,
                               const std::string& entity) :
    intrusive_refcount(0),
    token(master, dueca::NameSet(nameset), dclass, label,
          eventtype ? dueca::Channel::Events : dueca::Channel::Continuous,
          dueca::Channel::OneOrMoreEntries),
    dclass(dclass),
    streamtype(!eventtype),
    changed(true),
    master_id(master)
#if defined(DataRecorder_hxx)
    ,recorder(entity.size() ? new dueca::DataRecorder() : NULL)
#endif
  {
#if defined(DataRecorder_hxx)
    if (recorder) {
      recorder->complete(entity, token);
    }
#endif
  }

  bool ChannelAccess::isPrepared()
  {
    bool res = true;
    CHECK_TOKEN(token);
#if defined(DataRecorder_hxx)
    if (recorder) {
      CHECK_RECORDER(*recorder);
    }
#endif
    return res;
  }

  ChannelAccess::
  WriteLinkBase::WriteLinkBase(ChannelAccess* p,
                               const std::string& vname, int idx) :
    FlexiLink(),
    parent(p),
    mname(vname),
    idx(idx)
  {

  }


  ChannelAccess::WriteLinkBase*
  ChannelAccess::createWriteLink(const std::string& vname, int idx)
  {
    // find the entry based on data class name
    dueca::DataClassRegistry_entry_type re =
      dueca::DataClassRegistry::single().getEntry(dclass);

    // accessor to this member
    dueca::CommObjectMemberAccessBasePtr acc =
      dueca::DataClassRegistry::single().getMemberAccessor(re, vname);

    // see if we have a matching class
    if (strcmp(acc->getClassname(), "double") == 0) {
      return new WriteLink<double>(this, vname, idx);
    }
    else if (strcmp(acc->getClassname(), "float") == 0) {
      return new WriteLink<float>(this, vname, idx);
    }
    else if (strcmp(acc->getClassname(), "int32_t") == 0) {
      return new WriteLink<int32_t>(this, vname, idx);
    }
    else if (strcmp(acc->getClassname(), "uint32_t") == 0) {
      return new WriteLink<uint32_t>(this, vname, idx);
    }
    else if (strcmp(acc->getClassname(), "int16_t") == 0) {
      return new WriteLink<int16_t>(this, vname, idx);
    }
    else if (strcmp(acc->getClassname(), "uint16_t") == 0) {
      return new WriteLink<uint16_t>(this, vname, idx);
    }
    else if (strcmp(acc->getClassname(), "int8_t") == 0) {
      return new WriteLink<int8_t>(this, vname, idx);
    }
    else if (strcmp(acc->getClassname(), "uint8_t") == 0) {
      return new WriteLink<uint8_t>(this, vname, idx);
    }
    else if (strcmp(acc->getClassname(), "bool") == 0) {
      return new WriteLink<bool>(this, vname, idx);
    }
    else {
      W_MOD("No recognition of member type " << acc->getClassname());
    }
    return NULL;
  }

  bool ChannelAccess::write(const dueca::TimeSpec& ts, dueca::SimulationState mode)
  {
#if defined(DataRecorder_hxx)
    if (recorder && mode == dueca::SimulationState::Replay) {
      return recorder->channelReplay(ts, token) > 0;
    }
#endif

    if (changed || streamtype) {
      dueca::DataTimeSpec dts
        (ts.getValidityStart(),
         streamtype ? ts.getValidityEnd() : ts.getValidityStart());

      dueca::DCOWriter w(dclass.c_str(), token, ts);
      PDEB("Channel write " << dclass);
      for (t_writelink::iterator wl = writelink.begin();
           wl != writelink.end(); wl++) {
        if (*wl) { (*wl)->write(w); }
      }
      changed = false;

#if defined(DataRecorder_hxx)
      if (recorder && mode == dueca::SimulationState::Advance) {
        recorder->channelRecord(dts, w);
      }
#endif

      return true;
    }
    return false;
  }
}
