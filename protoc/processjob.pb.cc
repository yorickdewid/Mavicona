// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: processjob.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "processjob.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* ProcessJob_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ProcessJob_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* ProcessJob_State_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_processjob_2eproto() {
  protobuf_AddDesc_processjob_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "processjob.proto");
  GOOGLE_CHECK(file != NULL);
  ProcessJob_descriptor_ = file->message_type(0);
  static const int ProcessJob_offsets_[9] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, quid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, state_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, content_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, data_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, partition_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, partition_count_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, quid_parent_),
  };
  ProcessJob_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ProcessJob_descriptor_,
      ProcessJob::default_instance_,
      ProcessJob_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessJob, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ProcessJob));
  ProcessJob_State_descriptor_ = ProcessJob_descriptor_->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_processjob_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ProcessJob_descriptor_, &ProcessJob::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_processjob_2eproto() {
  delete ProcessJob::default_instance_;
  delete ProcessJob_reflection_;
}

void protobuf_AddDesc_processjob_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\020processjob.proto\"\345\001\n\nProcessJob\022\014\n\004nam"
    "e\030\001 \002(\t\022\n\n\002id\030\002 \002(\005\022\014\n\004quid\030\003 \001(\t\022 \n\005sta"
    "te\030\004 \002(\0162\021.ProcessJob.State\022\017\n\007content\030\005"
    " \001(\014\022\014\n\004data\030\006 \001(\014\022\021\n\tpartition\030\007 \001(\005\022\027\n"
    "\017partition_count\030\010 \001(\005\022\023\n\013quid_parent\030\t "
    "\001(\t\"-\n\005State\022\t\n\005SPAWN\020\000\022\r\n\tPARTITION\020\001\022\n"
    "\n\006FUNNEL\020\002", 250);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "processjob.proto", &protobuf_RegisterTypes);
  ProcessJob::default_instance_ = new ProcessJob();
  ProcessJob::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_processjob_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_processjob_2eproto {
  StaticDescriptorInitializer_processjob_2eproto() {
    protobuf_AddDesc_processjob_2eproto();
  }
} static_descriptor_initializer_processjob_2eproto_;

// ===================================================================

const ::google::protobuf::EnumDescriptor* ProcessJob_State_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ProcessJob_State_descriptor_;
}
bool ProcessJob_State_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const ProcessJob_State ProcessJob::SPAWN;
const ProcessJob_State ProcessJob::PARTITION;
const ProcessJob_State ProcessJob::FUNNEL;
const ProcessJob_State ProcessJob::State_MIN;
const ProcessJob_State ProcessJob::State_MAX;
const int ProcessJob::State_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int ProcessJob::kNameFieldNumber;
const int ProcessJob::kIdFieldNumber;
const int ProcessJob::kQuidFieldNumber;
const int ProcessJob::kStateFieldNumber;
const int ProcessJob::kContentFieldNumber;
const int ProcessJob::kDataFieldNumber;
const int ProcessJob::kPartitionFieldNumber;
const int ProcessJob::kPartitionCountFieldNumber;
const int ProcessJob::kQuidParentFieldNumber;
#endif  // !_MSC_VER

ProcessJob::ProcessJob()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:ProcessJob)
}

void ProcessJob::InitAsDefaultInstance() {
}

ProcessJob::ProcessJob(const ProcessJob& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:ProcessJob)
}

void ProcessJob::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  id_ = 0;
  quid_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  state_ = 0;
  content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  data_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  partition_ = 0;
  partition_count_ = 0;
  quid_parent_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ProcessJob::~ProcessJob() {
  // @@protoc_insertion_point(destructor:ProcessJob)
  SharedDtor();
}

void ProcessJob::SharedDtor() {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete name_;
  }
  if (quid_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete quid_;
  }
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete content_;
  }
  if (data_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete data_;
  }
  if (quid_parent_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete quid_parent_;
  }
  if (this != default_instance_) {
  }
}

void ProcessJob::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ProcessJob::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ProcessJob_descriptor_;
}

const ProcessJob& ProcessJob::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_processjob_2eproto();
  return *default_instance_;
}

ProcessJob* ProcessJob::default_instance_ = NULL;

ProcessJob* ProcessJob::New() const {
  return new ProcessJob;
}

void ProcessJob::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<ProcessJob*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  if (_has_bits_[0 / 32] & 255) {
    ZR_(id_, state_);
    ZR_(partition_, partition_count_);
    if (has_name()) {
      if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        name_->clear();
      }
    }
    if (has_quid()) {
      if (quid_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        quid_->clear();
      }
    }
    if (has_content()) {
      if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        content_->clear();
      }
    }
    if (has_data()) {
      if (data_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        data_->clear();
      }
    }
  }
  if (has_quid_parent()) {
    if (quid_parent_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
      quid_parent_->clear();
    }
  }

#undef OFFSET_OF_FIELD_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ProcessJob::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:ProcessJob)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string name = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->name().data(), this->name().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "name");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_id;
        break;
      }

      // required int32 id = 2;
      case 2: {
        if (tag == 16) {
         parse_id:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &id_)));
          set_has_id();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(26)) goto parse_quid;
        break;
      }

      // optional string quid = 3;
      case 3: {
        if (tag == 26) {
         parse_quid:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_quid()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->quid().data(), this->quid().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "quid");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(32)) goto parse_state;
        break;
      }

      // required .ProcessJob.State state = 4;
      case 4: {
        if (tag == 32) {
         parse_state:
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::ProcessJob_State_IsValid(value)) {
            set_state(static_cast< ::ProcessJob_State >(value));
          } else {
            mutable_unknown_fields()->AddVarint(4, value);
          }
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(42)) goto parse_content;
        break;
      }

      // optional bytes content = 5;
      case 5: {
        if (tag == 42) {
         parse_content:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_content()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(50)) goto parse_data;
        break;
      }

      // optional bytes data = 6;
      case 6: {
        if (tag == 50) {
         parse_data:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_data()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(56)) goto parse_partition;
        break;
      }

      // optional int32 partition = 7;
      case 7: {
        if (tag == 56) {
         parse_partition:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &partition_)));
          set_has_partition();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(64)) goto parse_partition_count;
        break;
      }

      // optional int32 partition_count = 8;
      case 8: {
        if (tag == 64) {
         parse_partition_count:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &partition_count_)));
          set_has_partition_count();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(74)) goto parse_quid_parent;
        break;
      }

      // optional string quid_parent = 9;
      case 9: {
        if (tag == 74) {
         parse_quid_parent:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_quid_parent()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->quid_parent().data(), this->quid_parent().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "quid_parent");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:ProcessJob)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:ProcessJob)
  return false;
#undef DO_
}

void ProcessJob::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:ProcessJob)
  // required string name = 1;
  if (has_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->name().data(), this->name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "name");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->name(), output);
  }

  // required int32 id = 2;
  if (has_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->id(), output);
  }

  // optional string quid = 3;
  if (has_quid()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->quid().data(), this->quid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "quid");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->quid(), output);
  }

  // required .ProcessJob.State state = 4;
  if (has_state()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      4, this->state(), output);
  }

  // optional bytes content = 5;
  if (has_content()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      5, this->content(), output);
  }

  // optional bytes data = 6;
  if (has_data()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      6, this->data(), output);
  }

  // optional int32 partition = 7;
  if (has_partition()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(7, this->partition(), output);
  }

  // optional int32 partition_count = 8;
  if (has_partition_count()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(8, this->partition_count(), output);
  }

  // optional string quid_parent = 9;
  if (has_quid_parent()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->quid_parent().data(), this->quid_parent().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "quid_parent");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      9, this->quid_parent(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:ProcessJob)
}

::google::protobuf::uint8* ProcessJob::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:ProcessJob)
  // required string name = 1;
  if (has_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->name().data(), this->name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "name");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->name(), target);
  }

  // required int32 id = 2;
  if (has_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->id(), target);
  }

  // optional string quid = 3;
  if (has_quid()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->quid().data(), this->quid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "quid");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->quid(), target);
  }

  // required .ProcessJob.State state = 4;
  if (has_state()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      4, this->state(), target);
  }

  // optional bytes content = 5;
  if (has_content()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        5, this->content(), target);
  }

  // optional bytes data = 6;
  if (has_data()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        6, this->data(), target);
  }

  // optional int32 partition = 7;
  if (has_partition()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(7, this->partition(), target);
  }

  // optional int32 partition_count = 8;
  if (has_partition_count()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(8, this->partition_count(), target);
  }

  // optional string quid_parent = 9;
  if (has_quid_parent()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->quid_parent().data(), this->quid_parent().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "quid_parent");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        9, this->quid_parent(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:ProcessJob)
  return target;
}

int ProcessJob::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string name = 1;
    if (has_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->name());
    }

    // required int32 id = 2;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->id());
    }

    // optional string quid = 3;
    if (has_quid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->quid());
    }

    // required .ProcessJob.State state = 4;
    if (has_state()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->state());
    }

    // optional bytes content = 5;
    if (has_content()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->content());
    }

    // optional bytes data = 6;
    if (has_data()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->data());
    }

    // optional int32 partition = 7;
    if (has_partition()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->partition());
    }

    // optional int32 partition_count = 8;
    if (has_partition_count()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->partition_count());
    }

  }
  if (_has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    // optional string quid_parent = 9;
    if (has_quid_parent()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->quid_parent());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ProcessJob::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ProcessJob* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ProcessJob*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ProcessJob::MergeFrom(const ProcessJob& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_name()) {
      set_name(from.name());
    }
    if (from.has_id()) {
      set_id(from.id());
    }
    if (from.has_quid()) {
      set_quid(from.quid());
    }
    if (from.has_state()) {
      set_state(from.state());
    }
    if (from.has_content()) {
      set_content(from.content());
    }
    if (from.has_data()) {
      set_data(from.data());
    }
    if (from.has_partition()) {
      set_partition(from.partition());
    }
    if (from.has_partition_count()) {
      set_partition_count(from.partition_count());
    }
  }
  if (from._has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    if (from.has_quid_parent()) {
      set_quid_parent(from.quid_parent());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ProcessJob::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ProcessJob::CopyFrom(const ProcessJob& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ProcessJob::IsInitialized() const {
  if ((_has_bits_[0] & 0x0000000b) != 0x0000000b) return false;

  return true;
}

void ProcessJob::Swap(ProcessJob* other) {
  if (other != this) {
    std::swap(name_, other->name_);
    std::swap(id_, other->id_);
    std::swap(quid_, other->quid_);
    std::swap(state_, other->state_);
    std::swap(content_, other->content_);
    std::swap(data_, other->data_);
    std::swap(partition_, other->partition_);
    std::swap(partition_count_, other->partition_count_);
    std::swap(quid_parent_, other->quid_parent_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ProcessJob::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ProcessJob_descriptor_;
  metadata.reflection = ProcessJob_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
