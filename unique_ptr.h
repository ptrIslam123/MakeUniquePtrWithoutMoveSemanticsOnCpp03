#pragma once

#include <cassert>

namespace mem {

/**
 * @brief Класс-функтор для очистки данных.
 * @tparam T Тип очищаемых данных.
 */
template<class T>
struct Deleter {
    void operator()(T *ptr) {
        delete ptr;
    }
};

/**
 * @brief Класс реализующий сущность уникального умного указателя
 * @tparam T Тип объекта, который будет хранить умный указатель
 * @tparam N Размер пула объектов
 */
template<class T, class D = Deleter<T> >
class UniquePtr {
public:
    //! Псевдоним для типа объекта от которого будет сконструирован умный указатель.
    typedef T ValueType;
    //! Псевдоним для типа пула объектов, размера N.
    typedef D Deleter;

    /**
     * @brief Внутренний класс умного указателя реализующий сущность rvalue объекта.
     * @details Данный объект нельзя разыменовать или/и обратиться к его внутренним членам.
     * Его единственная задача быть сконструированным от другого умного уникального указателя,
     * передавая управление другому. Если этого не произойдет и вызовется его деструктор он просто
     * затрет данные которые он хранит.
     */
    class RvalueUniquePtr {
    public:
        /**
         * @brief Копирующий конструктор класса.
         * @param other Ссылка на другой объект класса RvalueUniquePtr.
         */
        RvalueUniquePtr(const RvalueUniquePtr &other);

        /**
         * @brief Деструктор для класса для очистки ресурсов.
         */
        ~RvalueUniquePtr();

        /**
         * @brief Оператор копирования класса.
         * @param other Ссылка на другой объект класса RvalueUniquePtr.
         * @return Возвращает ссылку на объект, сконструированный от переданного объекта.
         */
        RvalueUniquePtr &operator=(const RvalueUniquePtr &other);

        /**
         *
         * @return
         */
        ValueType *release();

    private:
        //! Делаем класс UniquePtr другом этого класса.
        friend class UniquePtr<ValueType>;

        /**
         * @brief Приватный конструктор класса.
         * @param poolObj Указатель на экземпляр пула объектов.
         * @param ptr Указатель на созданный объект хранения.
         */
        RvalueUniquePtr(ValueType *ptr);

        /**
         * @brief Метод для зануления указателя на экземпляр объекта хранения.
         */
        void freeData() const;

        mutable ValueType *data_; //!< Указатель на экземпляр объекта хранения.
        Deleter deleter_; //!< Функтор для освобождения данных
    };

    /**
     * @brief Конструктор по умолчанию. Инициализирует членены класса NULL.
     */
    UniquePtr();

    /**
     * @brief Конструктор класса, принимающий указатели на экземпляры пула объектов и объекта
     * хранения.
     * @param poolObj Указатель на экземпляр пула объектов.
     * @param ptr Указатель на экземпляр объекта хранения.
     */
    UniquePtr(ValueType *ptr);

    /**
     * @brief Конструктор класса от ссылки на объект RvalueUniquePtr.
     * @param rvalue Ссылка на экземпляр класса RvalueUniquePtr.
     */
    explicit UniquePtr(const RvalueUniquePtr &rvalue);

    /**
     * @brief Копирующий оператор класса от ссылки на экземпляр класса RvalueUniquePtr.
     * @param rvalue Ссылка на экземпляр класса RvalueUniquePtr.
     * @return Ссылку на объект класса, сконструированного от переданного экземпляра RvalueUniquePtr.
     */
    UniquePtr &operator=(const RvalueUniquePtr &rvalue);

    /**
     * @brief Деструктор класса для очистки ресурсов.
     */
    ~UniquePtr();

    /**
     * @brief Метод для создания экземпляра класса RvalueUniquePtr с данными этого объекта.
     * @details Используется что бы передать владение другому объекту класса умного указателя.
     * @return Объект класса RvalueUniquePtr.
     */
    RvalueUniquePtr move();

    ValueType *release();

    /**
     * @brief Оператор разыменования.
     * @details Подразумевает, что UniquePtr не перемещен(moved).
     * @return Ссылка на значение внутри UniquePtr
     */
    ValueType &operator*();

    /**
     * @brief Константный оператор разыменования.
     * @details Подразумевает, что UniquePtr не перемещен(moved).
     * @return Константная ссылка на значение внутри UniquePtr
     */
    ValueType &operator*() const;

    /**
     * @brief Оператор ->
     * @details Позволяет обращаться к внутренним членам значения, содержащегося внутри UniquePtr.
     */
    ValueType *operator->();

    /**
     * @brief Константный оператор ->
     * @details Позволяет обращаться к внутренним членам значения, содержащегося внутри UniquePtr.
     */
    ValueType *operator->() const;

    /**
     * @brief Получить указатель на неконстантный внутренний объект.
     * @return Возвращает указатель на внутренний объект.
     */
    ValueType *get();

    /**
     * @brief Получить указатель на константный внутренний объект.
     * @return Возвращает указатель на внутренний объект.
     */
    ValueType *get() const;

private:
    /**
     * @brief Приватный конструктор копирования.
     * @param other Ссылка на другой экземпляр класса UniquePtr.
     */
    UniquePtr(const UniquePtr<ValueType> &other);

    /**
     * @brief Приватный оператор копирования.
     * @param other Ссылка на другой экземпляр класса UniquePtr.
     * @return Ссылку на новый объект класса UniquePtr, сконструированного от переданного экземпляра.
     */
    UniquePtr<ValueType> &operator=(const UniquePtr<ValueType> &other);

    /**
     * @brief Метод для зануления указателя на экземпляр объекта хранения.
     */
    void freeData() const;

    mutable ValueType *data_; //!< Указатель на экземпляр объекта хранения.
    Deleter deleter_; //!< Функтор для освобождения данных
};

template<class T, class D>
UniquePtr<T, D>::RvalueUniquePtr::RvalueUniquePtr(ValueType *ptr):
data_(ptr),
deleter_() {
}

template<class T, class D>
UniquePtr<T, D>::RvalueUniquePtr::RvalueUniquePtr(const RvalueUniquePtr &other):
data_(other.data_),
deleter_() {
    other.freeData();
}

template<class T, class D>
UniquePtr<T, D>::RvalueUniquePtr::~RvalueUniquePtr() {
    if (data_ != NULL) {
        deleter_(data_);
    }
}

template<class T, class D>
typename UniquePtr<T, D>::ValueType *UniquePtr<T, D>::RvalueUniquePtr::release() {
    ValueType *ptr = data_;
    data_ = NULL;
    return ptr;
}

template<class T, class D>
typename UniquePtr<T, D>::RvalueUniquePtr
        &UniquePtr<T, D>::RvalueUniquePtr::operator=(const RvalueUniquePtr &other) {
    data_ = other.data_;
    other.freeData();

    return *this;
}

template<class T, class D>
void UniquePtr<T, D>::RvalueUniquePtr::freeData() const {
    data_ = NULL;
}

template<class T, class D>
UniquePtr<T, D>::UniquePtr():
data_(NULL),
deleter_() {
}

template<class T, class D>
UniquePtr<T, D>::UniquePtr(ValueType *ptr):
data_(ptr),
deleter_() {
}

template<class T, class D>
UniquePtr<T, D>::UniquePtr(const RvalueUniquePtr &rvalue):
data_(rvalue.data_){
    rvalue.freeData();
}

template<class T, class D>
UniquePtr<T, D> &UniquePtr<T, D>::operator=(const RvalueUniquePtr &rvalue) {
    data_ = rvalue.data_;
    rvalue.freeData();

    return *this;
}

template<class T, class D>
UniquePtr<T, D>::~UniquePtr() {
    if (data_ != NULL) {
        deleter_(data_);
    }
}

template<class T, class D>
typename UniquePtr<T, D>::RvalueUniquePtr UniquePtr<T, D>::move() {
    RvalueUniquePtr rvalue(data_);
    data_ = NULL;
    return rvalue;
}

template<class T, class D>
typename UniquePtr<T, D>::ValueType *UniquePtr<T, D>::release() {
    ValueType *ptr = data_;
    data_ = NULL;
    return data_;
}

template<class T, class D>
typename UniquePtr<T, D>::ValueType &UniquePtr<T, D>::operator*() {
    assert(data_ != NULL);
    return *data_;
}

template<class T, class D>
typename UniquePtr<T, D>::ValueType& UniquePtr<T, D>::operator*() const {
    assert(data_ != NULL);
    return *data_;
}

template<class T, class D>
typename UniquePtr<T, D>::ValueType* UniquePtr<T, D>::operator->() {
    assert(data_ != NULL);
    return data_;
}

template<class T, class D>
typename UniquePtr<T, D>::ValueType* UniquePtr<T, D>::operator->() const {
    assert(data_ != NULL);
    return data_;
}

template<class T, class D>
typename UniquePtr<T, D>::ValueType *UniquePtr<T, D>::get() {
    return data_;
}

template<class T, class D>
typename UniquePtr<T, D>::ValueType *UniquePtr<T, D>::get() const {
    return data_;
}

template<class T, class D>
void UniquePtr<T, D>::freeData() const {
    data_ = NULL;
}

} // namespace mem
