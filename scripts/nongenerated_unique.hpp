//## Copyright (c) 2017-2019 The Khronos Group Inc.
//## Copyright (c) 2019 Collabora, Ltd.
//##
//## Licensed under the Apache License, Version 2.0 (the "License");
//## you may not use this file except in compliance with the License.
//## You may obtain a copy of the License at
//##
//##     http://www.apache.org/licenses/LICENSE-2.0
//##
//## Unless required by applicable law or agreed to in writing, software
//## distributed under the License is distributed on an "AS IS" BASIS,
//## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//## See the License for the specific language governing permissions and
//## limitations under the License.
//##
//## ---- Exceptions to the Apache 2.0 License: ----
//##
//## As an exception, if you use this Software to generate code and portions of
//## this Software are embedded into the generated code as a result, you may
//## redistribute such product without providing attribution as would otherwise
//## be required by Sections 4(a), 4(b) and 4(d) of the License.
//##
//## In addition, if you combine or link code generated by this Software with
//## software that is licensed under the GPLv2 or the LGPL v2.0 or 2.1
//## ("`Combined Software`") and if a court of competent jurisdiction determines
//## that the patent provision (Section 3), the indemnity provision (Section 9)
//## or other Section of the License conflicts with the conditions of the
//## applicable GPL or LGPL license, you may retroactively and prospectively
//## choose to deem waived or otherwise exclude such Section(s) of the License,
//## but only in their entirety and only with respect to the Combined Software.


namespace OPENXR_HPP_NAMESPACE {

#ifndef OPENXR_HPP_NO_SMART_HANDLE

namespace traits {

    template <typename Type, typename Dispatch>
    class UniqueHandleTraits;

}  // namespace traits

namespace impl {

    // Used when returning unique handles.
    template <typename T>
    using RemoveRefConst = typename std::remove_const<typename std::remove_reference<T>::type>::type;
}  // namespace impl

/*!
 * @brief Template class for holding a handle with unique ownership, much like unique_ptr.
 *
 * Note that this does not keep track of children or parents, though OpenXR specifies that destruction of a handle also destroys its
 * children automatically. Thus, it is important to order destruction of these correctly, usually by ordering declarations.
 *
 * Inherits from the deleter to use empty-base-class optimization when possible.
 */
template <typename Type, typename Dispatch>
class UniqueHandle : public traits::UniqueHandleTraits<Type, Dispatch>::deleter {
   private:
    using Deleter = typename traits::UniqueHandleTraits<Type, Dispatch>::deleter;

   public:
    //! Explicit constructor with deleter.
    explicit UniqueHandle(Type const &value = Type(), Deleter const &deleter = Deleter())
        : Deleter(deleter), m_value(value) {}

    // Cannot copy
    UniqueHandle(UniqueHandle const &) = delete;

    //! Move constructor
    UniqueHandle(UniqueHandle &&other) : Deleter(std::move(static_cast<Deleter &>(other))), m_value(other.release()) {}

    //! Destructor: destroys owned handle if valid.
    ~UniqueHandle() {
        if (m_value) this->destroy(m_value);
    }

    // cannot copy-assign
    UniqueHandle &operator=(UniqueHandle const &) = delete;

    //! Move-assignment operator.
    UniqueHandle &operator=(UniqueHandle &&other) {
        reset(other.release());
        *static_cast<Deleter *>(this) = std::move(static_cast<Deleter &>(other));
        return *this;
    }

    //! Explicit bool conversion: for testing if the handle is valid.
    explicit operator bool() const { return m_value.operator bool(); }

    // Smart pointer operator
    Type const *operator->() const { return &m_value; }

    // Smart pointer operator
    Type *operator->() { return &m_value; }

    // Smart pointer operator
    Type const &operator*() const { return m_value; }

    // Smart pointer operator
    Type &operator*() { return m_value; }

    //! Get the underlying (wrapped) handle type.
    const Type &get() const { return m_value; }

    //! Get the underlying (wrapped) handle type.
    Type &get() { return m_value; }

    //! Get the raw OpenXR handle or XR_NULL_HANDLE
    typename Type::RawHandleType getRawHandle() { return m_value ? m_value.get() : XR_NULL_HANDLE; }

    //! Clear or re-assign the underlying handle
    void reset(Type const &value = Type()) {
        if (m_value != value) {
            if (m_value) this->destroy(m_value);
            m_value = value;
        }
    }

    //! Clear this handle and return a pointer to the storage, for assignment/creation purposes.
    typename Type::RawHandleType *put() {
        reset();
        return m_value.put();
    }

    //! Relinquish ownership of the contained handle and return it without destroying it.
    Type release() {
        Type value = m_value;
        m_value = nullptr;
        return value;
    }

    //! Swap with another handle of this type.
    void swap(UniqueHandle<Type, Dispatch> &rhs) {
        std::swap(m_value, rhs.m_value);
        std::swap(static_cast<Deleter &>(*this), static_cast<Deleter &>(rhs));
    }

   private:
    Type m_value;
};

template <typename Type, typename Dispatch>
class UniqueHandle<Type, Dispatch &> : public UniqueHandle<Type, Dispatch> {};

template <typename Type, typename Dispatch>
class UniqueHandle<Type, Dispatch const> : public UniqueHandle<Type, Dispatch> {};

//! @relates UniqueHandle
template <typename Type, typename Dispatch>
OPENXR_HPP_INLINE void swap(UniqueHandle<Type, Dispatch> &lhs, UniqueHandle<Type, Dispatch> &rhs) {
    lhs.swap(rhs);
}

//! @relates UniqueHandle
template <typename Type, typename Dispatch>
OPENXR_HPP_INLINE const Type& get(const UniqueHandle<Type, Dispatch> &h) {
    return h.get();
}

//! @brief Equality comparison between two UniqueHandles, potentially of different dispatch.
//! @relates UniqueHandle
template <typename Type, typename D1, typename D2>
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool operator==(UniqueHandle<Type, D1> const &lhs,
                                                       UniqueHandle<Type, D2> const &rhs) {
    return lhs.get() == rhs.get();
}
//! @brief Inequality comparison between two UniqueHandles, potentially of different dispatch.
//! @relates UniqueHandle
template <typename Type, typename D1, typename D2>
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool operator!=(UniqueHandle<Type, D1> const &lhs,
                                                       UniqueHandle<Type, D2> const &rhs) {
    return lhs.get() != rhs.get();
}
//! @brief Equality comparison between UniqueHandle and nullptr: true if the handle is
//! null.
//! @relates UniqueHandle
template <typename Type, typename Dispatch>
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool operator==(UniqueHandle<Type, Dispatch> const &lhs,
                                                       std::nullptr_t /* unused */) {
    return lhs.get() == XR_NULL_HANDLE;
}
//! @brief Equality comparison between nullptr and UniqueHandle: true if the handle is
//! null.
//! @relates UniqueHandle
template <typename Type, typename Dispatch>
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool operator==(std::nullptr_t /* unused */,
                                                       UniqueHandle<Type, Dispatch> const &rhs) {
    return rhs.get() == XR_NULL_HANDLE;
}
//! @brief Inequality comparison between UniqueHandle and nullptr: true if the handle
//! is not null.
//! @relates UniqueHandle
template <typename Type, typename Dispatch>
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool operator!=(UniqueHandle<Type, Dispatch> const &lhs,
                                                       std::nullptr_t /* unused */) {
    return lhs.get() != XR_NULL_HANDLE;
}
//! @brief Inequality comparison between nullptr and UniqueHandle: true if the handle
//! is not null.
//! @relates UniqueHandle
template <typename Type, typename Dispatch>
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool operator!=(std::nullptr_t /* unused */,
                                                       UniqueHandle<Type, Dispatch> const &rhs) {
    return rhs.get() != XR_NULL_HANDLE;
}
#endif

template <typename Dispatch>
class ObjectDestroy {
   public:
    ObjectDestroy(Dispatch const &dispatch = Dispatch()) : m_dispatch(&dispatch) {}

   protected:
    template <typename T>
    void destroy(T t) {
        t.destroy(*m_dispatch);
    }

   private:
    Dispatch const *m_dispatch;
};
}  // namespace OPENXR_HPP_NAMESPACE
