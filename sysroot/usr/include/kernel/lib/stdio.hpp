/**
 * @file stdio.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-07-09
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2020
 * 
 */
#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <lib/mutex.hpp>

#ifndef EOF
#define EOF (-1)
#endif

typedef int (*fnptr_t)(unsigned c, void** helper);

int px_do_printf(const char* fmt, va_list args, fnptr_t fn, void* ptr);

/**
 * @brief Sends formatted output to a string using an argument list.
 * 
 * @param buf Pointer to a buffer where the result is stored
 * @param fmt C string that contains a format string
 * @param args A value identifying a variable arguments list
 * @return int The total number of characters written.
 * The number of characters not written if negative.
 */
int px_kvsprintf(char* buf, const char* fmt, va_list args);
/**
 * @brief Sends formatted output to a string.
 * 
 * @param buf Pointer to a buffer where the result is stored
 * @param fmt C string that contains a format string
 * @param ... Sequence of additional arguments
 * @return int The total number of characters written.
 * The number of characters not written if negative.
 */
int px_ksprintf(char* buf, const char* fmt, ...);
/**
 * @brief Sends formatted output to stdout using an argument list.
 * 
 * @param fmt C string that contains a format string
 * @param args A value identifying a variable arguments list
 * @return int The total number of characters written.
 * The number of characters not written if negative.
 */
int px_kvprintf(const char* fmt, va_list args);
/**
 * @brief Sends formatted output to stdout.
 * 
 * @param fmt C string that contains a format string
 * @param ... Sequence of additional arguments
 * @return int The total number of characters written.
 * The number of characters not written if negative.
 */
int px_kprintf(const char* fmt, ...);
/**
 * @brief Prints a single character to the kernel display.
 *
 * @param c Character to be printed.
 */
int putchar(char c);
int putchar_unlocked(char c);
extern px_mutex_t put_mutex;
/**
 * @brief Prints a given string to the kernel display.
 * 
 * @param str String to be printed.
 */
int puts(const char *str);
/**
 * @brief Prints a statement to serial debugger if the kernel
 * is built with the debug flag defined. Max message size is
 * 1024 (including null terminator).
 * @param fmt Formatted C string
 * @param ... Sequence of additional arguments
 * @return int The total number of characters written.
 */
int px_debugf(const char* fmt, ...);
