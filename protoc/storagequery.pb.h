// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: storagequery.proto

#ifndef PROTOBUF_storagequery_2eproto__INCLUDED
#define PROTOBUF_storagequery_2eproto__INCLUDED

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
void  protobuf_AddDesc_storagequery_2eproto();
void protobuf_AssignDesc_storagequery_2eproto();
void protobuf_ShutdownFile_storagequery_2eproto();

class StorageQuery;
class StorageQuery_MetaEntry;

enum StorageQuery_Action {
  StorageQuery_Action_SELECT = 0,
  StorageQuery_Action_INSERT = 1,
  StorageQuery_Action_UPDATE = 2,
  StorageQuery_Action_DELETE = 3
};
bool StorageQuery_Action_IsValid(int value);
const StorageQuery_Action StorageQuery_Action_Action_MIN = StorageQuery_Action_SELECT;
const StorageQuery_Action StorageQuery_Action_Action_MAX = StorageQuery_Action_DELETE;
const int StorageQuery_Action_Action_ARRAYSIZE = StorageQuery_Action_Action_MAX + 1;

const ::google::protobuf::EnumDescriptor* StorageQuery_Action_descriptor();
inline const ::std::string& StorageQuery_Action_Name(StorageQuery_Action value) {
  return ::google::protobuf::internal::NameOfEnum(
    StorageQuery_Action_descriptor(), value);
}
inline bool StorageQuery_Action_Parse(
    const ::std::string& name, StorageQuery_Action* value) {
  return ::google::protobuf::internal::ParseNamedEnum<StorageQuery_Action>(
    StorageQuery_Action_descriptor(), name, value);
}
enum StorageQuery_Result {
  StorageQuery_Result_SUCCESS = 0,
  StorageQuery_Result_NOTFOUND = 1,
  StorageQuery_Result_DUPLICATE = 2
};
bool StorageQuery_Result_IsValid(int value);
const StorageQuery_Result StorageQuery_Result_Result_MIN = StorageQuery_Result_SUCCESS;
const StorageQuery_Result StorageQuery_Result_Result_MAX = StorageQuery_Result_DUPLICATE;
const int StorageQuery_Result_Result_ARRAYSIZE = StorageQuery_Result_Result_MAX + 1;

const ::google::protobuf::EnumDescriptor* StorageQuery_Result_descriptor();
inline const ::std::string& StorageQuery_Result_Name(StorageQuery_Result value) {
  return ::google::protobuf::internal::NameOfEnum(
    StorageQuery_Result_descriptor(), value);
}
inline bool StorageQuery_Result_Parse(
    const ::std::string& name, StorageQuery_Result* value) {
  return ::google::protobuf::internal::ParseNamedEnum<StorageQuery_Result>(
    StorageQuery_Result_descriptor(), name, value);
}
// ===================================================================

class StorageQuery_MetaEntry : public ::google::protobuf::Message {
 public:
  StorageQuery_MetaEntry();
  virtual ~StorageQuery_MetaEntry();

  StorageQuery_MetaEntry(const StorageQuery_MetaEntry& from);

  inline StorageQuery_MetaEntry& operator=(const StorageQuery_MetaEntry& from) {
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
  static const StorageQuery_MetaEntry& default_instance();

  void Swap(StorageQuery_MetaEntry* other);

  // implements Message ----------------------------------------------

  StorageQuery_MetaEntry* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const StorageQuery_MetaEntry& from);
  void MergeFrom(const StorageQuery_MetaEntry& from);
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

  // required string key = 1;
  inline bool has_key() const;
  inline void clear_key();
  static const int kKeyFieldNumber = 1;
  inline const ::std::string& key() const;
  inline void set_key(const ::std::string& value);
  inline void set_key(const char* value);
  inline void set_key(const char* value, size_t size);
  inline ::std::string* mutable_key();
  inline ::std::string* release_key();
  inline void set_allocated_key(::std::string* key);

  // optional bytes value = 2;
  inline bool has_value() const;
  inline void clear_value();
  static const int kValueFieldNumber = 2;
  inline const ::std::string& value() const;
  inline void set_value(const ::std::string& value);
  inline void set_value(const char* value);
  inline void set_value(const void* value, size_t size);
  inline ::std::string* mutable_value();
  inline ::std::string* release_value();
  inline void set_allocated_value(::std::string* value);

  // repeated .StorageQuery.MetaEntry meta = 3;
  inline int meta_size() const;
  inline void clear_meta();
  static const int kMetaFieldNumber = 3;
  inline const ::StorageQuery_MetaEntry& meta(int index) const;
  inline ::StorageQuery_MetaEntry* mutable_meta(int index);
  inline ::StorageQuery_MetaEntry* add_meta();
  inline const ::google::protobuf::RepeatedPtrField< ::StorageQuery_MetaEntry >&
      meta() const;
  inline ::google::protobuf::RepeatedPtrField< ::StorageQuery_MetaEntry >*
      mutable_meta();

  // @@protoc_insertion_point(class_scope:StorageQuery.MetaEntry)
 private:
  inline void set_has_key();
  inline void clear_has_key();
  inline void set_has_value();
  inline void clear_has_value();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* key_;
  ::std::string* value_;
  ::google::protobuf::RepeatedPtrField< ::StorageQuery_MetaEntry > meta_;
  friend void  protobuf_AddDesc_storagequery_2eproto();
  friend void protobuf_AssignDesc_storagequery_2eproto();
  friend void protobuf_ShutdownFile_storagequery_2eproto();

  void InitAsDefaultInstance();
  static StorageQuery_MetaEntry* default_instance_;
};
// -------------------------------------------------------------------

class StorageQuery : public ::google::protobuf::Message {
 public:
  StorageQuery();
  virtual ~StorageQuery();

  StorageQuery(const StorageQuery& from);

  inline StorageQuery& operator=(const StorageQuery& from) {
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
  static const StorageQuery& default_instance();

  void Swap(StorageQuery* other);

  // implements Message ----------------------------------------------

  StorageQuery* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const StorageQuery& from);
  void MergeFrom(const StorageQuery& from);
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

  typedef StorageQuery_MetaEntry MetaEntry;

  typedef StorageQuery_Action Action;
  static const Action SELECT = StorageQuery_Action_SELECT;
  static const Action INSERT = StorageQuery_Action_INSERT;
  static const Action UPDATE = StorageQuery_Action_UPDATE;
  static const Action DELETE = StorageQuery_Action_DELETE;
  static inline bool Action_IsValid(int value) {
    return StorageQuery_Action_IsValid(value);
  }
  static const Action Action_MIN =
    StorageQuery_Action_Action_MIN;
  static const Action Action_MAX =
    StorageQuery_Action_Action_MAX;
  static const int Action_ARRAYSIZE =
    StorageQuery_Action_Action_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Action_descriptor() {
    return StorageQuery_Action_descriptor();
  }
  static inline const ::std::string& Action_Name(Action value) {
    return StorageQuery_Action_Name(value);
  }
  static inline bool Action_Parse(const ::std::string& name,
      Action* value) {
    return StorageQuery_Action_Parse(name, value);
  }

  typedef StorageQuery_Result Result;
  static const Result SUCCESS = StorageQuery_Result_SUCCESS;
  static const Result NOTFOUND = StorageQuery_Result_NOTFOUND;
  static const Result DUPLICATE = StorageQuery_Result_DUPLICATE;
  static inline bool Result_IsValid(int value) {
    return StorageQuery_Result_IsValid(value);
  }
  static const Result Result_MIN =
    StorageQuery_Result_Result_MIN;
  static const Result Result_MAX =
    StorageQuery_Result_Result_MAX;
  static const int Result_ARRAYSIZE =
    StorageQuery_Result_Result_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Result_descriptor() {
    return StorageQuery_Result_descriptor();
  }
  static inline const ::std::string& Result_Name(Result value) {
    return StorageQuery_Result_Name(value);
  }
  static inline bool Result_Parse(const ::std::string& name,
      Result* value) {
    return StorageQuery_Result_Parse(name, value);
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

  // required string quid = 3;
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

  // optional bytes content = 4;
  inline bool has_content() const;
  inline void clear_content();
  static const int kContentFieldNumber = 4;
  inline const ::std::string& content() const;
  inline void set_content(const ::std::string& value);
  inline void set_content(const char* value);
  inline void set_content(const void* value, size_t size);
  inline ::std::string* mutable_content();
  inline ::std::string* release_content();
  inline void set_allocated_content(::std::string* content);

  // required .StorageQuery.Action queryaction = 5;
  inline bool has_queryaction() const;
  inline void clear_queryaction();
  static const int kQueryactionFieldNumber = 5;
  inline ::StorageQuery_Action queryaction() const;
  inline void set_queryaction(::StorageQuery_Action value);

  // required .StorageQuery.Result queryresult = 6;
  inline bool has_queryresult() const;
  inline void clear_queryresult();
  static const int kQueryresultFieldNumber = 6;
  inline ::StorageQuery_Result queryresult() const;
  inline void set_queryresult(::StorageQuery_Result value);

  // repeated .StorageQuery.MetaEntry meta = 7;
  inline int meta_size() const;
  inline void clear_meta();
  static const int kMetaFieldNumber = 7;
  inline const ::StorageQuery_MetaEntry& meta(int index) const;
  inline ::StorageQuery_MetaEntry* mutable_meta(int index);
  inline ::StorageQuery_MetaEntry* add_meta();
  inline const ::google::protobuf::RepeatedPtrField< ::StorageQuery_MetaEntry >&
      meta() const;
  inline ::google::protobuf::RepeatedPtrField< ::StorageQuery_MetaEntry >*
      mutable_meta();

  // @@protoc_insertion_point(class_scope:StorageQuery)
 private:
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_quid();
  inline void clear_has_quid();
  inline void set_has_content();
  inline void clear_has_content();
  inline void set_has_queryaction();
  inline void clear_has_queryaction();
  inline void set_has_queryresult();
  inline void clear_has_queryresult();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* name_;
  ::std::string* quid_;
  ::google::protobuf::int32 id_;
  int queryaction_;
  ::std::string* content_;
  ::google::protobuf::RepeatedPtrField< ::StorageQuery_MetaEntry > meta_;
  int queryresult_;
  friend void  protobuf_AddDesc_storagequery_2eproto();
  friend void protobuf_AssignDesc_storagequery_2eproto();
  friend void protobuf_ShutdownFile_storagequery_2eproto();

  void InitAsDefaultInstance();
  static StorageQuery* default_instance_;
};
// ===================================================================


// ===================================================================

// StorageQuery_MetaEntry

// required string key = 1;
inline bool StorageQuery_MetaEntry::has_key() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void StorageQuery_MetaEntry::set_has_key() {
  _has_bits_[0] |= 0x00000001u;
}
inline void StorageQuery_MetaEntry::clear_has_key() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void StorageQuery_MetaEntry::clear_key() {
  if (key_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    key_->clear();
  }
  clear_has_key();
}
inline const ::std::string& StorageQuery_MetaEntry::key() const {
  // @@protoc_insertion_point(field_get:StorageQuery.MetaEntry.key)
  return *key_;
}
inline void StorageQuery_MetaEntry::set_key(const ::std::string& value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    key_ = new ::std::string;
  }
  key_->assign(value);
  // @@protoc_insertion_point(field_set:StorageQuery.MetaEntry.key)
}
inline void StorageQuery_MetaEntry::set_key(const char* value) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    key_ = new ::std::string;
  }
  key_->assign(value);
  // @@protoc_insertion_point(field_set_char:StorageQuery.MetaEntry.key)
}
inline void StorageQuery_MetaEntry::set_key(const char* value, size_t size) {
  set_has_key();
  if (key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    key_ = new ::std::string;
  }
  key_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:StorageQuery.MetaEntry.key)
}
inline ::std::string* StorageQuery_MetaEntry::mutable_key() {
  set_has_key();
  if (key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    key_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:StorageQuery.MetaEntry.key)
  return key_;
}
inline ::std::string* StorageQuery_MetaEntry::release_key() {
  clear_has_key();
  if (key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = key_;
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void StorageQuery_MetaEntry::set_allocated_key(::std::string* key) {
  if (key_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete key_;
  }
  if (key) {
    set_has_key();
    key_ = key;
  } else {
    clear_has_key();
    key_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:StorageQuery.MetaEntry.key)
}

// optional bytes value = 2;
inline bool StorageQuery_MetaEntry::has_value() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void StorageQuery_MetaEntry::set_has_value() {
  _has_bits_[0] |= 0x00000002u;
}
inline void StorageQuery_MetaEntry::clear_has_value() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void StorageQuery_MetaEntry::clear_value() {
  if (value_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_->clear();
  }
  clear_has_value();
}
inline const ::std::string& StorageQuery_MetaEntry::value() const {
  // @@protoc_insertion_point(field_get:StorageQuery.MetaEntry.value)
  return *value_;
}
inline void StorageQuery_MetaEntry::set_value(const ::std::string& value) {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  value_->assign(value);
  // @@protoc_insertion_point(field_set:StorageQuery.MetaEntry.value)
}
inline void StorageQuery_MetaEntry::set_value(const char* value) {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  value_->assign(value);
  // @@protoc_insertion_point(field_set_char:StorageQuery.MetaEntry.value)
}
inline void StorageQuery_MetaEntry::set_value(const void* value, size_t size) {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  value_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:StorageQuery.MetaEntry.value)
}
inline ::std::string* StorageQuery_MetaEntry::mutable_value() {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:StorageQuery.MetaEntry.value)
  return value_;
}
inline ::std::string* StorageQuery_MetaEntry::release_value() {
  clear_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = value_;
    value_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void StorageQuery_MetaEntry::set_allocated_value(::std::string* value) {
  if (value_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete value_;
  }
  if (value) {
    set_has_value();
    value_ = value;
  } else {
    clear_has_value();
    value_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:StorageQuery.MetaEntry.value)
}

// repeated .StorageQuery.MetaEntry meta = 3;
inline int StorageQuery_MetaEntry::meta_size() const {
  return meta_.size();
}
inline void StorageQuery_MetaEntry::clear_meta() {
  meta_.Clear();
}
inline const ::StorageQuery_MetaEntry& StorageQuery_MetaEntry::meta(int index) const {
  // @@protoc_insertion_point(field_get:StorageQuery.MetaEntry.meta)
  return meta_.Get(index);
}
inline ::StorageQuery_MetaEntry* StorageQuery_MetaEntry::mutable_meta(int index) {
  // @@protoc_insertion_point(field_mutable:StorageQuery.MetaEntry.meta)
  return meta_.Mutable(index);
}
inline ::StorageQuery_MetaEntry* StorageQuery_MetaEntry::add_meta() {
  // @@protoc_insertion_point(field_add:StorageQuery.MetaEntry.meta)
  return meta_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::StorageQuery_MetaEntry >&
StorageQuery_MetaEntry::meta() const {
  // @@protoc_insertion_point(field_list:StorageQuery.MetaEntry.meta)
  return meta_;
}
inline ::google::protobuf::RepeatedPtrField< ::StorageQuery_MetaEntry >*
StorageQuery_MetaEntry::mutable_meta() {
  // @@protoc_insertion_point(field_mutable_list:StorageQuery.MetaEntry.meta)
  return &meta_;
}

// -------------------------------------------------------------------

// StorageQuery

// required string name = 1;
inline bool StorageQuery::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void StorageQuery::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void StorageQuery::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void StorageQuery::clear_name() {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& StorageQuery::name() const {
  // @@protoc_insertion_point(field_get:StorageQuery.name)
  return *name_;
}
inline void StorageQuery::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set:StorageQuery.name)
}
inline void StorageQuery::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set_char:StorageQuery.name)
}
inline void StorageQuery::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:StorageQuery.name)
}
inline ::std::string* StorageQuery::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:StorageQuery.name)
  return name_;
}
inline ::std::string* StorageQuery::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void StorageQuery::set_allocated_name(::std::string* name) {
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
  // @@protoc_insertion_point(field_set_allocated:StorageQuery.name)
}

// required int32 id = 2;
inline bool StorageQuery::has_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void StorageQuery::set_has_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void StorageQuery::clear_has_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void StorageQuery::clear_id() {
  id_ = 0;
  clear_has_id();
}
inline ::google::protobuf::int32 StorageQuery::id() const {
  // @@protoc_insertion_point(field_get:StorageQuery.id)
  return id_;
}
inline void StorageQuery::set_id(::google::protobuf::int32 value) {
  set_has_id();
  id_ = value;
  // @@protoc_insertion_point(field_set:StorageQuery.id)
}

// required string quid = 3;
inline bool StorageQuery::has_quid() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void StorageQuery::set_has_quid() {
  _has_bits_[0] |= 0x00000004u;
}
inline void StorageQuery::clear_has_quid() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void StorageQuery::clear_quid() {
  if (quid_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_->clear();
  }
  clear_has_quid();
}
inline const ::std::string& StorageQuery::quid() const {
  // @@protoc_insertion_point(field_get:StorageQuery.quid)
  return *quid_;
}
inline void StorageQuery::set_quid(const ::std::string& value) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(value);
  // @@protoc_insertion_point(field_set:StorageQuery.quid)
}
inline void StorageQuery::set_quid(const char* value) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(value);
  // @@protoc_insertion_point(field_set_char:StorageQuery.quid)
}
inline void StorageQuery::set_quid(const char* value, size_t size) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:StorageQuery.quid)
}
inline ::std::string* StorageQuery::mutable_quid() {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:StorageQuery.quid)
  return quid_;
}
inline ::std::string* StorageQuery::release_quid() {
  clear_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = quid_;
    quid_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void StorageQuery::set_allocated_quid(::std::string* quid) {
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
  // @@protoc_insertion_point(field_set_allocated:StorageQuery.quid)
}

// optional bytes content = 4;
inline bool StorageQuery::has_content() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void StorageQuery::set_has_content() {
  _has_bits_[0] |= 0x00000008u;
}
inline void StorageQuery::clear_has_content() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void StorageQuery::clear_content() {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_->clear();
  }
  clear_has_content();
}
inline const ::std::string& StorageQuery::content() const {
  // @@protoc_insertion_point(field_get:StorageQuery.content)
  return *content_;
}
inline void StorageQuery::set_content(const ::std::string& value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set:StorageQuery.content)
}
inline void StorageQuery::set_content(const char* value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set_char:StorageQuery.content)
}
inline void StorageQuery::set_content(const void* value, size_t size) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:StorageQuery.content)
}
inline ::std::string* StorageQuery::mutable_content() {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:StorageQuery.content)
  return content_;
}
inline ::std::string* StorageQuery::release_content() {
  clear_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = content_;
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void StorageQuery::set_allocated_content(::std::string* content) {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete content_;
  }
  if (content) {
    set_has_content();
    content_ = content;
  } else {
    clear_has_content();
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:StorageQuery.content)
}

// required .StorageQuery.Action queryaction = 5;
inline bool StorageQuery::has_queryaction() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void StorageQuery::set_has_queryaction() {
  _has_bits_[0] |= 0x00000010u;
}
inline void StorageQuery::clear_has_queryaction() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void StorageQuery::clear_queryaction() {
  queryaction_ = 0;
  clear_has_queryaction();
}
inline ::StorageQuery_Action StorageQuery::queryaction() const {
  // @@protoc_insertion_point(field_get:StorageQuery.queryaction)
  return static_cast< ::StorageQuery_Action >(queryaction_);
}
inline void StorageQuery::set_queryaction(::StorageQuery_Action value) {
  assert(::StorageQuery_Action_IsValid(value));
  set_has_queryaction();
  queryaction_ = value;
  // @@protoc_insertion_point(field_set:StorageQuery.queryaction)
}

// required .StorageQuery.Result queryresult = 6;
inline bool StorageQuery::has_queryresult() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void StorageQuery::set_has_queryresult() {
  _has_bits_[0] |= 0x00000020u;
}
inline void StorageQuery::clear_has_queryresult() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void StorageQuery::clear_queryresult() {
  queryresult_ = 0;
  clear_has_queryresult();
}
inline ::StorageQuery_Result StorageQuery::queryresult() const {
  // @@protoc_insertion_point(field_get:StorageQuery.queryresult)
  return static_cast< ::StorageQuery_Result >(queryresult_);
}
inline void StorageQuery::set_queryresult(::StorageQuery_Result value) {
  assert(::StorageQuery_Result_IsValid(value));
  set_has_queryresult();
  queryresult_ = value;
  // @@protoc_insertion_point(field_set:StorageQuery.queryresult)
}

// repeated .StorageQuery.MetaEntry meta = 7;
inline int StorageQuery::meta_size() const {
  return meta_.size();
}
inline void StorageQuery::clear_meta() {
  meta_.Clear();
}
inline const ::StorageQuery_MetaEntry& StorageQuery::meta(int index) const {
  // @@protoc_insertion_point(field_get:StorageQuery.meta)
  return meta_.Get(index);
}
inline ::StorageQuery_MetaEntry* StorageQuery::mutable_meta(int index) {
  // @@protoc_insertion_point(field_mutable:StorageQuery.meta)
  return meta_.Mutable(index);
}
inline ::StorageQuery_MetaEntry* StorageQuery::add_meta() {
  // @@protoc_insertion_point(field_add:StorageQuery.meta)
  return meta_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::StorageQuery_MetaEntry >&
StorageQuery::meta() const {
  // @@protoc_insertion_point(field_list:StorageQuery.meta)
  return meta_;
}
inline ::google::protobuf::RepeatedPtrField< ::StorageQuery_MetaEntry >*
StorageQuery::mutable_meta() {
  // @@protoc_insertion_point(field_mutable_list:StorageQuery.meta)
  return &meta_;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::StorageQuery_Action> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::StorageQuery_Action>() {
  return ::StorageQuery_Action_descriptor();
}
template <> struct is_proto_enum< ::StorageQuery_Result> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::StorageQuery_Result>() {
  return ::StorageQuery_Result_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_storagequery_2eproto__INCLUDED
