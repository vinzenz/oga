#
# Copyright 2014-2016 Vinzenz Feenstra, Red Hat, Inc. and/or its affiliates.
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
SUBDIRS=type_traits

noinst_LTLIBRARIES = liboga-util.la

liboga_util_la_CXXFLAGS=$(AM_CXXFLAGS) $(OGA_CXX_FLAGS)
liboga_util_la_LDFLAGS=$(AM_LDFLAGS) $(OGA_LDFLAGS)
liboga_util_la_SOURCES=\
	encoding.cpp \
	registry.cpp \
	wmiclient.cpp \
	$(NULL)


liboga_util_la_SOURCES+=\
	enable_if.hpp \
	encoding.hpp \
	registry.hpp \
	shared_ptr.hpp \
	split.hpp \
	string_appender.hpp \
	strip.hpp \
	wmiclient.hpp \
	$(NULL)
