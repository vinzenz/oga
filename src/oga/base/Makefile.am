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

noinst_LTLIBRARIES = liboga-base.la

liboga_base_la_CXXFLAGS=$(AM_CXXFLAGS) $(OGA_CXX_FLAGS)
liboga_base_la_SOURCES=\
	detail/process_posix.cpp \
	detail/process_windows.cpp \
	detail/thread_posix.cpp \
	detail/thread_windows.cpp \
	errors.cpp \
	logging.cpp \
	logging_appender.cpp \
	logging_formatter.cpp \
	logging_logger.cpp \
	timer_queue.cpp \
	$(NULL)


liboga_base_la_SOURCES+=\
	detail/process.hpp \
	detail/thread.hpp \
	error_def.hpp \
	errors.hpp \
	logging.hpp \
	logging_appender.hpp \
	process.hpp \
	thread.hpp \
	timer_queue.hpp \
	types.hpp \
	$(NULL)
