/**
 * @file logging.cpp
 * Support functions for debug logging
 * 
 * Inspired by https://github.com/gklingler/simpleLogger
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <boost/log/core/core.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <ostream>

#include "logging.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;

/*
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
*/
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logging::trivial::severity_level)

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, src::severity_logger_mt) {
  src::severity_logger_mt<boost::log::trivial::severity_level> logger;

  // add a text sink
  typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
  boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

  // add a logfile stream to our sink
  sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(LOGFILE));

  // add "console" output stream to our sink
  sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));

  // specify the format of the log message
  logging::formatter formatter = expr::stream << expr::smessage;

  // only messages with severity >= SEVERITY_THRESHOLD are written
  sink->set_filter(severity >= SEVERITY_THRESHOLD);

  // "register" our sink
  logging::core::get()->add_sink(sink);

  return logger;
}
