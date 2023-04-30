#pragma once

#include "google/protobuf/message.h"
#include <string>
#include <google/protobuf/arena.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/parse_context.h>

#include <optional>

/// @brief store raw binary data without parsing
/// and provide Unpack method to deserialize data to Message.
/// @tparam T Message which will be stored in raw form
template<class T>
class TLazyField {

enum class EBinaryDataType {
    STRING = 0,
    LIST_BUFFERS = 1
};

public:
    TLazyField();
    TLazyField(google::protobuf::Arena* arena);
    TLazyField(const TLazyField<T>& other);
    TLazyField<T>& operator=(const TLazyField<T>& other);

    TLazyField(TLazyField<T>&& other);
    TLazyField<T>& operator=(TLazyField<T>&& other);

    uint8_t* Serialize(uint8_t* target, ::google::protobuf::io::EpsCopyOutputStream* stream) const;
    void InternalParse(std::string&& buff);
    void InternalParse(std::vector<google::protobuf::internal::TLazyRefBuffer> data);
    T* Unpack() const;

    bool IsInitialized() const;
    void Clear();
    size_t ByteSizeLong() const;
    int GetCachedSize() const;
    void MergeFrom(const TLazyField<T>& from);

    ~TLazyField();
private:
    size_t GetBinarySize() const;

private:
    google::protobuf::Arena* arena = nullptr;
    mutable T* Value_ = nullptr;
    mutable bool IsUnpacked_ = false;

    EBinaryDataType BinaryDataType_;
    std::vector<google::protobuf::internal::TLazyRefBuffer> BinaryDataList_;
    std::shared_ptr<std::string> BinaryData_;
    mutable absl::optional<size_t> BinarySize_;
};

/////////////////////////////////////////////////////////////////////////////////////

template<class T>
TLazyField<T>::TLazyField() 
    : arena(nullptr)
    , IsUnpacked_(true)
{
    Value_ = google::protobuf::Arena::CreateMessage<T>(arena);
    BinaryData_ = std::make_shared<std::string>("");
}

template<class T>
TLazyField<T>::TLazyField(google::protobuf::Arena* arena)
    : arena(arena) 
{
    IsUnpacked_ = true;
    Value_ = google::protobuf::Arena::CreateMessage<T>(arena);
    BinaryData_ = std::make_shared<std::string>("");
}


template<class T>
TLazyField<T>::TLazyField(const TLazyField<T>& other) {
    arena = other.arena;
    if (other.Value_) {
        Value_ = google::protobuf::Arena::CreateMessage<T>(arena);
        *Value_ = *other.Value_;
    }
    BinaryData_ = other.BinaryData_;
    BinaryDataList_ = other.BinaryDataList_;
    BinaryDataType_ = other.BinaryDataType_;
    BinarySize_ = other.BinarySize_;
    IsUnpacked_ = other.IsUnpacked_;
}

template<class T>
TLazyField<T>::TLazyField(TLazyField<T>&& other) {
    arena = other.arena;
    
    Value_ = other.Value_;
    other.Value_ = nullptr;

    BinaryData_ = std::move(other.BinaryData_);
    BinaryDataList_ = std::move(other.BinaryDataList_);
    BinaryDataType_ = other.BinaryDataType_;
    BinarySize_ = other.BinarySize_;
    IsUnpacked_ = other.IsUnpacked_;
}

template<class T>
TLazyField<T>& TLazyField<T>::operator=(TLazyField<T>&& other) {
    arena = other.arena;
    
    Value_ = other.Value_;
    other.Value_ = nullptr;

    BinaryData_ = std::move(other.BinaryData_);
    BinaryDataList_ = std::move(other.BinaryDataList_);
    BinaryDataType_ = other.BinaryDataType_;
    BinarySize_ = other.BinarySize_;
    IsUnpacked_ = other.IsUnpacked_;
    
    return *this;
}

template<class T>
TLazyField<T>& TLazyField<T>::operator=(const TLazyField<T>& other) {
    arena = other.arena;
    if (other.Value_) {
        Value_ = google::protobuf::Arena::CreateMessage<T>(arena);
        *Value_ = *other.Value_;
    }
    BinaryData_ = other.BinaryData_;
    BinaryDataList_ = other.BinaryDataList_;
    BinaryDataType_ = other.BinaryDataType_;
    BinarySize_ = other.BinarySize_;
    IsUnpacked_ = other.IsUnpacked_;

    return *this;
}

template<class T>
uint8_t* TLazyField<T>::Serialize(uint8_t* target, ::google::protobuf::io::EpsCopyOutputStream* stream) const {
    if (IsUnpacked_) {
        target = Value_->_InternalSerialize(target, stream);
    } else {
        if (BinaryDataType_ == EBinaryDataType::STRING) {
            target = stream->WriteRaw(BinaryData_->c_str(), BinaryData_->size(), target);
        } else {
            for (const auto& buff : BinaryDataList_) {
                uint8_t* data_start = buff.buffer.data.get() + buff.start_offset;
                uint8_t* data_end = buff.buffer.data.get() + buff.buffer.size - buff.end_offset;                
                target = stream->WriteRaw(data_start, data_end - data_start, target);
            }
        }
    }

    return target;
}

template<class T>
T* TLazyField<T>::Unpack() const {
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

template<class T>
bool TLazyField<T>::IsInitialized() const {
    return true;
}

template<class T>
void TLazyField<T>::Clear() {
    IsUnpacked_ = false;
    if (Value_) Value_->Clear();
    BinaryData_ = std::make_shared<std::string>("");
    BinaryDataList_.clear();
    BinaryDataType_ = EBinaryDataType::STRING;
    BinarySize_.reset();
}

template<class T>
size_t TLazyField<T>::ByteSizeLong() const {
    if (!IsUnpacked_) {
        return GetBinarySize();
    }
    return Value_->ByteSizeLong();
}

template<class T>
int TLazyField<T>::GetCachedSize() const {
    if (!IsUnpacked_) {
        return GetBinarySize();
    }
    return Value_->GetCachedSize();
}

template<class T>
void TLazyField<T>::MergeFrom(const TLazyField<T>& from) {
    if (from.IsUnpacked_) {
        Unpack()->MergeFrom(*from.Value_);
    } else {
        T tmp;
        tmp.ParseFromString(*from.BinaryData_);
        Unpack()->MergeFrom(tmp);
    }
}

template<class T>
TLazyField<T>::~TLazyField() {
    if (!arena && Value_) {
        delete Value_;
    }
}

template<class T>
void TLazyField<T>::InternalParse(std::string&& buff) {
    BinaryData_ = std::make_shared<std::string>(std::move(buff));
    BinaryDataType_ = EBinaryDataType::STRING;
    IsUnpacked_ = false;
}

template<class T>
void TLazyField<T>::InternalParse(std::vector<google::protobuf::internal::TLazyRefBuffer> data) {
    BinaryDataList_ = std::move(data);
    BinaryDataType_ = EBinaryDataType::LIST_BUFFERS;
    IsUnpacked_ = false;
}

template<class T>
size_t TLazyField<T>::GetBinarySize() const {
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
