
/** Retrieve an event from the data */
void getEvent(SDL_Event& ev);

/** Construct from an event */
SDLEventData(const SDL_Event& ev);

// end the class
};

#define __CUSTOM_COMPATLEVEL_110
#define __CUSTOM_COMPATLEVEL_MSGPACK_1
#define __CUSTOM_MSGPACK_PACK_SDLEventData
#include <dueca/msgpack.hxx>
#include <dueca/msgpack-unstream-iter.hxx>

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

namespace dummy {
