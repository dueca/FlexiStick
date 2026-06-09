
/** Retrieve an event from the data */
void getEvent(SDL_Event& ev);

/** Construct from an event */
SDLEventData(const SDL_Event& ev);

// end the class
};


#define __CUSTOM_COMPATLEVEL_110
#define __CUSTOM_COMPATLEVEL_111
#define __CUSTOM_MSGPACK_PACK_SDLEventData
#include <dueca/msgpack.hxx>
#include <dueca/msgpack-unstream-iter.hxx>

#if DUECA_CONFIG_MSGPACK == 1
#define __CUSTOM_COMPATLEVEL_MSGPACK_1

namespace msgpack {
/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond
namespace adaptor {

/// msgpack pack specialization
template <>
struct pack<SDLEventData> {
  template <typename Stream>
  msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o,
                                      const SDLEventData& v) const
  {
    MSGPACK_DCO_OBJECT(3);
    MSGPACK_DCO_MEMBER(type);
    MSGPACK_DCO_MEMBER(timestamp);
    o.pack_bin(40);
    o.pack_bin_body(v.data.data(), 40);
    return o;
  }
};

} // namespace adaptor
/// @cond
} // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond
} // namespace msgpack

#define __CUSTOM_MSGPACK_VISITOR_SDLEventData
#define __CUSTOM_MSGPACK_UNPACK_SDLEventData
namespace msgunpack {
template<typename S>
void msg_unpack(S& i0, const S& iend, SDLEventData&i)
{
  MSGPACK_CHECK_DCO_SIZE(0);
  MSGPACK_UNPACK_MEMBER(i.type);
  MSGPACK_UNPACK_MEMBER(i.timestamp);
  uint32_t bsize = msgunpack::unstream<S>::unpack_binsize(i0, iend);
  assert(bsize == 40);
  for (auto &ch: i.data ) {
    ch = *i0; ++i0;
    assert(i0 != iend);
  }
};
} // namespace msgunpack

#else
#define __CUSTOM_COMPATLEVEL_MSGPACK_2

/** This ensures that this DCO objects are marked as such when trying
    to pack to msgpack as array */
inline const msgpack_dco_array<SDLEventData>&
mark_for_dco_msgpack(const SDLEventData &obj)
{
  return *reinterpret_cast<const msgpack_dco_array<SDLEventData>*>(&obj);
}

namespace msgpack {
/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond
namespace adaptor {

/// member size for this type of object, including parent members
struct SDLEventData_membersize
{
  /// return number of elements in object
  static constexpr unsigned n_members() {
  return 3U;
  }
};

/// msgpack pack specialization with array coding
template <>
struct pack<msgpack_dco_array<SDLEventData>>:
  public SDLEventData_membersize
{
  // main operator, packs data
  template <typename Stream>
  msgpack::packer<Stream>& operator()(
    msgpack::packer<Stream>& o,
    const msgpack_dco_array<SDLEventData>& v) const
  {
    o.pack_array(this->n_members());
    this->pack_members<Stream>(o, v);
    return o;
  }

  template <typename Stream>
  static void pack_members(msgpack::packer<Stream>& o,
                           const SDLEventData& v)
  {
    o.pack(dueca::messagepack::msgpack_visitor<uint32_t>::variant::mark_for_dco_msgpack(v.type));
    o.pack(dueca::messagepack::msgpack_visitor<uint32_t>::variant::mark_for_dco_msgpack(v.timestamp));
    // o.pack(dueca::messagepack::msgpack_visitor<string40>::variant::mark_for_dco_msgpack(v.data));
    o.pack_bin(40);
    o.pack_bin_body(v.data.data(), 40);
  }
};

/// msgpack pack specialization with object coding
template <>
struct pack<SDLEventData>:
  public SDLEventData_membersize
{
  /// main operator, packs DCO as object
  template <typename Stream>
  msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o,
                                      const SDLEventData& v) const
  {
    o.pack_map(this->n_members());
    this->pack_members<Stream>(o, v);
    return o;
  }

  template <typename Stream>
  static void pack_members(msgpack::packer<Stream>& o,
                           const SDLEventData& v)
  {
    o.pack_str(4);
    o.pack_str_body("type", 4);
    o.pack(v.type);
    o.pack_str(9);
    o.pack_str_body("timestamp", 9);
    o.pack(v.timestamp);
    o.pack_str(4);
    o.pack_str_body("data", 4);
    o.pack_bin(40);
    o.pack_bin_body(v.data.data(), 40);
  }

  // unpack members, assembled in the pack struct to enable referring
  // to parent code
  template<typename S>
  static void unpack_members(S& i0, const S& iend,
                             SDLEventData& v)
  {
    msgunpack::msg_unpack(i0, iend, v.type);
    msgunpack::msg_unpack(i0, iend, v.timestamp);
    // msgunpack::msg_unpack(i0, iend, v.data);
    unsigned sz = msgunpack::unstream<S>::unpack_binsize(i0, iend);
    assert(sz == 40U);
    msgunpack::unstream<S>::unpack_bin(i0, iend, v.data.data(), sz);
   }
};
} // namespace adaptor
/// @cond
} // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond
} // namespace msgpack


#endif

namespace dummy {
