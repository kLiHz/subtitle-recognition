//
// Created by henry on 2023/5/20.
//

#ifndef SUBREC_SRT_H
#define SUBREC_SRT_H

#include <string>
#include "fmt/core.h"

std::string msToTimeStr(long long t);

std::string composeSrtItem(long long i, long long start_ms, long long end_ms, const std::string & text);

#endif //SUBREC_SRT_H
