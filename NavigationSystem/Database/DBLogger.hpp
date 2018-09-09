/**
 * @file    DBLogger.hpp
 *
 * @brief   Logs dataLogs to the database in a efficient manor and offloads the work to a worker thread.
 *
 */

#ifndef DBLOGGER_HPP
#define DBLOGGER_HPP

#include "DBHandler.hpp"
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class DBLogger {
   public:
    DBLogger(unsigned int LogBufferSize, DBHandler& dbHandler);
    ~DBLogger();

    void startWorkerThread();

    void log(LogItem& item);

    unsigned int bufferSize() { return m_bufferSize; }

   private:
    template <typename FloatOrDouble>
    FloatOrDouble setValue(FloatOrDouble value);

    static void workerThread(DBLogger* ptr);

    std::thread* m_thread;
    std::atomic<bool> m_working;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    DBHandler& m_dbHandler;
    unsigned int m_bufferSize;
    std::vector<LogItem>* m_logBufferFront;
    std::vector<LogItem>* m_logBufferBack;
};

#endif /* DBLOGGER_HPP */
