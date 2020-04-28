#pragma once

#define TEXT_HIDDEN  "\e[8m"
#define TEXT_VISIBLE "\e[28m"

#define TEXT_RESET "\033[0m"

#define TEXT_BOLD "\033[1m"

#define TEXT_BLACK   "\033[30m"
#define TEXT_RED     "\033[31m"
#define TEXT_GREEN   "\033[32m"
#define TEXT_YELLOW  "\033[33m"
#define TEXT_BLUE    "\033[34m"
#define TEXT_MAGENTA "\033[35m"
#define TEXT_CYAN    "\033[36m"
#define TEXT_WHITE   "\033[37m"

#ifndef NCOLOR

#define MSG_ERROR_(s) TEXT_BOLD TEXT_RED s TEXT_RESET
#define MSG_INFO_(s) TEXT_BOLD TEXT_GREEN s TEXT_RESET
#define MSG_PROMPT_(s) TEXT_BOLD TEXT_YELLOW s TEXT_RESET
#define MSG_STATUS_(s) TEXT_BOLD TEXT_YELLOW s TEXT_RESET

#else // NCOLOR

#define MSG_ERROR_(s) s
#define MSG_INFO_(s) s
#define MSG_PROMPT_(s) s
#define MSG_STATUS_(s) s

#endif

#define MSG(s) s "\n"

#define MSG_ERROR(s) MSG(MSG_ERROR_(s))
#define MSG_INFO(s) MSG(MSG_INFO_(s))
#define MSG_PROMPT(s) MSG(MSG_PROMPT_(s))
#define MSG_STATUS(s) MSG(MSG_STATUS_(s))
