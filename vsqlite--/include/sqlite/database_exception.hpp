/*##############################################################################
 VSQLite++ - virtuosic bytes SQLite3 C++ wrapper

 Copyright (c) 2006-2014 Vinzenz Feenstra vinzenz.feenstra@gmail.com
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of virtuosic bytes nor the names of its contributors may
   be used to endorse or promote products derived from this software without
   specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

##############################################################################*/
#ifndef GUARD_SQLITE_DATABASE_EXCEPTION_HPP_INCLUDED
#define GUARD_SQLITE_DATABASE_EXCEPTION_HPP_INCLUDED

#include <stdexcept>
#include <string>

namespace sqlite{
    struct database_exception : public std::runtime_error {
        database_exception(std::string const & msg)
        : std::runtime_error(msg.c_str())
        {}
    };

    struct database_exception_code : database_exception {
        database_exception_code(std::string const & error_message,
                                int sqlite_error_code)
        : database_exception(error_message)
        , sqlite_error_code_(sqlite_error_code)
        {}

        int error_code() const {
            return sqlite_error_code_;
        }
    protected:
        int const sqlite_error_code_;
    };

    struct buffer_too_small_exception : public std::runtime_error{
        buffer_too_small_exception(std::string const & msg)
            : std::runtime_error(msg.c_str()){}
    };

    struct database_misuse_exception : public std::logic_error{
        database_misuse_exception(std::string const & msg)
        : std::logic_error(msg)
        {}
    };

    struct database_misuse_exception_code : database_misuse_exception {
        database_misuse_exception_code(std::string const & msg,
                                       int sqlite_error_code)
        : database_misuse_exception(msg)
        , sqlite_error_code_(sqlite_error_code)
        {}

        int error_code() const {
            return sqlite_error_code_;
        }
    protected:
        int const sqlite_error_code_;
    };

    struct database_system_error : database_exception {
        database_system_error(std::string const & msg,
                              int error_code)
        : database_exception(msg)
        , error_code_(error_code)
        {}

        int error_code() const {
            return error_code_;
        }
    protected:
        int error_code_;
    };
}

#endif //GUARD_SQLITE_DATABASE_EXCEPTION_HPP_INCLUDED
