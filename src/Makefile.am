#
# Copyright 2014 Vinzenz Feenstra, Red Hat, Inc. and/or its affiliates.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Refer to the README and COPYING files for full details of the license.
#

SUBDIRS=oga

noinst_PROGRAMS=oga2

oga2_SOURCES=main.cpp
oga2_CXXFLAGS=$(AM_CXXFLAGS) $(OGA_CXX_FLAGS)
oga2_LDFLAGS=$(AM_LDFLAGS) $(OGA_LDFLAGS)
oga2_LDADD= \
	oga/base/liboga-base.la \
	oga/core/liboga-core.la \
	oga/comm/liboga-comm.la \
	oga/proto/config/liboga-proto-config.la \
	oga/proto/json/liboga-proto-json.la \
	oga/util/liboga-util.la \
	$(NULL)
