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

#include <oga/core/providers/detail/applications.hpp>

#if defined(_WIN32)
namespace oga {
namespace core {
namespace providers {
namespace detail {

class applications_windows : public applications {
public:
    applications_windows(oga::log::logger_ptr logger);
    bool source_modified() const;
    virtual oga::error_type refresh(std::set<std::string> & apps);
    virtual oga::error_type configure(oga::proto::config::object const & cfg);
};

}}}}
#endif
