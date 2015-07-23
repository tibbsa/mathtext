/**
 * @file logging.cpp
 * Support functions for debug logging
 * 
 * Inspired by https://github.com/gklingler/simpleLogger
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
#include "logging.h"

int logIndentLevel = 0;
void logIncreaseIndent (void)
{
  logIndentLevel++;
}

void logDecreaseIndent (void)
{
  assert (logIndentLevel != 0);
  logIndentLevel--;
}

void logResetIndent (void)
{
  logIndentLevel = 0;
}

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logging::trivial::severity_level)

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, src::severity_logger_mt) {
  src::severity_logger_mt<boost::log::trivial::severity_level> logger;

  // add a text sink
  typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
  boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

  // add a logfile stream to our sink
  sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(LOGFILE));

  // add "console" output stream to our sink
  /*
#ifndef MT_NO_CONSOLE_LOG
  sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
#endif
  */

  // specify the format of the log message
  logging::formatter formatter = expr::stream << expr::smessage;

  // only messages with severity >= SEVERITY_THRESHOLD are written
  sink->set_filter(severity >= SEVERITY_THRESHOLD);

  // "register" our sink
  logging::core::get()->add_sink(sink);

  return logger;
}


