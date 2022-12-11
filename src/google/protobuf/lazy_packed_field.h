#pragma once

#include <string>
#include <google/protobuf/arena.h>
#include <google/protobuf/io/coded_stream.h>

template<class T>
class TLazyField {
public:
    uint8_t* Serialize(uint8_t* target, ::google::protobuf::io::EpsCopyOutputStream* stream) const {
        if (IsUnpacked_) {
            std::string tmp;
            Value_->SerializeToString(&tmp);
            target = stream->WriteRaw(tmp.data(), tmp.size(), target);    
        } else {
            target = stream->WriteRaw(BinaryData_.c_str(), BinaryData_.size(), target);    
        }

        return target;
    }

    void InternalParse(std::string&& buff) {
        BinaryData_ = std::move(buff);
        IsUnpacked_ = false;
    }

    T* Unpack() {
        if (!IsUnpacked_) {
            Value_->ParseFromString(BinaryData_);    
        }

        return Value_;
    }

    bool IsInitialized() const {
        return true;
    }

    void Clear() {
        IsUnpacked_ = false;
        if (Value_) Value_->Clear();
        BinaryData_.clear();
    }

    size_t ByteSizeLong() const {
        if (!IsUnpacked_) return BinaryData_.size();
        return Value_->ByteSizeLong();
    }

    int GetCachedSize() const {
        if (!IsUnpacked_) return BinaryData_.size();
        return Value_->GetCachedSize();
    }

    void MergeFrom(const TLazyField<T>& from) {
        if (from.IsUnpacked_) {
            Unpack()->MergeFrom(*from.Value_);
        } else {
            T tmp;
            tmp.ParseFromString(from.BinaryData_);
            Unpack()->MergeFrom(tmp);
        }
    }

    TLazyField() 
    : arena(nullptr)
    , IsUnpacked_(true)
    {
        Value_ = google::protobuf::Arena::CreateMessage<T>(arena);
    }

    TLazyField(google::protobuf::Arena* arena)
    : arena(arena) 
    {
        IsUnpacked_ = true;
        Value_ = google::protobuf::Arena::CreateMessage<T>(arena);
    }

    TLazyField(const TLazyField<T>& other) {
        arena = other.arena;
        Value_ = google::protobuf::Arena::CreateMessage<T>(arena);
        *Value_ = *other.Value_;
        IsUnpacked_ = other.IsUnpacked_;
    }

    TLazyField(TLazyField<T>&& other) {
        arena = other.arena;
        other.arena = nullptr;
        Value_ = other.Value_;
        other.Value_ = nullptr;
        BinaryData_ = std::move(other.BinaryData_);
        IsUnpacked_ = other.IsUnpacked_;
    }

    TLazyField<T>& operator=(const TLazyField<T>& other) {
        TLazyField<T> tmp = other;
        std::swap(*this, tmp);
        return *this;
    }

    TLazyField<T>& operator=(TLazyField<T>&& other) {
        TLazyField<T> tmp = std::move(other);
        std::swap(*this, tmp);
        return *this;
    }

    ~TLazyField() {
        if (!arena && !Value_) {
            delete Value_;
        }
    }

private:
    google::protobuf::Arena* arena = nullptr;
    T* Value_ = nullptr;
    bool IsUnpacked_ = false;
    std::string BinaryData_;
};