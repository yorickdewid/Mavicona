// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: task.proto

#ifndef PROTOBUF_task_2eproto__INCLUDED
#define PROTOBUF_task_2eproto__INCLUDED

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
void  protobuf_AddDesc_task_2eproto();
void protobuf_AssignDesc_task_2eproto();
void protobuf_ShutdownFile_task_2eproto();

class Task;

enum Task_QueuePriority {
  Task_QueuePriority_REALTIME = 0,
  Task_QueuePriority_HIGH = 1,
  Task_QueuePriority_NORMAL = 2,
  Task_QueuePriority_LOW = 3,
  Task_QueuePriority_IDLE = 4
};
bool Task_QueuePriority_IsValid(int value);
const Task_QueuePriority Task_QueuePriority_QueuePriority_MIN = Task_QueuePriority_REALTIME;
const Task_QueuePriority Task_QueuePriority_QueuePriority_MAX = Task_QueuePriority_IDLE;
const int Task_QueuePriority_QueuePriority_ARRAYSIZE = Task_QueuePriority_QueuePriority_MAX + 1;

const ::google::protobuf::EnumDescriptor* Task_QueuePriority_descriptor();
inline const ::std::string& Task_QueuePriority_Name(Task_QueuePriority value) {
  return ::google::protobuf::internal::NameOfEnum(
    Task_QueuePriority_descriptor(), value);
}
inline bool Task_QueuePriority_Parse(
    const ::std::string& name, Task_QueuePriority* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Task_QueuePriority>(
    Task_QueuePriority_descriptor(), name, value);
}
// ===================================================================

class Task : public ::google::protobuf::Message {
 public:
  Task();
  virtual ~Task();

  Task(const Task& from);

  inline Task& operator=(const Task& from) {
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
  static const Task& default_instance();

  void Swap(Task* other);

  // implements Message ----------------------------------------------

  Task* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Task& from);
  void MergeFrom(const Task& from);
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

  typedef Task_QueuePriority QueuePriority;
  static const QueuePriority REALTIME = Task_QueuePriority_REALTIME;
  static const QueuePriority HIGH = Task_QueuePriority_HIGH;
  static const QueuePriority NORMAL = Task_QueuePriority_NORMAL;
  static const QueuePriority LOW = Task_QueuePriority_LOW;
  static const QueuePriority IDLE = Task_QueuePriority_IDLE;
  static inline bool QueuePriority_IsValid(int value) {
    return Task_QueuePriority_IsValid(value);
  }
  static const QueuePriority QueuePriority_MIN =
    Task_QueuePriority_QueuePriority_MIN;
  static const QueuePriority QueuePriority_MAX =
    Task_QueuePriority_QueuePriority_MAX;
  static const int QueuePriority_ARRAYSIZE =
    Task_QueuePriority_QueuePriority_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  QueuePriority_descriptor() {
    return Task_QueuePriority_descriptor();
  }
  static inline const ::std::string& QueuePriority_Name(QueuePriority value) {
    return Task_QueuePriority_Name(value);
  }
  static inline bool QueuePriority_Parse(const ::std::string& name,
      QueuePriority* value) {
    return Task_QueuePriority_Parse(name, value);
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

  // required .Task.QueuePriority priority = 4 [default = NORMAL];
  inline bool has_priority() const;
  inline void clear_priority();
  static const int kPriorityFieldNumber = 4;
  inline ::Task_QueuePriority priority() const;
  inline void set_priority(::Task_QueuePriority value);

  // optional bytes content = 5;
  inline bool has_content() const;
  inline void clear_content();
  static const int kContentFieldNumber = 5;
  inline const ::std::string& content() const;
  inline void set_content(const ::std::string& value);
  inline void set_content(const char* value);
  inline void set_content(const void* value, size_t size);
  inline ::std::string* mutable_content();
  inline ::std::string* release_content();
  inline void set_allocated_content(::std::string* content);

  // @@protoc_insertion_point(class_scope:Task)
 private:
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_quid();
  inline void clear_has_quid();
  inline void set_has_priority();
  inline void clear_has_priority();
  inline void set_has_content();
  inline void clear_has_content();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* name_;
  ::std::string* quid_;
  ::google::protobuf::int32 id_;
  int priority_;
  ::std::string* content_;
  friend void  protobuf_AddDesc_task_2eproto();
  friend void protobuf_AssignDesc_task_2eproto();
  friend void protobuf_ShutdownFile_task_2eproto();

  void InitAsDefaultInstance();
  static Task* default_instance_;
};
// ===================================================================


// ===================================================================

// Task

// required string name = 1;
inline bool Task::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Task::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Task::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Task::clear_name() {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& Task::name() const {
  // @@protoc_insertion_point(field_get:Task.name)
  return *name_;
}
inline void Task::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set:Task.name)
}
inline void Task::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set_char:Task.name)
}
inline void Task::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Task.name)
}
inline ::std::string* Task::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Task.name)
  return name_;
}
inline ::std::string* Task::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Task::set_allocated_name(::std::string* name) {
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
  // @@protoc_insertion_point(field_set_allocated:Task.name)
}

// required int32 id = 2;
inline bool Task::has_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Task::set_has_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Task::clear_has_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Task::clear_id() {
  id_ = 0;
  clear_has_id();
}
inline ::google::protobuf::int32 Task::id() const {
  // @@protoc_insertion_point(field_get:Task.id)
  return id_;
}
inline void Task::set_id(::google::protobuf::int32 value) {
  set_has_id();
  id_ = value;
  // @@protoc_insertion_point(field_set:Task.id)
}

// optional string quid = 3;
inline bool Task::has_quid() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Task::set_has_quid() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Task::clear_has_quid() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Task::clear_quid() {
  if (quid_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_->clear();
  }
  clear_has_quid();
}
inline const ::std::string& Task::quid() const {
  // @@protoc_insertion_point(field_get:Task.quid)
  return *quid_;
}
inline void Task::set_quid(const ::std::string& value) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(value);
  // @@protoc_insertion_point(field_set:Task.quid)
}
inline void Task::set_quid(const char* value) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(value);
  // @@protoc_insertion_point(field_set_char:Task.quid)
}
inline void Task::set_quid(const char* value, size_t size) {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  quid_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Task.quid)
}
inline ::std::string* Task::mutable_quid() {
  set_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    quid_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Task.quid)
  return quid_;
}
inline ::std::string* Task::release_quid() {
  clear_has_quid();
  if (quid_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = quid_;
    quid_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Task::set_allocated_quid(::std::string* quid) {
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
  // @@protoc_insertion_point(field_set_allocated:Task.quid)
}

// required .Task.QueuePriority priority = 4 [default = NORMAL];
inline bool Task::has_priority() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Task::set_has_priority() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Task::clear_has_priority() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Task::clear_priority() {
  priority_ = 2;
  clear_has_priority();
}
inline ::Task_QueuePriority Task::priority() const {
  // @@protoc_insertion_point(field_get:Task.priority)
  return static_cast< ::Task_QueuePriority >(priority_);
}
inline void Task::set_priority(::Task_QueuePriority value) {
  assert(::Task_QueuePriority_IsValid(value));
  set_has_priority();
  priority_ = value;
  // @@protoc_insertion_point(field_set:Task.priority)
}

// optional bytes content = 5;
inline bool Task::has_content() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void Task::set_has_content() {
  _has_bits_[0] |= 0x00000010u;
}
inline void Task::clear_has_content() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void Task::clear_content() {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_->clear();
  }
  clear_has_content();
}
inline const ::std::string& Task::content() const {
  // @@protoc_insertion_point(field_get:Task.content)
  return *content_;
}
inline void Task::set_content(const ::std::string& value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set:Task.content)
}
inline void Task::set_content(const char* value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set_char:Task.content)
}
inline void Task::set_content(const void* value, size_t size) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Task.content)
}
inline ::std::string* Task::mutable_content() {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Task.content)
  return content_;
}
inline ::std::string* Task::release_content() {
  clear_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = content_;
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Task::set_allocated_content(::std::string* content) {
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
  // @@protoc_insertion_point(field_set_allocated:Task.content)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::Task_QueuePriority> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Task_QueuePriority>() {
  return ::Task_QueuePriority_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_task_2eproto__INCLUDED
