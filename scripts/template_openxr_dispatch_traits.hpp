//## Copyright (c) 2017-2021 The Khronos Group Inc.
//## Copyright (c) 2019-2021 Collabora, Ltd.
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

//# include('file_header.hpp')
/**
 * @file
 * @brief Contains a trait to identify dispatcher classes.
 * @ingroup dispatch
 */

#include <type_traits>

//# include('define_namespace.hpp') without context

namespace OPENXR_HPP_NAMESPACE {
namespace traits {
    /*!
     * @brief Type trait identifying if a class is a dispatcher.
     *
     * Without this, there would be many more ambiguous methods in the enhanced mode.
     * To make your own dispatch class marked as a dispatch class, just specialize this template accordingly.
     *
     * Default implementation is "false".
     * @see OPENXR_HPP_CLASS_IS_DISPATCH
     * @ingroup dispatch
     */
    template <typename T>
    struct is_dispatch : std::false_type {};


#ifndef OPENXR_HPP_DOXYGEN
    // forward, removing reference
    template <typename T>
    struct is_dispatch<T&> : is_dispatch<T> {};
    // forward, removing const
    template <typename T>
    struct is_dispatch<T const> : is_dispatch<T> {};
#endif  //! OPENXR_HPP_DOXYGEN

}  // namespace traits
}  // namespace OPENXR_HPP_NAMESPACE

/*!
 * @brief Macro to tag a type as a dispatcher
 *
 * Invoke this, outside of all namespaces, with the qualified name of the type you'd like to mark as a dispatcher.
 * This macro will specialize the type trait accordingly.
 *
 * @see traits::is_dispatch
 * @ingroup dispatch
 */
#define OPENXR_HPP_CLASS_IS_DISPATCH(TYPE)                \
    namespace OPENXR_HPP_NAMESPACE {                      \
        namespace traits {                                \
            template <>                                   \
            struct is_dispatch<TYPE> : std::true_type {}; \
        }                                                 \
    }

/*!
 * @brief Implementation detail: Macro for enable_if to verify a type is a dispatch.
 *
 * You can ignore the presence of this: you will never need to specify an argument for it.
 * It exists solely to make sure that only actual dispatch classes are accepted for the Dispatch templated argument.
 *
 * In more detail:
 * This is used in wrapper function template argument lists to prevent overload ambiguity caused by the optional Dispatch parameter to all calls.
 * It ensures that an overload is available with a type as a dispatch only if that type actually is marked as a dispatch (using traits::is_dispatch).
 * This macro expands to a type expression: it evaluates to `int` if `TYPE` is a dispatch type, and is a substitution failure otherwise.
 * Due to the "substitution failure is not an error" (SFINAE) principle, it can thus remove an overload from consideration.
 *
 * This macro is defined to shorten repeated uses of this expression.
 *
 * @see traits::is_dispatch
 * @ingroup dispatch
 */
#define OPENXR_HPP_REQUIRE_DISPATCH(TYPE) \
    typename std::enable_if<::OPENXR_HPP_NAMESPACE::traits::is_dispatch<Dispatch>::value, int>::type

//# include('file_footer.hpp')
