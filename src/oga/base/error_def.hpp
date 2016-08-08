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

#ifndef GUARD_OGA_BASE_ERROR_DEF_HPP_INCLUDED
#define GUARD_OGA_BASE_ERROR_DEF_HPP_INCLUDED

namespace oga {

// When adding an error code, add a corresponding error message!
enum application_errors {
    kAppErrSuccess = 0,
    kAppErrInvalidMessage,
    kAppErrInvalidMessageFormat,
    kAppErrInvalidParameter,
    kAppErrInvalidNumberOfArguments,
    kAppErrInvalidHandle,
    kAppErrNotImplemented,
    kAppErrRequestedDataNotProvided,
    kAppErrNoChanges,
    kAppErrUnexpectedType,

    // This should be always the last one
    kAppErr_COUNT
};

static char const * const application_error_messages[kAppErr_COUNT] = {
    "Operation completed successfully",
    "Couldn't parse a message received",
    "The message received had an invalid format",
    "An invalid paramter has been passed",
    "An invalid number of arguments have been passed.",
    "An invalid handle has been used.",
    "The function used has not been implemented.",
    "The data provider does not provide the requested data.",
    "Data has not been changed",
    "An unexpected data type has been encountered."
};


}

#endif //GUARD_OGA_BASE_ERROR_DEF_HPP_INCLUDED
