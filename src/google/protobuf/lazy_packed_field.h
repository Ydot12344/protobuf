#include <string>
#include <google/protobuf/parse_context.h>

template<class T>
class TLazyField {
public:
    constexpr TLazyField<T>(std::nullptr_t)
    {}

    TLazyField<T>& operator=(T* value) {
        Value_ = value;
        IsUnpacked_ = true;
        return *this;
    } 

    TLazyField(std::string&& binary) 
    : BinaryData_(std::move(binary))
    , IsUnpacked_(false)
    {}

    TLazyField(const TLazyField<T>& other) 
    : IsUnpacked_(other.IsUnpacked_)
    , BinaryData_(other.BinaryData_)
    {
        if (Value_) {
            if (other.Value_) {
                *Value_ = *other.Value_;
            } else {
                delete Value_;
            }
        } else {
            if (other.Value_) {
                Value_ = CreateMaybeMessage<T>(GetArenaForAllocation());
                *Value_ = *other.Value_;
            } else {
                Value_ = nullptr;
            }
        }
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

    TLazyField(TLazyField<T>&& other)
    : IsUnpacked_(other.IsUnpacked_)
    , BinaryData_(std::move(other.BinaryData_))
    {
        auto* tmp = other.Value_;
        other.Value_= Value_;
        Value_ = tmp;
        other.IsUnpacked_ = false;
    }

    operator T*() const {
        return Value_;
    }

    T& operator*() const {
        return Value_;
    }

    T* operator->() const {
        return Value_;
    }

    T* Unpack() {
        if (!IsUnpacked_) {
            Value_ = CreateMaybeMessage<T>(GetArenaForAllocation());
            Value_->ParseFromString(BinaryData_);
            IsUnpacked_ = true;
        }

        return Value_;
    }

    ~TLazyField() {
        if (Value_) 
            delete Value_;
    }
private:
    T* Value_;
    bool IsUnpacked_ = false;
    std::string BinaryData_;
};