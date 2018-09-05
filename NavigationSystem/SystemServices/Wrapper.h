/*******************************************************************************
 *
 * File:
 *
 *  Wrapper.h
 *
 ******************************************************************************/

#pragma once

/** @todo move to Utility class */

#include "Logger.hpp"

/* "Safe" stoi which does not throw exceptions on bad input values */
static int safe_stoi(const std::string& str, std::size_t* pos = 0, int base = 10) {
    int retvalue = 0;
    try {
        retvalue = std::stoi(str, pos, base);
    } catch (std::invalid_argument& e) {
        Logger::error("%s stoi(): invalid argument (%s)", __PRETTY_FUNCTION__, str);
    } catch (std::out_of_range& e) {
        Logger::error("%s stoi(): value out of range (%s)", __PRETTY_FUNCTION__, str);
    }
    return retvalue;
}
