/**
 * @file logging.h 
 * Support header for logging functions
 *
 * Inspired by https://github.com/gklingler/simpleLogger
 * 
 * @copyright Copyright 2015 Anthony Tibbs.
 * This project is released under the GNU General Public License.
*/

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <boost/log/trivial.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include "utility.h" // for "whitespace" manipualtor

// the logs are also written to LOGFILE
#define LOGFILE "mathtext.log"

// just log messages with severity >= SEVERITY_THRESHOLD are written
#define SEVERITY_THRESHOLD logging::trivial::trace

// register a global logger
BOOST_LOG_GLOBAL_LOGGER(logger, boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>)

// to allow for easier-to-read logs, we automatically insert 2 spaces for 
// every level of indentation that the application has indicated.  These
// functions are used to increase/decrease the indentation level.
void logIncreaseIndent (void);
void logDecreaseIndent (void);
void logResetIndent (void);
extern int logIndentLevel;

// just a helper macro used by the macros below - don't use it in your code
#define BLOG(severity) BOOST_LOG_SEV(logger::get(),boost::log::trivial::severity)

#define LOG(severity) BLOG(severity) << Whitespace(logIndentLevel)

// ===== log macros =====
#define LOG_TRACE   LOG(trace)
#define LOG_DEBUG   LOG(debug)
#define LOG_INFO    LOG(info)
#define LOG_WARNING LOG(warning)
#define LOG_ERROR   LOG(error)
#define LOG_FATAL   LOG(fatal)


#endif /* __LOGGING_H__ */
