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

noinst_LTLIBRARIES = liboga-proto-json.la

liboga_proto_json_la_CXXFLAGS=$(AM_CXXFLAGS) $(OGA_CXX_FLAGS)
liboga_proto_json_la_SOURCES=\
	json.cpp \
	json_generator.cpp \
	json_parser.cpp \
	$(NULL)

liboga_proto_json_la_SOURCES+=\
	json.hpp \
	json_generator.hpp \
	json_parser.hpp \
	$(NULL)

noinst_PROGRAMS=json_test

json_test_SOURCES=main.cpp
json_test_CXXFLAGS=$(AM_CXXFLAGS) $(OGA_CXX_FLAGS)
json_test_LDADD=liboga-proto-json.la
