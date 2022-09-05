/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "time_str.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

std::string now_str() {
    time_t     now      = time(nullptr);
    struct tm *timeinfo = localtime(&now);

    std::ostringstream sstr;
    sstr << std::setfill('0') << std::right;
    sstr << std::setw(4) << timeinfo->tm_year + 1900 << '-';
    sstr << std::setw(2) << timeinfo->tm_mon + 1 << '-';
    sstr << std::setw(2) << timeinfo->tm_mday + 1 << '_';
    sstr << std::setw(2) << timeinfo->tm_hour << ":";
    sstr << std::setw(2) << timeinfo->tm_min << ":";
    sstr << std::setw(2) << timeinfo->tm_sec;

    return sstr.str();
}
