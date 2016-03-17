// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: scrapedata.proto

#ifndef PROTOBUF_scrapedata_2eproto__INCLUDED
#define PROTOBUF_scrapedata_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_scrapedata_2eproto();
void protobuf_AssignDesc_scrapedata_2eproto();
void protobuf_ShutdownFile_scrapedata_2eproto();

class ScrapeData;
class ScrapeData_Data;

enum ScrapeData_DataType {
  ScrapeData_DataType_PLAIN = 0,
  ScrapeData_DataType_FILE = 1,
  ScrapeData_DataType_BINARY = 2,
  ScrapeData_DataType_STREAM = 3
};
bool ScrapeData_DataType_IsValid(int value);
const ScrapeData_DataType ScrapeData_DataType_DataType_MIN = ScrapeData_DataType_PLAIN;
const ScrapeData_DataType ScrapeData_DataType_DataType_MAX = ScrapeData_DataType_STREAM;
const int ScrapeData_DataType_DataType_ARRAYSIZE = ScrapeData_DataType_DataType_MAX + 1;

const ::google::protobuf::EnumDescriptor* ScrapeData_DataType_descriptor();
inline const ::std::string& ScrapeData_DataType_Name(ScrapeData_DataType value) {
  return ::google::protobuf::internal::NameOfEnum(
    ScrapeData_DataType_descriptor(), value);
}
inline bool ScrapeData_DataType_Parse(
    const ::std::string& name, ScrapeData_DataType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ScrapeData_DataType>(
    ScrapeData_DataType_descriptor(), name, value);
}
// ===================================================================

class ScrapeData_Data : public ::google::protobuf::Message {
 public:
  ScrapeData_Data();
  virtual ~ScrapeData_Data();

  ScrapeData_Data(const ScrapeData_Data& from);

  inline ScrapeData_Data& operator=(const ScrapeData_Data& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ScrapeData_Data& default_instance();

  void Swap(ScrapeData_Data* other);

  // implements Message ----------------------------------------------

  ScrapeData_Data* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ScrapeData_Data& from);
  void MergeFrom(const ScrapeData_Data& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required bytes payload = 1;
  inline bool has_payload() const;
  inline void clear_payload();
  static const int kPayloadFieldNumber = 1;
  inline const ::std::string& payload() const;
  inline void set_payload(const ::std::string& value);
  inline void set_payload(const char* value);
  inline void set_payload(const void* value, size_t size);
  inline ::std::string* mutable_payload();
  inline ::std::string* release_payload();
  inline void set_allocated_payload(::std::string* payload);

  // optional uint64 size = 2;
  inline bool has_size() const;
  inline void clear_size();
  static const int kSizeFieldNumber = 2;
  inline ::google::protobuf::uint64 size() const;
  inline void set_size(::google::protobuf::uint64 value);

  // optional string extension = 3;
  inline bool has_extension() const;
  inline void clear_extension();
  static const int kExtensionFieldNumber = 3;
  inline const ::std::string& extension() const;
  inline void set_extension(const ::std::string& value);
  inline void set_extension(const char* value);
  inline void set_extension(const char* value, size_t size);
  inline ::std::string* mutable_extension();
  inline ::std::string* release_extension();
  inline void set_allocated_extension(::std::string* extension);

  // @@protoc_insertion_point(class_scope:ScrapeData.Data)
 private:
  inline void set_has_payload();
  inline void clear_has_payload();
  inline void set_has_size();
  inline void clear_has_size();
  inline void set_has_extension();
  inline void clear_has_extension();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* payload_;
  ::google::protobuf::uint64 size_;
  ::std::string* extension_;
  friend void  protobuf_AddDesc_scrapedata_2eproto();
  friend void protobuf_AssignDesc_scrapedata_2eproto();
  friend void protobuf_ShutdownFile_scrapedata_2eproto();

  void InitAsDefaultInstance();
  static ScrapeData_Data* default_instance_;
};
// -------------------------------------------------------------------

class ScrapeData : public ::google::protobuf::Message {
 public:
  ScrapeData();
  virtual ~ScrapeData();

  ScrapeData(const ScrapeData& from);

  inline ScrapeData& operator=(const ScrapeData& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ScrapeData& default_instance();

  void Swap(ScrapeData* other);

  // implements Message ----------------------------------------------

  ScrapeData* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ScrapeData& from);
  void MergeFrom(const ScrapeData& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  typedef ScrapeData_Data Data;

  typedef ScrapeData_DataType DataType;
  static const DataType PLAIN = ScrapeData_DataType_PLAIN;
  static const DataType FILE = ScrapeData_DataType_FILE;
  static const DataType BINARY = ScrapeData_DataType_BINARY;
  static const DataType STREAM = ScrapeData_DataType_STREAM;
  static inline bool DataType_IsValid(int value) {
    return ScrapeData_DataType_IsValid(value);
  }
  static const DataType DataType_MIN =
    ScrapeData_DataType_DataType_MIN;
  static const DataType DataType_MAX =
    ScrapeData_DataType_DataType_MAX;
  static const int DataType_ARRAYSIZE =
    ScrapeData_DataType_DataType_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  DataType_descriptor() {
    return ScrapeData_DataType_descriptor();
  }
  static inline const ::std::string& DataType_Name(DataType value) {
    return ScrapeData_DataType_Name(value);
  }
  static inline bool DataType_Parse(const ::std::string& name,
      DataType* value) {
    return ScrapeData_DataType_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // required string name = 1;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 1;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // required int32 id = 2;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 2;
  inline ::google::protobuf::int32 id() const;
  inline void set_id(::google::protobuf::int32 value);

  // optional string quid = 3;
  inline bool has_quid() const;
  inline void clear_quid();
  static const int kQuidFieldNumber = 3;
  inline const ::std::string& quid() const;
  inline void set_quid(const ::std::string& value);
  inline void set_quid(const char* value);
  inline void set_quid(const char* value, size_t size);
  inline ::std::string* mutable_quid();
  inline ::std::string* release_quid();
  inline void set_allocated_quid(::std::string* quid);

  // optional string zone = 4;
  inline bool has_zone() const;
  inline void clear_zone();
  static const int kZoneFieldNumber = 4;
  inline const ::std::string& zone() const;
  inline void set_zone(const ::std::string& value);
  inline void set_zone(const char* value);
  inline void set_zone(const char* value, size_t size);
  inline ::std::string* mutable_zone();
  inline ::std::string* release_zone();
  inline void set_allocated_zone(::std::string* zone);

  // required .ScrapeData.DataType type = 5 [default = PLAIN];
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 5;
  inline ::ScrapeData_DataType type() const;
  inline void set_type(::ScrapeData_DataType value);

  // required .ScrapeData.Data content = 6;
  inline bool has_content() const;
  inline void clear_content();
  static const int kContentFieldNumber = 6;
  inline const ::ScrapeData_Data& content() const;
  inline ::ScrapeData_Data* mutable_content();
  inline ::ScrapeData_Data* release_content();
  inline void set_allocated_content(::ScrapeData_Data* content);

  // @@protoc_insertion_point(class_scope:ScrapeData)
 private:
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_quid();
  inline void clear_has_quid();
  inline void set_has_zone();
  inline void clear_has_zone();
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_content();
  inline void clear_has_content();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* name_;
  ::std::string* quid_;
  ::google::protobuf::int32 id_;
  int type_;
  ::std::string* zone_;
  ::ScrapeData_Data* content_;
  friend void  protobuf_AddDesc_scrapedata_2eproto();
  friend void protobuf_AssignDesc_scrapedata_2eproto();
  friend void protobuf_ShutdownFile_scrapedata_2eproto();

  void InitAsDefaultInstance();
  static ScrapeData* default_instance_;
};
// ===================================================================


// ===================================================================

// ScrapeData_Data

// required bytes payload = 1;
inline bool ScrapeData_Data::has_payload() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ScrapeData_Data::set_has_payload() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ScrapeData_Data::clear_has_payload() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ScrapeData_Data::clear_payload() {
  if (payload_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    payload_->clear();
  }
  clear_has_payload();
}
inline const ::std::string& ScrapeData_Data::payload() const {
  // @@protoc_insertion_point(field_get:ScrapeData.Data.payload)
  return *payload_;
}
inline void ScrapeData_Data::set_payload(const ::std::string& value) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    payload_ = new ::std::string;
  }
  payload_->assign(value);
  // @@protoc_insertion_point(field_set:ScrapeData.Data.payload)
}
inline void ScrapeData_Data::set_payload(const char* value) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    payload_ = new ::std::string;
  }
  payload_->assign(value);
  // @@protoc_insertion_point(field_set_char:ScrapeData.Data.payload)
}
inline void ScrapeData_Data::set_payload(const void* value, size_t size) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    payload_ = new ::std::string;
  }
  payload_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ScrapeData.Data.payload)
}
inline ::std::string* ScrapeData_Data::mutable_payload() {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    payload_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ScrapeData.Data.payload)
  return payload_;
}
inline ::std::string* ScrapeData_Data::release_payload() {
  clear_has_payload();
  if (payload_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = payload_;
    payload_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ScrapeData_Data::set_allocated_payload(::std::string* payload) {
  if (payload_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete payload_;
  }
  if (payload) {
    set_has_payload();
    payload_ = payload;
  } else {
    clear_has_payload();
    payload_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:ScrapeData.Data.payload)
}

// optional uint64 size = 2;
inline bool ScrapeData_Data::has_size() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ScrapeData_Data::set_has_size() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ScrapeData_Data::clear_has_size() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ScrapeData_Data::clear_size() {
  size_ = GOOGLE_ULONGLONG(0);
  clear_has_size();
}
inline ::google::protobuf::uint64 ScrapeData_Data::size() const {
  // @@protoc_insertion_point(field_get:ScrapeData.Data.size)
  return size_;
}
inline void ScrapeData_Data::set_size(::google::protobuf::uint64 value) {
  set_has_size();
  size_ = value;
  // @@protoc_insertion_point(field_set:ScrapeData.Data.size)
}

// optional string extension = 3;
inline bool ScrapeData_Data::has_extension() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ScrapeData_Data::set_has_extension() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ScrapeData_Data::clear_has_extension() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ScrapeData_Data::clear_extension() {
  if (extension_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    extension_->clear();
  }
  clear_has_extension();
}
inline const ::std::string& ScrapeData_Data::extension() const {
  // @@protoc_insertion_point(field_get:ScrapeData.Data.extension)
  return *extension_;
}
inline void ScrapeData_Data::set_extension(const ::std::string& value) {
  set_has_extension();
  if (extension_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    extension_ = new ::std::string;
  }
  extension_->assign(value);
  // @@protoc_insertion_point(field_set:ScrapeData.Data.extension)
}
inline void ScrapeData_Data::set_extension(const char* value) {
  set_has_extension();
  if (extension_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    extension_ = new ::std::string;
  }
  extension_->assign(value);
  // @@protoc_insertion_point(field_set_char:ScrapeData.Data.extension)
}
inline void ScrapeData_Data::set_extension(const char* value, size_t size) {
  set_has_extension();
  if (extension_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    extension_ = new ::std::string;
  }
  extension_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ScrapeData.Data.extension)
}
inline ::std::string* ScrapeData_Data::mutable_extension() {
  set_has_extension();
  if (extension_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    extension_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ScrapeData.Data.extension)
  return extension_;
}
inline ::std::string* ScrapeData_Data::release_extension() {
  clear_has_extension();
  if (extension_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = extension_;
    extension_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ScrapeData_Data::set_allocated_extension(::std::string* extension) {
  if (extension_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete extension_;
  }
  if (extension) {
    set_has_extension();
    extension_ = extension;
  } else {
    clear_has_extension();
    extension_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:ScrapeData.Data.extension)
}

// -------------------------------------------------------------------

// ScrapeData

// required string name = 1;
inline bool ScrapeData::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ScrapeData::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ScrapeData::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ScrapeData::clear_name() {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& ScrapeData::name() const {
  // @@protoc_insertion_point(field_get:ScrapeData.name)
  return *name_;
}
inline void ScrapeData::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set:ScrapeData.name)
}
inline void ScrapeData::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set_char:ScrapeData.name)
}
inline void ScrapeData::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ScrapeData.name)
}
inline ::std::string* ScrapeData::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ScrapeData.name)
  return name_;
}
inline ::std::string* ScrapeData::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ScrapeData::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:ScrapeData.name)
}

// required int32 id = 2;
inline bool ScrapeData::has_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ScrapeData::set_has_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ScrapeData::clear_has_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ScrapeData::clear_id() {
  id_ = 0;
  clear_has_id();
}
inline ::google::protobuf::int32 ScrapeData::id() const {
  // @@protoc_insertion_point(field_get:ScrapeData.id)
  return id_;
}
inline void ScrapeData::set_id(::google::protobuf::int32 value) {
  set_has_id();
  id_ = value;
  // @@protoc_insertion_point(field_set:ScrapeData.id)
}

// optional string quid = 3;
inline bool ScrapeData::has_quid() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ScrapeData::set_has_quid() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ScrapeData::clear_has_quid() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ScrapeData::clear_quid() {
  if (quid_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_->clear();
  }
  clear_has_quid();
}
inline const ::std::string& ScrapeData::quid() const {
  // @@protoc_insertion_point(field_get:ScrapeData.quid)
  return *quid_;
}
inline void ScrapeData::set_quid(const ::std::string& value) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(value);
  // @@protoc_insertion_point(field_set:ScrapeData.quid)
}
inline void ScrapeData::set_quid(const char* value) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(value);
  // @@protoc_insertion_point(field_set_char:ScrapeData.quid)
}
inline void ScrapeData::set_quid(const char* value, size_t size) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ScrapeData.quid)
}
inline ::std::string* ScrapeData::mutable_quid() {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ScrapeData.quid)
  return quid_;
}
inline ::std::string* ScrapeData::release_quid() {
  clear_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = quid_;
    quid_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ScrapeData::set_allocated_quid(::std::string* quid) {
  if (quid_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete quid_;
  }
  if (quid) {
    set_has_quid();
    quid_ = quid;
  } else {
    clear_has_quid();
    quid_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:ScrapeData.quid)
}

// optional string zone = 4;
inline bool ScrapeData::has_zone() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ScrapeData::set_has_zone() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ScrapeData::clear_has_zone() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ScrapeData::clear_zone() {
  if (zone_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    zone_->clear();
  }
  clear_has_zone();
}
inline const ::std::string& ScrapeData::zone() const {
  // @@protoc_insertion_point(field_get:ScrapeData.zone)
  return *zone_;
}
inline void ScrapeData::set_zone(const ::std::string& value) {
  set_has_zone();
  if (zone_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    zone_ = new ::std::string;
  }
  zone_->assign(value);
  // @@protoc_insertion_point(field_set:ScrapeData.zone)
}
inline void ScrapeData::set_zone(const char* value) {
  set_has_zone();
  if (zone_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    zone_ = new ::std::string;
  }
  zone_->assign(value);
  // @@protoc_insertion_point(field_set_char:ScrapeData.zone)
}
inline void ScrapeData::set_zone(const char* value, size_t size) {
  set_has_zone();
  if (zone_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    zone_ = new ::std::string;
  }
  zone_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ScrapeData.zone)
}
inline ::std::string* ScrapeData::mutable_zone() {
  set_has_zone();
  if (zone_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    zone_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ScrapeData.zone)
  return zone_;
}
inline ::std::string* ScrapeData::release_zone() {
  clear_has_zone();
  if (zone_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = zone_;
    zone_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ScrapeData::set_allocated_zone(::std::string* zone) {
  if (zone_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete zone_;
  }
  if (zone) {
    set_has_zone();
    zone_ = zone;
  } else {
    clear_has_zone();
    zone_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:ScrapeData.zone)
}

// required .ScrapeData.DataType type = 5 [default = PLAIN];
inline bool ScrapeData::has_type() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void ScrapeData::set_has_type() {
  _has_bits_[0] |= 0x00000010u;
}
inline void ScrapeData::clear_has_type() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void ScrapeData::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::ScrapeData_DataType ScrapeData::type() const {
  // @@protoc_insertion_point(field_get:ScrapeData.type)
  return static_cast< ::ScrapeData_DataType >(type_);
}
inline void ScrapeData::set_type(::ScrapeData_DataType value) {
  assert(::ScrapeData_DataType_IsValid(value));
  set_has_type();
  type_ = value;
  // @@protoc_insertion_point(field_set:ScrapeData.type)
}

// required .ScrapeData.Data content = 6;
inline bool ScrapeData::has_content() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void ScrapeData::set_has_content() {
  _has_bits_[0] |= 0x00000020u;
}
inline void ScrapeData::clear_has_content() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void ScrapeData::clear_content() {
  if (content_ != NULL) content_->::ScrapeData_Data::Clear();
  clear_has_content();
}
inline const ::ScrapeData_Data& ScrapeData::content() const {
  // @@protoc_insertion_point(field_get:ScrapeData.content)
  return content_ != NULL ? *content_ : *default_instance_->content_;
}
inline ::ScrapeData_Data* ScrapeData::mutable_content() {
  set_has_content();
  if (content_ == NULL) content_ = new ::ScrapeData_Data;
  // @@protoc_insertion_point(field_mutable:ScrapeData.content)
  return content_;
}
inline ::ScrapeData_Data* ScrapeData::release_content() {
  clear_has_content();
  ::ScrapeData_Data* temp = content_;
  content_ = NULL;
  return temp;
}
inline void ScrapeData::set_allocated_content(::ScrapeData_Data* content) {
  delete content_;
  content_ = content;
  if (content) {
    set_has_content();
  } else {
    clear_has_content();
  }
  // @@protoc_insertion_point(field_set_allocated:ScrapeData.content)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::ScrapeData_DataType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ScrapeData_DataType>() {
  return ::ScrapeData_DataType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_scrapedata_2eproto__INCLUDED
