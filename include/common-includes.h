/**
 * @file common-includes.h
 * Header file (precompiled for speed) containing generally used
 * standard includes and Boost includes from across the project.
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __COMMON_INCLUDES_H__
#define __COMMON_INCLUDES_H__

#ifdef _MSC_VER
#pragma message("Compiling precompiled headers.\n")
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <assert.h>
#include <errno.h>

#include <cctype>
#include <iostream>
#include <fstream>
#include <map>
#include <ostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/make_shared.hpp>
#include <boost/regex.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>


#endif // __COMMON_INCLUDES_H__
