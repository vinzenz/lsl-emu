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
#ifndef GUARD_SQLITE_QUERY_HPP_INCLUDED
#define GUARD_SQLITE_QUERY_HPP_INCLUDED

#include <boost/shared_ptr.hpp>
#include <sqlite/command.hpp>
#include <sqlite/result.hpp>

namespace sqlite{

    /** \brief query should be used to execute SQL queries
      * An object of this class is not copyable
      */
    struct query : command {
        /** \brief constructor
          * \param con reference to the connection which should be used
          * \param sql is the SQL query statement
          */
        query(connection & con, std::string const & sql);

        /** \brief destructor
          *
          */
        virtual ~query();

        /** \brief executes the sql command
          * \return result_type which is boost::shared_ptr<result>
          */
        result_type emit_result();

        /** \brief returns the results (needs a previous emit() call)
          * \return result_type which is boost::shared_ptr<result>
          */
        result_type get_result();
    private:
        friend struct result;
        void access_check();
        bool step();
    };
}

#endif //GUARD_SQLITE_QUERY_HPP_INCLUDED

