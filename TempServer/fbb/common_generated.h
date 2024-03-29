// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_COMMON_H_
#define FLATBUFFERS_GENERATED_COMMON_H_

#include "flatbuffers/flatbuffers.h"

struct fbVec;

struct EntityInfo;

struct DebugColliderInfo;
struct DebugColliderInfoBuilder;
struct DebugColliderInfoT;

enum class ErrorCode : uint16_t {
  None = 0,
  PacketError = 1,
  InValidSession = 2,
  InValidPos = 101,
  MIN = None,
  MAX = InValidPos
};

inline const ErrorCode (&EnumValuesErrorCode())[4] {
  static const ErrorCode values[] = {
    ErrorCode::None,
    ErrorCode::PacketError,
    ErrorCode::InValidSession,
    ErrorCode::InValidPos
  };
  return values;
}

inline const char *EnumNameErrorCode(ErrorCode e) {
  switch (e) {
    case ErrorCode::None: return "None";
    case ErrorCode::PacketError: return "PacketError";
    case ErrorCode::InValidSession: return "InValidSession";
    case ErrorCode::InValidPos: return "InValidPos";
    default: return "";
  }
}

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) fbVec FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;
  float z_;

 public:
  fbVec() {
    memset(static_cast<void *>(this), 0, sizeof(fbVec));
  }
  fbVec(float _x, float _y, float _z)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)),
        z_(flatbuffers::EndianScalar(_z)) {
  }
  float x() const {
    return flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return flatbuffers::EndianScalar(y_);
  }
  float z() const {
    return flatbuffers::EndianScalar(z_);
  }
};
FLATBUFFERS_STRUCT_END(fbVec, 12);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) EntityInfo FLATBUFFERS_FINAL_CLASS {
 private:
  fbVec pos_;
  fbVec endpos_;
  float spd_;
  uint32_t table_id_;
  uint32_t entity_id_;
  int16_t angle_;
  int16_t padding0__;

 public:
  EntityInfo() {
    memset(static_cast<void *>(this), 0, sizeof(EntityInfo));
  }
  EntityInfo(const fbVec &_pos, const fbVec &_endpos, float _spd, uint32_t _table_id, uint32_t _entity_id, int16_t _angle)
      : pos_(_pos),
        endpos_(_endpos),
        spd_(flatbuffers::EndianScalar(_spd)),
        table_id_(flatbuffers::EndianScalar(_table_id)),
        entity_id_(flatbuffers::EndianScalar(_entity_id)),
        angle_(flatbuffers::EndianScalar(_angle)),
        padding0__(0) {
    (void)padding0__;
  }
  const fbVec &pos() const {
    return pos_;
  }
  const fbVec &endpos() const {
    return endpos_;
  }
  float spd() const {
    return flatbuffers::EndianScalar(spd_);
  }
  uint32_t table_id() const {
    return flatbuffers::EndianScalar(table_id_);
  }
  uint32_t entity_id() const {
    return flatbuffers::EndianScalar(entity_id_);
  }
  int16_t angle() const {
    return flatbuffers::EndianScalar(angle_);
  }
};
FLATBUFFERS_STRUCT_END(EntityInfo, 40);

struct DebugColliderInfoT : public flatbuffers::NativeTable {
  typedef DebugColliderInfo TableType;
  uint8_t type;
  std::unique_ptr<fbVec> center;
  std::vector<uint8_t> data;
  DebugColliderInfoT()
      : type(0) {
  }
};

struct DebugColliderInfo FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef DebugColliderInfoT NativeTableType;
  typedef DebugColliderInfoBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TYPE = 4,
    VT_CENTER = 6,
    VT_DATA = 8
  };
  uint8_t type() const {
    return GetField<uint8_t>(VT_TYPE, 0);
  }
  const fbVec *center() const {
    return GetStruct<const fbVec *>(VT_CENTER);
  }
  const flatbuffers::Vector<uint8_t> *data() const {
    return GetPointer<const flatbuffers::Vector<uint8_t> *>(VT_DATA);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_TYPE) &&
           VerifyField<fbVec>(verifier, VT_CENTER) &&
           VerifyOffset(verifier, VT_DATA) &&
           verifier.VerifyVector(data()) &&
           verifier.EndTable();
  }
  DebugColliderInfoT *UnPack(const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  void UnPackTo(DebugColliderInfoT *_o, const flatbuffers::resolver_function_t *_resolver = nullptr) const;
  static flatbuffers::Offset<DebugColliderInfo> Pack(flatbuffers::FlatBufferBuilder &_fbb, const DebugColliderInfoT* _o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);
};

struct DebugColliderInfoBuilder {
  typedef DebugColliderInfo Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_type(uint8_t type) {
    fbb_.AddElement<uint8_t>(DebugColliderInfo::VT_TYPE, type, 0);
  }
  void add_center(const fbVec *center) {
    fbb_.AddStruct(DebugColliderInfo::VT_CENTER, center);
  }
  void add_data(flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data) {
    fbb_.AddOffset(DebugColliderInfo::VT_DATA, data);
  }
  explicit DebugColliderInfoBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  DebugColliderInfoBuilder &operator=(const DebugColliderInfoBuilder &);
  flatbuffers::Offset<DebugColliderInfo> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<DebugColliderInfo>(end);
    return o;
  }
};

inline flatbuffers::Offset<DebugColliderInfo> CreateDebugColliderInfo(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint8_t type = 0,
    const fbVec *center = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data = 0) {
  DebugColliderInfoBuilder builder_(_fbb);
  builder_.add_data(data);
  builder_.add_center(center);
  builder_.add_type(type);
  return builder_.Finish();
}

inline flatbuffers::Offset<DebugColliderInfo> CreateDebugColliderInfoDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint8_t type = 0,
    const fbVec *center = 0,
    const std::vector<uint8_t> *data = nullptr) {
  auto data__ = data ? _fbb.CreateVector<uint8_t>(*data) : 0;
  return CreateDebugColliderInfo(
      _fbb,
      type,
      center,
      data__);
}

flatbuffers::Offset<DebugColliderInfo> CreateDebugColliderInfo(flatbuffers::FlatBufferBuilder &_fbb, const DebugColliderInfoT *_o, const flatbuffers::rehasher_function_t *_rehasher = nullptr);

inline DebugColliderInfoT *DebugColliderInfo::UnPack(const flatbuffers::resolver_function_t *_resolver) const {
  std::unique_ptr<DebugColliderInfoT> _o = std::unique_ptr<DebugColliderInfoT>(new DebugColliderInfoT());
  UnPackTo(_o.get(), _resolver);
  return _o.release();
}

inline void DebugColliderInfo::UnPackTo(DebugColliderInfoT *_o, const flatbuffers::resolver_function_t *_resolver) const {
  (void)_o;
  (void)_resolver;
  { auto _e = type(); _o->type = _e; }
  { auto _e = center(); if (_e) _o->center = std::unique_ptr<fbVec>(new fbVec(*_e)); }
  { auto _e = data(); if (_e) { _o->data.resize(_e->size()); for (flatbuffers::uoffset_t _i = 0; _i < _e->size(); _i++) { _o->data[_i] = _e->Get(_i); } } }
}

inline flatbuffers::Offset<DebugColliderInfo> DebugColliderInfo::Pack(flatbuffers::FlatBufferBuilder &_fbb, const DebugColliderInfoT* _o, const flatbuffers::rehasher_function_t *_rehasher) {
  return CreateDebugColliderInfo(_fbb, _o, _rehasher);
}

inline flatbuffers::Offset<DebugColliderInfo> CreateDebugColliderInfo(flatbuffers::FlatBufferBuilder &_fbb, const DebugColliderInfoT *_o, const flatbuffers::rehasher_function_t *_rehasher) {
  (void)_rehasher;
  (void)_o;
  struct _VectorArgs { flatbuffers::FlatBufferBuilder *__fbb; const DebugColliderInfoT* __o; const flatbuffers::rehasher_function_t *__rehasher; } _va = { &_fbb, _o, _rehasher}; (void)_va;
  auto _type = _o->type;
  auto _center = _o->center ? _o->center.get() : 0;
  auto _data = _o->data.size() ? _fbb.CreateVector(_o->data) : 0;
  return CreateDebugColliderInfo(
      _fbb,
      _type,
      _center,
      _data);
}

#endif  // FLATBUFFERS_GENERATED_COMMON_H_
