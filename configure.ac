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

AC_INIT([ovirt-guest-agent],
        [2.0],
        [evilissimo@redhat.com],
        [ovirt-guest-agent],
        [http://github.com/vinzenz/ovirt-guest-agent2])
AC_PREREQ([2.59])

AM_INIT_AUTOMAKE([1.10 -Wall no-define subdir-objects])

AC_PROG_CXX
AC_CONFIG_MACRO_DIR([m4])

m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR

LT_INIT


OGA_CXX_INCLUDES="-I\$(top_srcdir)/src -I\$(top_srcdir)/ext/utf8cpp/source"
OGA_CXX_FLAGS="-std=c++98 -W -Wall -pedantic \$(OGA_CXX_INCLUDES)"
AC_SUBST([OGA_CXX_INCLUDES])
AC_SUBST([OGA_CXX_FLAGS])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/oga/Makefile
    src/oga/base/Makefile
    src/oga/comm/Makefile
    src/oga/proto/Makefile
    src/oga/proto/json/Makefile
])
AC_OUTPUT
