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

//# macro twocallbody(enhanced, exceptions_allowed)
    uint32_t /*{ enhanced.count_output_param_name }*/ = 0;
    uint32_t /*{ enhanced.capacity_input_param_name }*/ = 0;

    /*{ enhanced.pre_statements |join("\n") | indent }*/
    //# if enhanced.item_type == 'char'
    std::basic_string<char, std::char_traits<char>, Allocator> str{vectorAllocator};
    //# endif
    /*{ enhanced.get_main_invoke(replacements={enhanced.array_param_name: "nullptr"}) }*/
    if (!unqualifiedSuccess(result) || /*{ enhanced.count_output_param_name }*/ == 0) {
        /*{ make_error_handling(enhanced, exceptions_allowed) }*/
        /*{ enhanced.return_statement }*/
    }
    do {
        /*{ enhanced.array_param_name }*/.resize(/*{ enhanced.count_output_param_name }*/);
        /*{ enhanced.capacity_input_param_name }*/ = static_cast<uint32_t>(/*{enhanced.array_param_name}*/.size());
        //# set raw_array_param = enhanced.array_param_name + ".data()"
        //# set array_param = "reinterpret_cast<" + enhanced.array_param.param.type + "*>(" + raw_array_param + ")"
        /*{ enhanced.get_main_invoke(replacements={ enhanced.array_param_name: array_param }) | replace("Result ", "") }*/
    } while (result == xr::Result::ErrorSizeInsufficient);
    if (succeeded(result)) {
        OPENXR_HPP_ASSERT(/*{ enhanced.count_output_param_name }*/ <= /*{enhanced.array_param_name}*/.size());
        /*{enhanced.array_param_name}*/.resize(/*{ enhanced.count_output_param_name }*/);
    } else /*{enhanced.array_param_name}*/.clear();
    /*{ enhanced.post_statements |join("\n") | indent }*/
    //# if enhanced.item_type == 'char'
    str.assign(/*{ enhanced.array_param_name }*/.begin(), /*{ enhanced.array_param_name }*/.end());
    //# endif
    /*{ make_error_handling(enhanced, exceptions_allowed) }*/
    /*{ enhanced.return_statement }*/
//# endmacro

//# macro make_two_call(enhanced, exceptions_allowed)
template </*{ enhanced.template_defns }*/>
OPENXR_HPP_INLINE /*{enhanced.return_type}*/ /*{enhanced.qualified_name}*/ (
    /*{ enhanced.get_definition_params() | join(", ")}*/) /*{enhanced.qualifiers}*/ {
    /*{ enhanced.vec_type }*/ /*{ enhanced.array_param_name }*/;
    /*{ twocallbody(enhanced, exceptions_allowed) |replace('vectorAllocator', '{}') }*/
}

template </*{ enhanced.template_defns }*/>
OPENXR_HPP_INLINE /*{enhanced.return_type}*/ /*{enhanced.qualified_name}*/ (
    //# set params = enhanced.get_definition_params(extras=["Allocator const& vectorAllocator"])
    /*{ params | join(", ")}*/) /*{enhanced.qualifiers}*/ {
    /*{ enhanced.vec_type }*/ /*{ enhanced.array_param_name }*/{vectorAllocator};
    /*{ twocallbody(enhanced, exceptions_allowed) }*/
}
//# endmacro

/*% macro _make_success_predicate(method) -%*/ succeeded(/*{method.result_name}*/) /*%- endmacro %*/

//# macro _make_error_handling_no_exceptions(method)
    OPENXR_HPP_ASSERT( /*{_make_success_predicate(method)}*/ );
//# endmacro

//# macro _make_error_handling_exceptions(method)
    if (!(/*{_make_success_predicate(method)}*/)) {
        exceptions::throwResultException(/*{method.result_name}*/, OPENXR_HPP_NAMESPACE_STRING "::/*{method.qualified_name}*/");
    }
//# endmacro

//# macro _make_error_handling_maybe_exceptions(method)
#ifdef OPENXR_HPP_NO_EXCEPTIONS
    OPENXR_HPP_ASSERT( /*{_make_success_predicate(method)}*/ );
#else
    if (!(/*{_make_success_predicate(method)}*/)) {
        exceptions::throwResultException(/*{method.result_name}*/, OPENXR_HPP_NAMESPACE_STRING "::/*{method.qualified_name}*/");
    }
#endif

//# endmacro

//# macro make_error_handling(enhanced, exceptions_allowed)
//#     if exceptions_allowed == "maybe"
    /*{ _make_error_handling_maybe_exceptions(enhanced) }*/
//#     elif exceptions_allowed
    /*{ _make_error_handling_exceptions(enhanced) }*/
//#     else
    /*{ _make_error_handling_no_exceptions(enhanced) }*/
//#     endif
//# endmacro

//# macro make_enhanced(enhanced, exceptions_allowed)

template </*{ enhanced.template_defns }*/>
OPENXR_HPP_INLINE /*{enhanced.return_type}*/ /*{enhanced.qualified_name}*/ (
    /*{ enhanced.get_definition_params() | join(", ")}*/) /*{enhanced.qualifiers}*/ {
    /*{ enhanced.pre_statements | join("\n") | indent}*/
    /*{ enhanced.get_main_invoke() }*/
    /*{ enhanced.post_statements | join("\n") | indent }*/
    /*{ make_error_handling(enhanced, exceptions_allowed) }*/
    /*{ enhanced.return_statement }*/
}
//# endmacro
