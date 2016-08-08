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

#ifndef GUARD_OGA_CORE_DATA_PROVIDER_HPP_INCLUDED
#define GUARD_OGA_CORE_DATA_PROVIDER_HPP_INCLUDED

#include <oga/proto/json/json.hpp>
#include <oga/proto/config/config_parser.hpp>
#include <oga/base/logging.hpp>
#include <map>
#include <set>

namespace oga {
namespace core {
    enum protocol_version {
        kProtocolVersionLegacy  = 0,
        kProtocolVersion1       = 1,
        kProtocolVersion2       = 2,        
		kProtocolVersion3		= 3,

		kProtocolVersionCount,
        kProtocolVersionLatest = (kProtocolVersionCount - 1)
    };

    class data_provider {
    public:
        typedef std::map<std::string, std::set<protocol_version> > provides_map_type;

    public:
        data_provider(std::string const & name,
                      oga::log::logger_ptr logger,
                      uint64_t refresh_interval = ~uint64_t(0));
        virtual ~data_provider();

        oga::error_type configure(oga::proto::config::object const & cfg);

        virtual bool needs_refresh() const;
        virtual uint64_t refresh_interval() const;
        virtual oga::error_type refresh();

        std::string const & name() const;
        provides_map_type const & provides() const;

        oga::error_type get(oga::proto::json::object & result,
                            std::string const & name,
                            protocol_version version);
        virtual bool source_modified() const;

    protected:
        void add_provides(std::string const & name,
                          protocol_version version);
        void add_provides_range(std::string const & name,
                                protocol_version least,
                                protocol_version max);

    protected:
        oga::log::logger_ptr logger_;
        uint64_t refresh_interval_;
        std::string name_;
        provides_map_type provides_;
    };
}}

#endif //GUARD_OGA_CORE_DATA_PROVIDER_HPP_INCLUDED
