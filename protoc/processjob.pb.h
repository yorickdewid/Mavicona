// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: processjob.proto

#ifndef PROTOBUF_processjob_2eproto__INCLUDED
#define PROTOBUF_processjob_2eproto__INCLUDED

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
void  protobuf_AddDesc_processjob_2eproto();
void protobuf_AssignDesc_processjob_2eproto();
void protobuf_ShutdownFile_processjob_2eproto();

class ProcessJob;

enum ProcessJob_Action {
  ProcessJob_Action_SOLICIT = 0,
  ProcessJob_Action_IDLE = 1,
  ProcessJob_Action_ACCEPT = 2
};
bool ProcessJob_Action_IsValid(int value);
const ProcessJob_Action ProcessJob_Action_Action_MIN = ProcessJob_Action_SOLICIT;
const ProcessJob_Action ProcessJob_Action_Action_MAX = ProcessJob_Action_ACCEPT;
const int ProcessJob_Action_Action_ARRAYSIZE = ProcessJob_Action_Action_MAX + 1;

const ::google::protobuf::EnumDescriptor* ProcessJob_Action_descriptor();
inline const ::std::string& ProcessJob_Action_Name(ProcessJob_Action value) {
  return ::google::protobuf::internal::NameOfEnum(
    ProcessJob_Action_descriptor(), value);
}
inline bool ProcessJob_Action_Parse(
    const ::std::string& name, ProcessJob_Action* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ProcessJob_Action>(
    ProcessJob_Action_descriptor(), name, value);
}
enum ProcessJob_Result {
  ProcessJob_Result_SUCCESS = 0,
  ProcessJob_Result_FAIL = 1,
  ProcessJob_Result_HARDFAIL = 2
};
bool ProcessJob_Result_IsValid(int value);
const ProcessJob_Result ProcessJob_Result_Result_MIN = ProcessJob_Result_SUCCESS;
const ProcessJob_Result ProcessJob_Result_Result_MAX = ProcessJob_Result_HARDFAIL;
const int ProcessJob_Result_Result_ARRAYSIZE = ProcessJob_Result_Result_MAX + 1;

const ::google::protobuf::EnumDescriptor* ProcessJob_Result_descriptor();
inline const ::std::string& ProcessJob_Result_Name(ProcessJob_Result value) {
  return ::google::protobuf::internal::NameOfEnum(
    ProcessJob_Result_descriptor(), value);
}
inline bool ProcessJob_Result_Parse(
    const ::std::string& name, ProcessJob_Result* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ProcessJob_Result>(
    ProcessJob_Result_descriptor(), name, value);
}
// ===================================================================

class ProcessJob : public ::google::protobuf::Message {
 public:
  ProcessJob();
  virtual ~ProcessJob();

  ProcessJob(const ProcessJob& from);

  inline ProcessJob& operator=(const ProcessJob& from) {
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
  static const ProcessJob& default_instance();

  void Swap(ProcessJob* other);

  // implements Message ----------------------------------------------

  ProcessJob* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ProcessJob& from);
  void MergeFrom(const ProcessJob& from);
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

  typedef ProcessJob_Action Action;
  static const Action SOLICIT = ProcessJob_Action_SOLICIT;
  static const Action IDLE = ProcessJob_Action_IDLE;
  static const Action ACCEPT = ProcessJob_Action_ACCEPT;
  static inline bool Action_IsValid(int value) {
    return ProcessJob_Action_IsValid(value);
  }
  static const Action Action_MIN =
    ProcessJob_Action_Action_MIN;
  static const Action Action_MAX =
    ProcessJob_Action_Action_MAX;
  static const int Action_ARRAYSIZE =
    ProcessJob_Action_Action_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Action_descriptor() {
    return ProcessJob_Action_descriptor();
  }
  static inline const ::std::string& Action_Name(Action value) {
    return ProcessJob_Action_Name(value);
  }
  static inline bool Action_Parse(const ::std::string& name,
      Action* value) {
    return ProcessJob_Action_Parse(name, value);
  }

  typedef ProcessJob_Result Result;
  static const Result SUCCESS = ProcessJob_Result_SUCCESS;
  static const Result FAIL = ProcessJob_Result_FAIL;
  static const Result HARDFAIL = ProcessJob_Result_HARDFAIL;
  static inline bool Result_IsValid(int value) {
    return ProcessJob_Result_IsValid(value);
  }
  static const Result Result_MIN =
    ProcessJob_Result_Result_MIN;
  static const Result Result_MAX =
    ProcessJob_Result_Result_MAX;
  static const int Result_ARRAYSIZE =
    ProcessJob_Result_Result_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Result_descriptor() {
    return ProcessJob_Result_descriptor();
  }
  static inline const ::std::string& Result_Name(Result value) {
    return ProcessJob_Result_Name(value);
  }
  static inline bool Result_Parse(const ::std::string& name,
      Result* value) {
    return ProcessJob_Result_Parse(name, value);
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

  // required int32 partition = 5;
  inline bool has_partition() const;
  inline void clear_partition();
  static const int kPartitionFieldNumber = 5;
  inline ::google::protobuf::int32 partition() const;
  inline void set_partition(::google::protobuf::int32 value);

  // required .ProcessJob.Action jobaction = 6;
  inline bool has_jobaction() const;
  inline void clear_jobaction();
  static const int kJobactionFieldNumber = 6;
  inline ::ProcessJob_Action jobaction() const;
  inline void set_jobaction(::ProcessJob_Action value);

  // required .ProcessJob.Result jobresult = 7;
  inline bool has_jobresult() const;
  inline void clear_jobresult();
  static const int kJobresultFieldNumber = 7;
  inline ::ProcessJob_Result jobresult() const;
  inline void set_jobresult(::ProcessJob_Result value);

  // optional int32 runtime = 8;
  inline bool has_runtime() const;
  inline void clear_runtime();
  static const int kRuntimeFieldNumber = 8;
  inline ::google::protobuf::int32 runtime() const;
  inline void set_runtime(::google::protobuf::int32 value);

  // optional int32 returncode = 9 [default = 0];
  inline bool has_returncode() const;
  inline void clear_returncode();
  static const int kReturncodeFieldNumber = 9;
  inline ::google::protobuf::int32 returncode() const;
  inline void set_returncode(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:ProcessJob)
 private:
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_quid();
  inline void clear_has_quid();
  inline void set_has_content();
  inline void clear_has_content();
  inline void set_has_partition();
  inline void clear_has_partition();
  inline void set_has_jobaction();
  inline void clear_has_jobaction();
  inline void set_has_jobresult();
  inline void clear_has_jobresult();
  inline void set_has_runtime();
  inline void clear_has_runtime();
  inline void set_has_returncode();
  inline void clear_has_returncode();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* name_;
  ::std::string* quid_;
  ::google::protobuf::int32 id_;
  ::google::protobuf::int32 partition_;
  ::std::string* content_;
  int jobaction_;
  int jobresult_;
  ::google::protobuf::int32 runtime_;
  ::google::protobuf::int32 returncode_;
  friend void  protobuf_AddDesc_processjob_2eproto();
  friend void protobuf_AssignDesc_processjob_2eproto();
  friend void protobuf_ShutdownFile_processjob_2eproto();

  void InitAsDefaultInstance();
  static ProcessJob* default_instance_;
};
// ===================================================================


// ===================================================================

// ProcessJob

// required string name = 1;
inline bool ProcessJob::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ProcessJob::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ProcessJob::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ProcessJob::clear_name() {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& ProcessJob::name() const {
  // @@protoc_insertion_point(field_get:ProcessJob.name)
  return *name_;
}
inline void ProcessJob::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set:ProcessJob.name)
}
inline void ProcessJob::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set_char:ProcessJob.name)
}
inline void ProcessJob::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ProcessJob.name)
}
inline ::std::string* ProcessJob::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ProcessJob.name)
  return name_;
}
inline ::std::string* ProcessJob::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ProcessJob::set_allocated_name(::std::string* name) {
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
  // @@protoc_insertion_point(field_set_allocated:ProcessJob.name)
}

// required int32 id = 2;
inline bool ProcessJob::has_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ProcessJob::set_has_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ProcessJob::clear_has_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ProcessJob::clear_id() {
  id_ = 0;
  clear_has_id();
}
inline ::google::protobuf::int32 ProcessJob::id() const {
  // @@protoc_insertion_point(field_get:ProcessJob.id)
  return id_;
}
inline void ProcessJob::set_id(::google::protobuf::int32 value) {
  set_has_id();
  id_ = value;
  // @@protoc_insertion_point(field_set:ProcessJob.id)
}

// optional string quid = 3;
inline bool ProcessJob::has_quid() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ProcessJob::set_has_quid() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ProcessJob::clear_has_quid() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ProcessJob::clear_quid() {
  if (quid_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_->clear();
  }
  clear_has_quid();
}
inline const ::std::string& ProcessJob::quid() const {
  // @@protoc_insertion_point(field_get:ProcessJob.quid)
  return *quid_;
}
inline void ProcessJob::set_quid(const ::std::string& value) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(value);
  // @@protoc_insertion_point(field_set:ProcessJob.quid)
}
inline void ProcessJob::set_quid(const char* value) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(value);
  // @@protoc_insertion_point(field_set_char:ProcessJob.quid)
}
inline void ProcessJob::set_quid(const char* value, size_t size) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ProcessJob.quid)
}
inline ::std::string* ProcessJob::mutable_quid() {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ProcessJob.quid)
  return quid_;
}
inline ::std::string* ProcessJob::release_quid() {
  clear_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = quid_;
    quid_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ProcessJob::set_allocated_quid(::std::string* quid) {
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
  // @@protoc_insertion_point(field_set_allocated:ProcessJob.quid)
}

// optional bytes content = 4;
inline bool ProcessJob::has_content() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ProcessJob::set_has_content() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ProcessJob::clear_has_content() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ProcessJob::clear_content() {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_->clear();
  }
  clear_has_content();
}
inline const ::std::string& ProcessJob::content() const {
  // @@protoc_insertion_point(field_get:ProcessJob.content)
  return *content_;
}
inline void ProcessJob::set_content(const ::std::string& value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set:ProcessJob.content)
}
inline void ProcessJob::set_content(const char* value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set_char:ProcessJob.content)
}
inline void ProcessJob::set_content(const void* value, size_t size) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ProcessJob.content)
}
inline ::std::string* ProcessJob::mutable_content() {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ProcessJob.content)
  return content_;
}
inline ::std::string* ProcessJob::release_content() {
  clear_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = content_;
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ProcessJob::set_allocated_content(::std::string* content) {
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
  // @@protoc_insertion_point(field_set_allocated:ProcessJob.content)
}

// required int32 partition = 5;
inline bool ProcessJob::has_partition() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void ProcessJob::set_has_partition() {
  _has_bits_[0] |= 0x00000010u;
}
inline void ProcessJob::clear_has_partition() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void ProcessJob::clear_partition() {
  partition_ = 0;
  clear_has_partition();
}
inline ::google::protobuf::int32 ProcessJob::partition() const {
  // @@protoc_insertion_point(field_get:ProcessJob.partition)
  return partition_;
}
inline void ProcessJob::set_partition(::google::protobuf::int32 value) {
  set_has_partition();
  partition_ = value;
  // @@protoc_insertion_point(field_set:ProcessJob.partition)
}

// required .ProcessJob.Action jobaction = 6;
inline bool ProcessJob::has_jobaction() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void ProcessJob::set_has_jobaction() {
  _has_bits_[0] |= 0x00000020u;
}
inline void ProcessJob::clear_has_jobaction() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void ProcessJob::clear_jobaction() {
  jobaction_ = 0;
  clear_has_jobaction();
}
inline ::ProcessJob_Action ProcessJob::jobaction() const {
  // @@protoc_insertion_point(field_get:ProcessJob.jobaction)
  return static_cast< ::ProcessJob_Action >(jobaction_);
}
inline void ProcessJob::set_jobaction(::ProcessJob_Action value) {
  assert(::ProcessJob_Action_IsValid(value));
  set_has_jobaction();
  jobaction_ = value;
  // @@protoc_insertion_point(field_set:ProcessJob.jobaction)
}

// required .ProcessJob.Result jobresult = 7;
inline bool ProcessJob::has_jobresult() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void ProcessJob::set_has_jobresult() {
  _has_bits_[0] |= 0x00000040u;
}
inline void ProcessJob::clear_has_jobresult() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void ProcessJob::clear_jobresult() {
  jobresult_ = 0;
  clear_has_jobresult();
}
inline ::ProcessJob_Result ProcessJob::jobresult() const {
  // @@protoc_insertion_point(field_get:ProcessJob.jobresult)
  return static_cast< ::ProcessJob_Result >(jobresult_);
}
inline void ProcessJob::set_jobresult(::ProcessJob_Result value) {
  assert(::ProcessJob_Result_IsValid(value));
  set_has_jobresult();
  jobresult_ = value;
  // @@protoc_insertion_point(field_set:ProcessJob.jobresult)
}

// optional int32 runtime = 8;
inline bool ProcessJob::has_runtime() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void ProcessJob::set_has_runtime() {
  _has_bits_[0] |= 0x00000080u;
}
inline void ProcessJob::clear_has_runtime() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void ProcessJob::clear_runtime() {
  runtime_ = 0;
  clear_has_runtime();
}
inline ::google::protobuf::int32 ProcessJob::runtime() const {
  // @@protoc_insertion_point(field_get:ProcessJob.runtime)
  return runtime_;
}
inline void ProcessJob::set_runtime(::google::protobuf::int32 value) {
  set_has_runtime();
  runtime_ = value;
  // @@protoc_insertion_point(field_set:ProcessJob.runtime)
}

// optional int32 returncode = 9 [default = 0];
inline bool ProcessJob::has_returncode() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void ProcessJob::set_has_returncode() {
  _has_bits_[0] |= 0x00000100u;
}
inline void ProcessJob::clear_has_returncode() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void ProcessJob::clear_returncode() {
  returncode_ = 0;
  clear_has_returncode();
}
inline ::google::protobuf::int32 ProcessJob::returncode() const {
  // @@protoc_insertion_point(field_get:ProcessJob.returncode)
  return returncode_;
}
inline void ProcessJob::set_returncode(::google::protobuf::int32 value) {
  set_has_returncode();
  returncode_ = value;
  // @@protoc_insertion_point(field_set:ProcessJob.returncode)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::ProcessJob_Action> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ProcessJob_Action>() {
  return ::ProcessJob_Action_descriptor();
}
template <> struct is_proto_enum< ::ProcessJob_Result> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ProcessJob_Result>() {
  return ::ProcessJob_Result_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_processjob_2eproto__INCLUDED