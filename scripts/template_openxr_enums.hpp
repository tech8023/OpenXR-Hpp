//## Copyright (c) 2017-2020 The Khronos Group Inc.
//## Copyright (c) 2019-2020 Collabora, Ltd.
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

//# from 'macros.hpp' import make_spec_url, include_guard_begin, include_guard_end

//# include('copyright_header.hpp') without context

/*{ include_guard_begin() }*/


#include <openxr/openxr.h>

#include <string>

// Fix name collisions from noisy includes
#ifdef Success
#undef Success
#endif
#ifdef None
#undef None
#endif

//# include('define_inline_constexpr.hpp') without context
//# include('define_namespace.hpp') without context

namespace OPENXR_HPP_NAMESPACE {
/*!
 * @defgroup enums Enumerations
 * @brief C++ enum classes corresponding to OpenXR C enumerations, plus associated utility functions.
 *
 * All enumerations have three utility functions defined:
 *
 * - get() - returns the raw C enum value
 * - to_string_literal() - returns a const char* containing the C++ name
 * - to_string() - wraps to_string_literal(), returning a std::string
 *
 * @{
 */
//# for enum in gen.api_enums
//#     set projected_type = project_type_name(enum.name)
/*{ protect_begin(enum) }*/
//! @brief Enum class associated with /*{enum.name}*/
//!
//! See the related specification text at /*{ make_spec_url(enum.name) }*/
enum class /*{projected_type -}*/ : /*{ 'int32_t' if enum.name == 'XrResult' else 'uint32_t' }*/ {
//#     for val in enum.values
    /*{ protect_begin(val, enum) }*/
    /*{create_enum_value(val.name, enum.name)}*/ = /*{val.name}*/,
    /*{ protect_end(val, enum) }*/
//#     endfor
};

//! @brief Free function for retrieving the raw /*{enum.name}*/ value from a /*{projected_type}*/
//! @relates /*{projected_type}*/
OPENXR_HPP_INLINE OPENXR_HPP_CONSTEXPR /*{enum.name}*/ get(/*{projected_type}*/ const& v) {
    return static_cast</*{enum.name}*/>(v);
}

//! @brief Free function for retrieving the string name of a /*{projected_type}*/ value as a const char *
//! @relates /*{projected_type}*/
OPENXR_HPP_INLINE OPENXR_HPP_SWITCH_CONSTEXPR const char* to_string_literal(/*{projected_type}*/ value) {
    switch (value) {
//#     for val in enum.values
        /*{ protect_begin(val, enum) }*/
//#         set valname = create_enum_value(val.name, enum.name)
        case /*{projected_type -}*/ ::/*{- valname }*/:
            return /*{ valname | quote_string }*/;
            /*{ protect_end(val, enum) }*/
//#     endfor
        default:
            return "invalid";
    }
}

//! @brief Free function for retrieving the string name of a /*{projected_type}*/ value as a std::string
//! @relates /*{projected_type}*/
OPENXR_HPP_INLINE OPENXR_HPP_SWITCH_CONSTEXPR std::string to_string(/*{projected_type}*/ value) {
    return {to_string_literal(value)};
}

/*{ protect_end(enum) }*/
//# endfor

//! @}


/*!
 * @defgroup result_helpers Result helper free functions
 * @{
 */
//! @brief Return true if the Result is negative, indicating a failure.
//! Equivalent of XR_FAILED()
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool failed(Result v) { return static_cast<int>(v) < 0; }

//! @brief Return true if the result is non-negative, indicating a success or non-error result.
//! Equivalent of XR_SUCCEEDED()
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool succeeded(Result v) { return static_cast<int>(v) >= 0; }

//! @brief Return true if the result is exactly equal to Result::Success.
//! Equivalent of XR_UNQUALIFIED_SUCCESS()
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool unqualifiedSuccess(Result v) { return v == Result::Success; }
//! @}

//# for op in ('<', '>', '<=', '>=', '==', '!=')
//! @brief `/*{op}*/` comparison between Result and integer, for compatibility with the XR_ function-type macros and XrResult.
//! @relates Result
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool operator/*{- op -}*/(Result lhs, int rhs) { return get(lhs) /*{- op -}*/ rhs; }

//! @brief `/*{op}*/` comparison between integer and Result, for compatibility with the XR_ function-type macros and XrResult.
//! @relates Result
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool operator/*{- op -}*/(int lhs, Result rhs) { return lhs /*{- op -}*/ get(rhs); }

//# endfor

}  // namespace OPENXR_HPP_NAMESPACE

/*{ include_guard_end() }*/
