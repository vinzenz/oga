//
// Copyright 2014 Vinzenz Feenstra, Red Hat, Inc. and/or its affiliates.
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

#ifndef GUARD_OGA_UTIL_ENABLE_IF_HPP_INCLUDED
#define GUARD_OGA_UTIL_ENABLE_IF_HPP_INCLUDED

namespace oga {
namespace util {

template<
    bool B,
    typename T = void
>
struct enable_if {
};

template<
    typename T
>
struct enable_if<true, T> {
    typedef T type;
};

}}

#endif // GUARD_OGA_UTIL_ENABLE_IF_HPP_INCLUDED
