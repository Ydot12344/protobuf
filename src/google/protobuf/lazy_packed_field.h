#pragma once

#include "google/protobuf/message.h"
#include "google/protobuf/port.h"
#include "google/protobuf/message_lite.h"
#include <google/protobuf/arena.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/parse_context.h>

#include <string>
#include <optional>

namespace google {
namespace protobuf {

/// @brief store raw binary data without parsing
/// and provide Unpack method to deserialize data to Message.
/// @tparam T Message which will be stored in raw form
template<class T>
class TLazyField : public MessageLite {
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
    ~TLazyField();

    std::string GetTypeName() const override;
    TLazyField<T>* New(Arena* arena) const override;
    
    void Clear() override;
    
    bool IsInitialized() const override;
    
    void CheckTypeAndMergeFrom(const MessageLite& other) override;
    
    size_t ByteSizeLong() const override;
    
    int GetCachedSize() const override;
    
    const Descriptor* GetDescriptor() const;
    
    const Reflection* GetReflection() const;
    
    T* Unpack() const;
    
    void MergeFrom(const TLazyField<T>& from);

    const char* _InternalParse(const char* ptr, internal::ParseContext* ctx) override;

    void _InternalParse(std::string&& buff);

    void _InternalParse(std::vector<google::protobuf::internal::TLazyRefBuffer> data);

private:
    uint8_t* _InternalSerialize(uint8_t* ptr, io::EpsCopyOutputStream* stream) const override;
    size_t GetBinarySize() const;

protected:

private:
    mutable google::protobuf::Arena* arena = nullptr;
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
const char* TLazyField<T>::_InternalParse(const char* ptr, internal::ParseContext* ctx) {
    if (ctx->IsDerivedFromReleasableBufferStream()) {
        _InternalParse(ctx->ReadAllDataAsBuffersArray(&ptr));
    } else {
        _InternalParse(ctx->ReadAllDataAsString(&ptr));
    }
    return ptr;
}

template<class T>
uint8_t* TLazyField<T>::_InternalSerialize(uint8_t* ptr, io::EpsCopyOutputStream* stream) const {
    if (IsUnpacked_) {
        ptr = Value_->_InternalSerialize(ptr, stream);
    } else {
        if (BinaryDataType_ == EBinaryDataType::STRING) {
            ptr = stream->WriteRaw(BinaryData_->c_str(), BinaryData_->size(), ptr);
        } else {
            for (const auto& buff : BinaryDataList_) {
                uint8_t* data_start = buff.data.get() + buff.start_offset;
                uint8_t* data_end = buff.data.get() + buff.size - buff.end_offset;                
                ptr = stream->WriteRaw(data_start, data_end - data_start, ptr);
            }
        }
    }

    return ptr;
}

template<class T>
T* TLazyField<T>::Unpack() const {
    if (!IsUnpacked_) {
        if (BinaryDataType_ == EBinaryDataType::STRING) {
            Value_->ParseFromString(*BinaryData_);
        } else {
            std::string tmp;
            for (const auto& buff : BinaryDataList_) {
                tmp += std::string(
                    buff.data.get() + buff.start_offset,
                    buff.data.get() + buff.size - buff.end_offset
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
void TLazyField<T>::_InternalParse(std::string&& buff) {
    BinaryData_ = std::make_shared<std::string>(std::move(buff));
    BinaryDataType_ = EBinaryDataType::STRING;
    IsUnpacked_ = false;
}

template<class T>
void TLazyField<T>::_InternalParse(std::vector<google::protobuf::internal::TLazyRefBuffer> data) {
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
            tmp += (buff.size - (buff.start_offset + buff.end_offset));
        }
        BinarySize_ = tmp;
    }

    return *BinarySize_;
}

template<class T>
const Descriptor* TLazyField<T>::GetDescriptor() const {
    return T::GetDescriptor();
}

template<class T>
const Reflection* TLazyField<T>::GetReflection() const {
    return T::GetReflection();
}

template<class T>
std::string TLazyField<T>::GetTypeName() const {
    return GetDescriptor()->full_name();
}

template<class T>
TLazyField<T>* TLazyField<T>::New(Arena* arena) const {
    this->arena = arena;
    return CreateMaybeMessage<TLazyField<T>>(arena);
}

template<class T>
void TLazyField<T>::CheckTypeAndMergeFrom(const MessageLite& other) {
    MergeFrom(
        *internal::DownCast<const TLazyField<T>*>(&other)
    );
}

} // namespace protobuf
} // namespace google
