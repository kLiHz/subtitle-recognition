//
// Created by henry on 2023/5/20.
//

#include "srt.h"

std::string msToTimeStr(long long t) {
    auto ms = t % 1000;
    auto s = t / 1000;
    auto m = s / 60;
    auto h = m / 60;
    m %= 60;
    s %= 60;
    return fmt::format("{:0>2}:{:0>2}:{:0>2},{:0>3}", h, m, s, ms);
}

std::string composeSrtItem(long long i, long long start_ms, long long end_ms, const std::string & text) {
    return fmt::format(
        "{}\n"
        "{} --> {}\n"
        "{}\n"
        , i
        , msToTimeStr(start_ms), msToTimeStr(end_ms)
        , text
    );
}
