#pragma once

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <boost/stacktrace.hpp>

namespace boost {
    inline void assertion_failed_msg(char const* expr, char const* msg, char const* function, 
                                     char const* file, long line) {
        std::cerr << file << ":" << line << " Expression '" << expr << "' is false in function '" 
                  << function << "': " << (msg ? msg : "<...>") << ".\n"
                  << "Backtrace:\n" << boost::stacktrace::stacktrace() << '\n';

        std::abort();
    }

    inline void assertion_failed(char const* expr, char const* function, char const* file, 
                                 long line) {
        ::boost::assertion_failed_msg(expr, 0 /*nullptr*/, function, file, line);
    }
} // namespace boost
