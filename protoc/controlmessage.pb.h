// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: controlmessage.proto

#ifndef PROTOBUF_controlmessage_2eproto__INCLUDED
#define PROTOBUF_controlmessage_2eproto__INCLUDED

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
void  protobuf_AddDesc_controlmessage_2eproto();
void protobuf_AssignDesc_controlmessage_2eproto();
void protobuf_ShutdownFile_controlmessage_2eproto();

class ControlMessage;

enum ControlMessage_Action {
  ControlMessage_Action_SOLICIT = 0,
  ControlMessage_Action_IDLE = 1,
  ControlMessage_Action_ACCEPTED = 2,
  ControlMessage_Action_SETUP = 3,
  ControlMessage_Action_RUNNING = 4,
  ControlMessage_Action_TEARDOWN = 5,
  ControlMessage_Action_CONFIRMED = 99
};
bool ControlMessage_Action_IsValid(int value);
const ControlMessage_Action ControlMessage_Action_Action_MIN = ControlMessage_Action_SOLICIT;
const ControlMessage_Action ControlMessage_Action_Action_MAX = ControlMessage_Action_CONFIRMED;
const int ControlMessage_Action_Action_ARRAYSIZE = ControlMessage_Action_Action_MAX + 1;

const ::google::protobuf::EnumDescriptor* ControlMessage_Action_descriptor();
inline const ::std::string& ControlMessage_Action_Name(ControlMessage_Action value) {
  return ::google::protobuf::internal::NameOfEnum(
    ControlMessage_Action_descriptor(), value);
}
inline bool ControlMessage_Action_Parse(
    const ::std::string& name, ControlMessage_Action* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ControlMessage_Action>(
    ControlMessage_Action_descriptor(), name, value);
}
// ===================================================================

class ControlMessage : public ::google::protobuf::Message {
 public:
  ControlMessage();
  virtual ~ControlMessage();

  ControlMessage(const ControlMessage& from);

  inline ControlMessage& operator=(const ControlMessage& from) {
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
  static const ControlMessage& default_instance();

  void Swap(ControlMessage* other);

  // implements Message ----------------------------------------------

  ControlMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ControlMessage& from);
  void MergeFrom(const ControlMessage& from);
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

  typedef ControlMessage_Action Action;
  static const Action SOLICIT = ControlMessage_Action_SOLICIT;
  static const Action IDLE = ControlMessage_Action_IDLE;
  static const Action ACCEPTED = ControlMessage_Action_ACCEPTED;
  static const Action SETUP = ControlMessage_Action_SETUP;
  static const Action RUNNING = ControlMessage_Action_RUNNING;
  static const Action TEARDOWN = ControlMessage_Action_TEARDOWN;
  static const Action CONFIRMED = ControlMessage_Action_CONFIRMED;
  static inline bool Action_IsValid(int value) {
    return ControlMessage_Action_IsValid(value);
  }
  static const Action Action_MIN =
    ControlMessage_Action_Action_MIN;
  static const Action Action_MAX =
    ControlMessage_Action_Action_MAX;
  static const int Action_ARRAYSIZE =
    ControlMessage_Action_Action_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Action_descriptor() {
    return ControlMessage_Action_descriptor();
  }
  static inline const ::std::string& Action_Name(Action value) {
    return ControlMessage_Action_Name(value);
  }
  static inline bool Action_Parse(const ::std::string& name,
      Action* value) {
    return ControlMessage_Action_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // required int32 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int32 id() const;
  inline void set_id(::google::protobuf::int32 value);

  // required string quid = 2;
  inline bool has_quid() const;
  inline void clear_quid();
  static const int kQuidFieldNumber = 2;
  inline const ::std::string& quid() const;
  inline void set_quid(const ::std::string& value);
  inline void set_quid(const char* value);
  inline void set_quid(const char* value, size_t size);
  inline ::std::string* mutable_quid();
  inline ::std::string* release_quid();
  inline void set_allocated_quid(::std::string* quid);

  // required .ControlMessage.Action action = 3;
  inline bool has_action() const;
  inline void clear_action();
  static const int kActionFieldNumber = 3;
  inline ::ControlMessage_Action action() const;
  inline void set_action(::ControlMessage_Action value);

  // optional int32 progress = 4;
  inline bool has_progress() const;
  inline void clear_progress();
  static const int kProgressFieldNumber = 4;
  inline ::google::protobuf::int32 progress() const;
  inline void set_progress(::google::protobuf::int32 value);

  // optional int32 cluster_jobs = 5;
  inline bool has_cluster_jobs() const;
  inline void clear_cluster_jobs();
  static const int kClusterJobsFieldNumber = 5;
  inline ::google::protobuf::int32 cluster_jobs() const;
  inline void set_cluster_jobs(::google::protobuf::int32 value);

  // repeated string cluster_workers = 6;
  inline int cluster_workers_size() const;
  inline void clear_cluster_workers();
  static const int kClusterWorkersFieldNumber = 6;
  inline const ::std::string& cluster_workers(int index) const;
  inline ::std::string* mutable_cluster_workers(int index);
  inline void set_cluster_workers(int index, const ::std::string& value);
  inline void set_cluster_workers(int index, const char* value);
  inline void set_cluster_workers(int index, const char* value, size_t size);
  inline ::std::string* add_cluster_workers();
  inline void add_cluster_workers(const ::std::string& value);
  inline void add_cluster_workers(const char* value);
  inline void add_cluster_workers(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& cluster_workers() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_cluster_workers();

  // @@protoc_insertion_point(class_scope:ControlMessage)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_quid();
  inline void clear_has_quid();
  inline void set_has_action();
  inline void clear_has_action();
  inline void set_has_progress();
  inline void clear_has_progress();
  inline void set_has_cluster_jobs();
  inline void clear_has_cluster_jobs();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* quid_;
  ::google::protobuf::int32 id_;
  int action_;
  ::google::protobuf::int32 progress_;
  ::google::protobuf::int32 cluster_jobs_;
  ::google::protobuf::RepeatedPtrField< ::std::string> cluster_workers_;
  friend void  protobuf_AddDesc_controlmessage_2eproto();
  friend void protobuf_AssignDesc_controlmessage_2eproto();
  friend void protobuf_ShutdownFile_controlmessage_2eproto();

  void InitAsDefaultInstance();
  static ControlMessage* default_instance_;
};
// ===================================================================


// ===================================================================

// ControlMessage

// required int32 id = 1;
inline bool ControlMessage::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ControlMessage::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ControlMessage::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ControlMessage::clear_id() {
  id_ = 0;
  clear_has_id();
}
inline ::google::protobuf::int32 ControlMessage::id() const {
  // @@protoc_insertion_point(field_get:ControlMessage.id)
  return id_;
}
inline void ControlMessage::set_id(::google::protobuf::int32 value) {
  set_has_id();
  id_ = value;
  // @@protoc_insertion_point(field_set:ControlMessage.id)
}

// required string quid = 2;
inline bool ControlMessage::has_quid() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ControlMessage::set_has_quid() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ControlMessage::clear_has_quid() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ControlMessage::clear_quid() {
  if (quid_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_->clear();
  }
  clear_has_quid();
}
inline const ::std::string& ControlMessage::quid() const {
  // @@protoc_insertion_point(field_get:ControlMessage.quid)
  return *quid_;
}
inline void ControlMessage::set_quid(const ::std::string& value) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(value);
  // @@protoc_insertion_point(field_set:ControlMessage.quid)
}
inline void ControlMessage::set_quid(const char* value) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(value);
  // @@protoc_insertion_point(field_set_char:ControlMessage.quid)
}
inline void ControlMessage::set_quid(const char* value, size_t size) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ControlMessage.quid)
}
inline ::std::string* ControlMessage::mutable_quid() {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ControlMessage.quid)
  return quid_;
}
inline ::std::string* ControlMessage::release_quid() {
  clear_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = quid_;
    quid_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ControlMessage::set_allocated_quid(::std::string* quid) {
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
  // @@protoc_insertion_point(field_set_allocated:ControlMessage.quid)
}

// required .ControlMessage.Action action = 3;
inline bool ControlMessage::has_action() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ControlMessage::set_has_action() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ControlMessage::clear_has_action() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ControlMessage::clear_action() {
  action_ = 0;
  clear_has_action();
}
inline ::ControlMessage_Action ControlMessage::action() const {
  // @@protoc_insertion_point(field_get:ControlMessage.action)
  return static_cast< ::ControlMessage_Action >(action_);
}
inline void ControlMessage::set_action(::ControlMessage_Action value) {
  assert(::ControlMessage_Action_IsValid(value));
  set_has_action();
  action_ = value;
  // @@protoc_insertion_point(field_set:ControlMessage.action)
}

// optional int32 progress = 4;
inline bool ControlMessage::has_progress() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ControlMessage::set_has_progress() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ControlMessage::clear_has_progress() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ControlMessage::clear_progress() {
  progress_ = 0;
  clear_has_progress();
}
inline ::google::protobuf::int32 ControlMessage::progress() const {
  // @@protoc_insertion_point(field_get:ControlMessage.progress)
  return progress_;
}
inline void ControlMessage::set_progress(::google::protobuf::int32 value) {
  set_has_progress();
  progress_ = value;
  // @@protoc_insertion_point(field_set:ControlMessage.progress)
}

// optional int32 cluster_jobs = 5;
inline bool ControlMessage::has_cluster_jobs() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void ControlMessage::set_has_cluster_jobs() {
  _has_bits_[0] |= 0x00000010u;
}
inline void ControlMessage::clear_has_cluster_jobs() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void ControlMessage::clear_cluster_jobs() {
  cluster_jobs_ = 0;
  clear_has_cluster_jobs();
}
inline ::google::protobuf::int32 ControlMessage::cluster_jobs() const {
  // @@protoc_insertion_point(field_get:ControlMessage.cluster_jobs)
  return cluster_jobs_;
}
inline void ControlMessage::set_cluster_jobs(::google::protobuf::int32 value) {
  set_has_cluster_jobs();
  cluster_jobs_ = value;
  // @@protoc_insertion_point(field_set:ControlMessage.cluster_jobs)
}

// repeated string cluster_workers = 6;
inline int ControlMessage::cluster_workers_size() const {
  return cluster_workers_.size();
}
inline void ControlMessage::clear_cluster_workers() {
  cluster_workers_.Clear();
}
inline const ::std::string& ControlMessage::cluster_workers(int index) const {
  // @@protoc_insertion_point(field_get:ControlMessage.cluster_workers)
  return cluster_workers_.Get(index);
}
inline ::std::string* ControlMessage::mutable_cluster_workers(int index) {
  // @@protoc_insertion_point(field_mutable:ControlMessage.cluster_workers)
  return cluster_workers_.Mutable(index);
}
inline void ControlMessage::set_cluster_workers(int index, const ::std::string& value) {
  // @@protoc_insertion_point(field_set:ControlMessage.cluster_workers)
  cluster_workers_.Mutable(index)->assign(value);
}
inline void ControlMessage::set_cluster_workers(int index, const char* value) {
  cluster_workers_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:ControlMessage.cluster_workers)
}
inline void ControlMessage::set_cluster_workers(int index, const char* value, size_t size) {
  cluster_workers_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ControlMessage.cluster_workers)
}
inline ::std::string* ControlMessage::add_cluster_workers() {
  return cluster_workers_.Add();
}
inline void ControlMessage::add_cluster_workers(const ::std::string& value) {
  cluster_workers_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:ControlMessage.cluster_workers)
}
inline void ControlMessage::add_cluster_workers(const char* value) {
  cluster_workers_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:ControlMessage.cluster_workers)
}
inline void ControlMessage::add_cluster_workers(const char* value, size_t size) {
  cluster_workers_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:ControlMessage.cluster_workers)
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
ControlMessage::cluster_workers() const {
  // @@protoc_insertion_point(field_list:ControlMessage.cluster_workers)
  return cluster_workers_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
ControlMessage::mutable_cluster_workers() {
  // @@protoc_insertion_point(field_mutable_list:ControlMessage.cluster_workers)
  return &cluster_workers_;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::ControlMessage_Action> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ControlMessage_Action>() {
  return ::ControlMessage_Action_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_controlmessage_2eproto__INCLUDED
