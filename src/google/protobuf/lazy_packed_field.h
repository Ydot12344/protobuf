#pragma once

#include "google/protobuf/message.h"
#include <string>
#include <google/protobuf/arena.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/parse_context.h>

#include <optional>

template<class T>
class TLazyField {

enum class EBinaryDataType {
    STRING = 0,
    LIST_BUFFERS = 1
};

friend ::google::protobuf::MessageLite;
friend ::google::protobuf::Message;

public:
    T* Unpack() const {
        if (!IsUnpacked_) {
            if (BinaryDataType_ == EBinaryDataType::STRING) {
                Value_->ParseFromString(*BinaryData_);
            } else {
                // TODO: ???
                // create stream from list<RefCountBuffers> + add ParseFromListBuffers() to avoid copying
                // optional extra task !!!
                std::string tmp;
                for (const auto& buff : BinaryDataList_) {
                    tmp += std::string(
                        buff.buffer.data.get() + buff.start_offset,
                        buff.buffer.data.get() + buff.buffer.size - buff.end_offset
                    );
                }
                Value_->ParseFromString(tmp);
            }
            IsUnpacked_ = true;
        }

        return Value_;
    }

    bool IsInitialized() const {
        return true;
    }

    void Clear() {
        IsUnpacked_ = false;
        if (Value_) Value_->Clear();
        BinaryData_->clear();
        BinaryDataList_.clear();
        BinaryDataType_ = EBinaryDataType::STRING;
        BinarySize_.reset();
    }

    size_t ByteSizeLong() const {
        if (!IsUnpacked_) {
            return GetBinarySize();
        }
        return Value_->ByteSizeLong();
    }

    int GetCachedSize() const {
        if (!IsUnpacked_) {
            return GetBinarySize();
        }
        return Value_->GetCachedSize();
    }

    void MergeFrom(const TLazyField<T>& from) {
        if (from.IsUnpacked_) {
            Unpack()->MergeFrom(*from.Value_);
        } else {
            T tmp;
            tmp.ParseFromString(*from.BinaryData_);
            Unpack()->MergeFrom(tmp);
        }
    }

    TLazyField() 
    : arena(nullptr)
    , IsUnpacked_(true)
    {
        Value_ = google::protobuf::Arena::CreateMessage<T>(arena);
        BinaryData_ = std::make_shared<std::string>("");
    }

    TLazyField(google::protobuf::Arena* arena)
    : arena(arena) 
    {
        IsUnpacked_ = true;
        Value_ = google::protobuf::Arena::CreateMessage<T>(arena);
        BinaryData_ = std::make_shared<std::string>("");
    }

    TLazyField(const TLazyField<T>& other) {
        arena = other.arena;
        Value_ = google::protobuf::Arena::CreateMessage<T>(arena);
        *Value_ = *other.Value_;
        BinaryData_ = other.BinaryData_;
        BinaryDataList_ = other.BinaryDataList_;
        BinaryDataType_ = other.BinaryDataType_;
        BinarySize_ = other.BinarySize_;
        IsUnpacked_ = other.IsUnpacked_;
    }

    TLazyField(TLazyField<T>&& other) = default;

    TLazyField<T>& operator=(const TLazyField<T>& other) = default;

    TLazyField<T>& operator=(TLazyField<T>&& other) = default;

    ~TLazyField() {
        if (!arena && Value_) {
            delete Value_;
        }
    }

public:
    uint8_t* Serialize(uint8_t* target, ::google::protobuf::io::EpsCopyOutputStream* stream) const {
        if (IsUnpacked_) {
            target = Value_->_InternalSerialize(target, stream);
        } else {
            if (BinaryDataType_ == EBinaryDataType::STRING) {
                target = stream->WriteRaw(BinaryData_->c_str(), BinaryData_->size(), target);
            } else {
                // TODO: Upgrade EpsCopyOutputStream for write list<RefCountBuffer>
                std::string tmp;
                for (const auto& buff : BinaryDataList_) {
                    tmp += std::string(
                        buff.buffer.data.get() + buff.start_offset,
                        buff.buffer.data.get() + buff.buffer.size - buff.end_offset
                    );
                }
                target = stream->WriteRaw(tmp.c_str(), tmp.size(), target);
            }
        }

        return target;
    }

    void InternalParse(std::string&& buff) {
        BinaryData_ = std::make_shared<std::string>(std::move(buff));
        BinaryDataType_ = EBinaryDataType::STRING;
        IsUnpacked_ = false;
    }

    void InternalParse(std::vector<google::protobuf::internal::TLazyRefBuffer> data) {
        BinaryDataList_ = std::move(data);
        BinaryDataType_ = EBinaryDataType::LIST_BUFFERS;
        IsUnpacked_ = false;
    }

private:
    size_t GetBinarySize() const {
        if (BinarySize_) {
            return *BinarySize_;
        }

        if (BinaryDataType_ == EBinaryDataType::STRING) {
            BinarySize_ = BinaryData_->size();
        } else {
            size_t tmp = 0;
            for (const auto& buff : BinaryDataList_) {
                tmp += (buff.buffer.size - (buff.start_offset + buff.end_offset));
            }
            BinarySize_ = tmp;
        }

        return *BinarySize_;
    }

    google::protobuf::Arena* arena = nullptr;
    mutable T* Value_ = nullptr;
    mutable bool IsUnpacked_ = false;

    EBinaryDataType BinaryDataType_;
    std::vector<google::protobuf::internal::TLazyRefBuffer> BinaryDataList_;
    std::shared_ptr<std::string> BinaryData_;
    mutable absl::optional<size_t> BinarySize_;
};