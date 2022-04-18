#pragma once

#include <memory>
#include <iostream>

namespace mem {

struct PlugClass {};

template<typename T>
struct Deleter {
    typedef T ValueType;
    typedef ValueType *Pointer;

    void operator()(void *ptr) {
        delete static_cast<Pointer>(ptr);
    }
};

template<typename T = void, typename D = Deleter<T> /* Args ... */, typename Plug = PlugClass>
struct UniquePtr;


template<typename D>
struct UniquePtr<void, D> : public D {
public:
    typedef void ValueType;
    typedef ValueType *PointType;
    typedef D DeleterType;
    typedef UniquePtr<void, DeleterType> SelfType;

    UniquePtr(const SelfType &other);
    UniquePtr &operator=(const SelfType &other);
    virtual ~UniquePtr();

    PointType release();
    PointType get();

protected:
    explicit UniquePtr(PointType data = NULL);
    PointType releaseData() const;

private:
    void destroyData();

    mutable PointType data_;
};

template<typename T, typename D>
struct UniquePtr<T, D> : public UniquePtr<void, D> {
public:
    typedef T ValueType;
    typedef ValueType *PointType;
    typedef D DeleterType;
    typedef UniquePtr<void, DeleterType> RvalueType;

    explicit UniquePtr(PointType data = NULL);
    explicit UniquePtr(const RvalueType &rvalue);

    UniquePtr<void, DeleterType> move();

    ValueType &operator*();
    PointType operator->();

    PointType release();
    PointType get();

private:
    typedef UniquePtr<void, DeleterType> DataType;

    UniquePtr(const UniquePtr<ValueType, DeleterType> &other);
    UniquePtr<ValueType, DeleterType> operator=(const UniquePtr<ValueType, DeleterType> &other);
};

/**
    template<typename D>
    struct UniquePtr<void, D> : public D
 **/

template<typename D>
UniquePtr<void, D>::UniquePtr(SelfType::PointType data):
data_(data) {
}

template<typename D>
UniquePtr<void, D>::UniquePtr(const SelfType &other):
data_(other.releaseData()) {
}

template<typename D>
UniquePtr<void, D> &UniquePtr<void, D>::operator=(const SelfType &other) {
    destroyData();
    data_ = other.releaseData();
    return *this;
}

template<typename D>
UniquePtr<void, D>::~UniquePtr() {
    destroyData();
}

template<typename D>
typename UniquePtr<void, D>::PointType UniquePtr<void, D>::release() {
    return releaseData();
}

template<typename D>
typename UniquePtr<void, D>::PointType UniquePtr<void, D>::releaseData() const {
    PointType ptr(data_);
    data_ = NULL;
    return ptr;
}

template<typename D>
void UniquePtr<void, D>::destroyData() {
    if (data_ != NULL) {
        DeleterType::operator()(data_);
    }
}

template<typename D>
typename UniquePtr<void, D>::PointType UniquePtr<void, D>::get() {
    return data_;
}

/**
    template<typename T, typename D>
    struct UniquePtr<T, D> : public UniquePtr<void, D>
 **/
template<typename T, typename D>
UniquePtr<T, D>::UniquePtr(PointType data):
DataType(data) {
}

template<typename T, typename D>
UniquePtr<T, D>::UniquePtr(const UniquePtr<void, DeleterType> &rvalue):
DataType(rvalue) {
}

template<typename T, typename D>
UniquePtr<void, D> UniquePtr<T, D>::move() {
    return static_cast<RvalueType>(*this);
}

template<typename T, typename D>
typename UniquePtr<T, D>::ValueType &UniquePtr<T, D>::operator*() {
    return *get();
}

template<typename T, typename D>
typename UniquePtr<T, D>::PointType UniquePtr<T, D>::operator->() {
    return get();
}

template<typename T, typename D>
typename UniquePtr<T, D>::PointType UniquePtr<T, D>::release() {
    return static_cast<PointType>(DataType::release());
}

template<typename T, typename D>
typename UniquePtr<T, D>::PointType UniquePtr<T, D>::get() {
    return static_cast<PointType>(DataType::get());
}

} // namespace mem
