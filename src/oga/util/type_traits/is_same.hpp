//
// Copyright 2014-2016 Vinzenz Feenstra, Red Hat, Inc. and/or its affiliates.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Refer to the README and COPYING files for full details of the license.
//

#ifndef GUARD_OGA_UTIL_TYPE_TRAITS_IS_SAME_HPP_INCLUDED
#define GUARD_OGA_UTIL_TYPE_TRAITS_IS_SAME_HPP_INCLUDED

#include <oga/util/type_traits/boolean.hpp>

namespace oga {
namespace util {
namespace type_traits {

template<
    typename T,
    typename U
> struct is_same
: oga::util::type_traits::false_type
{
};

template<
    typename T
>
struct is_same<T, T>
: oga::util::type_traits::true_type
{
};



}}}

#endif // GUARD_OGA_UTIL_TYPE_TRAITS_IS_SAME_HPP_INCLUDED
