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

noinst_LTLIBRARIES = liboga-core.la

liboga_core_la_CXXFLAGS=$(AM_CXXFLAGS) $(OGA_CXX_FLAGS)
liboga_core_la_SOURCES=\
	agent.cpp \
	data_provider.cpp \
	detail/config_loader.cpp \
	detail/config_loader_linux.cpp \
	detail/config_loader_windows.cpp \
	logic.cpp \
	providers/applications.cpp \
	providers/detail/applications_linux_apt.cpp \
	providers/detail/applications_linux_rpm.cpp \
	providers/detail/applications_windows.cpp \
	$(NULL)


liboga_core_la_SOURCES+=\
	agent.hpp \
	data_provider.hpp \
	detail/config_loader.hpp \
	logic.hpp \
	providers/applications.hpp \
	providers/detail/applications.hpp \
	providers/detail/applications_linux_apt.hpp \
	providers/detail/applications_linux_rpm.hpp \
	providers/detail/applications_windows.hpp \
	$(NULL)

liboga_core_la_LDFLAGS=$(AM_LDFLAGS) $(OGA_CORE_LDFLAGS)

if OGA_WITH_APT
liboga_core_la_LDFLAGS+=-lapt-pkg
endif

if OGA_WITH_RPM
liboga_core_la_LDFLAGS+=-lrpm
endif
