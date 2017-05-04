#ifndef GUARD_SQLITE_BACKUP_HPP_INCLUDED
#define GUARD_SQLITE_BACKUP_HPP_INCLUDED

#include <boost/noncopyable.hpp>
#include <sqlite/connection.hpp>

struct sqlite3_backup;

namespace sqlite {
    /** \brief \a backup is a class for representing SQLite backup operations
      * An object of this class is not copyable
      */
    struct backup: boost::noncopyable {
        /** \brief \a backup constructor
          * \param conn_to takes a reference to the \a connection object
          *        of the destination database
          * \param conn_from takes a reference to the \a connection object
          *        of the source database
          */
        backup(connection& conn_to, connection& conn_from);

        /** \brief \a backup destructor. The backup operation is automatically
          *        finished after the object destructed.
          */
        ~backup();


        /** \brief Do a backup step
          * \param nPage the number of pages to backup in this step. If a
          *        negative integer is given, all pages are backuped. If this
          *        parameter is omitted, -1 is used.
          */
        bool step(int nPage = -1);

        /** \brief Finish the backup operation
          *        This is used for flushing current backup operation. After
          *        this call, the backup object should not be used anymore.
          */
        void finish();

    private:
        sqlite3* get_to_handle() const;
    private:
        sqlite3_backup* m_pBackup;
        connection& m_conn_to;
    };
}
#endif // #ifndef GUARD_SQLITE_BACKUP_HPP_INCLUDED
