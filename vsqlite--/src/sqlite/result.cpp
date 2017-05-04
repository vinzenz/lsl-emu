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
#include <sqlite/private/result_construct_params_private.hpp>
#include <sqlite/database_exception.hpp>
#include <sqlite/result.hpp>
#include <sqlite/query.hpp>
#include <sqlite3.h>
#include <boost/make_shared.hpp>
#include <limits>

namespace sqlite{
    namespace detail {
        bool end(result_construct_params_private const & params) {
            return params.ended;
        }

        void reset(result_construct_params_private & params) {
            params.ended = false;
        }
    }

    result::result(construct_params p)
    : m_params(p) {
        m_params->access_check();
        m_columns = sqlite3_column_count(m_params->statement);
        m_row_count = m_params->row_count;
    }

    result::~result(){
    }

    bool result::next_row(){
        if(!m_params->ended) {
            m_params->ended = !m_params->step();
            return !end();
        }
        return false;
    }

    std::string result::get_column_decltype(int idx) {
        access_check(idx);
        return sqlite3_column_decltype(m_params->statement,idx);
    }

    type result::get_column_type(int idx) {
        access_check(idx);
        switch(sqlite3_column_type(m_params->statement,idx)) {
            case SQLITE_BLOB:
                return  sqlite::blob;
            case SQLITE_NULL:
                return sqlite::null;
            case SQLITE_FLOAT:
                return sqlite::real;
            case SQLITE_INTEGER:
                return sqlite::integer;
            case SQLITE_TEXT:
                return sqlite::text;
            default:
                break;
        }
        return sqlite::unknown;
    }

    variant_t result::get_variant(int idx) {
        variant_t v;
        switch( get_column_type(idx) ) {
            case sqlite::integer:
                {
                    boost::int64_t i = get_int64(idx);
                    if( i > std::numeric_limits<int>::max()
                        || i < std::numeric_limits<int>::min() ) {
                        v = i;
                    }
                    else {
                        v = int(i);
                    }
                }
                break;
            case sqlite::blob:
                v = boost::make_shared<blob_t>();
                get_binary(idx, *boost::get<blob_ref_t>(v));
                break;
            case sqlite::real:
                {
                    long double x = get_double(idx);
                    v = x;
                }
                break;
            case sqlite::null:
                v = null_t();
                break;
            default:
            case sqlite::text:
                v = get_string(idx);
                break;
        }
        return v;
    }

    int result::get_int(int idx){
        access_check(idx);
        if(sqlite3_column_type(m_params->statement,idx) == SQLITE_NULL)
            return 0;
        return sqlite3_column_int(m_params->statement,idx);
    }

    boost::int64_t result::get_int64(int idx){
        access_check(idx);
        if(sqlite3_column_type(m_params->statement,idx) == SQLITE_NULL)
            return 0;
        return sqlite3_column_int64(m_params->statement,idx);
    }

    std::string result::get_string(int idx){
        access_check(idx);
        if(sqlite3_column_type(m_params->statement,idx) == SQLITE_NULL)
            return std::string("NULL");
        char const * v = reinterpret_cast<char const*>
                         (sqlite3_column_text(m_params->statement,idx));
        size_t length = get_binary_size(idx);
        return std::string(v, v+length);
    }

    double result::get_double(int idx){
        access_check(idx);
        if(sqlite3_column_type(m_params->statement,idx) == SQLITE_NULL)
            return 0.0;
        return sqlite3_column_double(m_params->statement,idx);
    }

    size_t result::get_binary_size(int idx){
        access_check(idx);
        if(sqlite3_column_type(m_params->statement,idx) == SQLITE_NULL)
            return 0;
        return sqlite3_column_bytes(m_params->statement,idx);
    }

    void result::get_binary(int idx, void * buf, size_t buf_size){
        access_check(idx);
        if(sqlite3_column_type(m_params->statement,idx) == SQLITE_NULL)
            return;
        size_t size = sqlite3_column_bytes(m_params->statement,idx);
        if(size > buf_size)
            throw buffer_too_small_exception("buffer too small");
        memcpy(buf,sqlite3_column_blob(m_params->statement,idx),size);
    }

    void result::get_binary(int idx, std::vector<unsigned char> & v){
        access_check(idx);
        if(sqlite3_column_type(m_params->statement,idx) == SQLITE_NULL)
            return;
        size_t size = sqlite3_column_bytes(m_params->statement,idx);
        v.resize(size,0);
        memcpy(&v[0],sqlite3_column_blob(m_params->statement,idx),size);
    }

    std::string result::get_column_name(int idx){
        access_check(idx);
        return sqlite3_column_name(m_params->statement,idx);
    }

    void result::access_check(int idx){
        m_params->access_check();
        if(idx < 0 || idx >= m_columns)
            throw std::out_of_range("no such column index");
    }

    int result::get_row_count(){
        return m_params->row_count;
    }

    int result::get_column_count(){
        return m_columns;
    }
}

