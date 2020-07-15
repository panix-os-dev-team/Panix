/**
 * @file px_tty.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief TTY is a small library to print unformatted strings to
 * the BIOS TTY. The important thing to keep in mind is that these
 * functions expect a null-terminator at the end of the string, which
 * C++ seems to take care of *most* of the time. These functions do
 * NOT accept formatted strings like printf. That is available in
 * px_kprintf().
 * @version 0.3
 * @date 2019-09-26
 *
 * @copyright Copyright Keeton Feavel (c) 2019
 *
 */
#ifndef PANIX_px_kprint_HPP
#define PANIX_px_kprint_HPP

#include <sys/panix.hpp>
#include <arch/arch.hpp>

enum px_tty_vga_color {
    VGA_Black           = 0x0,
    VGA_Blue            = 0x1,
    VGA_Green           = 0x2,
    VGA_Cyan            = 0x3,
    VGA_Red             = 0x4,
    VGA_Magenta         = 0x5,
    VGA_Brown           = 0x6,
    VGA_LightGrey       = 0x7,
    VGA_DarkGrey        = 0x8,
    VGA_LightBlue       = 0x9,
    VGA_LightGreen      = 0xA,
    VGA_LightCyan       = 0xB,
    VGA_LightRed        = 0xC,
    VGA_LightMagenta    = 0xD,
    VGA_Yellow          = 0xE,
    VGA_White           = 0xF
};

// Add 10 to these to convert from
// foreground colors to background
enum px_tty_ansi_color {
    ANSI_Black          = 0x1E,
    ANSI_Red            = 0x1F,
    ANSI_Green          = 0x20,
    ANSI_Yellow         = 0x21,
    ANSI_Blue           = 0x22,
    ANSI_Magenta        = 0x23,
    ANSI_Cyan           = 0x24,
    ANSI_White          = 0x25,
    ANSI_BrightBlack    = 0x5A,
    ANSI_BrightRed      = 0x5B,
    ANSI_BrightGreen    = 0x5C,
    ANSI_BrightYellow   = 0x5D,
    ANSI_BrightBlue     = 0x5E,
    ANSI_BrightMagenta  = 0x5F,
    ANSI_BrightCyan     = 0x60,
    ANSI_BrightWhite    = 0x61
};

enum px_print_level {
    Info            = 0,
    Warning         = 1,
    Error           = 2,
    Success         = 3
};

/**
 * @brief Prints a single character to the kernel display.
 *
 * @param c Character to be printed.
 */
void putchar(char c);
/**
 * @brief Prints a debug message to the kernel display and
 * sets a tag and color according to the debug level.
 *
 * @param msg Message to be printed
 * @param lvl Message debug level
 */
void px_print_debug(char* msg, px_print_level lvl);
/**
 * @brief Prints out a string in a specified color
 *
 * @param str Input string to be printed
 * @param color Text color
 */
void px_kprint_color(char* str, px_tty_vga_color color);
/**
 * @brief Set the color of the text when the next print is called.
 *
 * @param fore Foreground color
 * @param back Background color
 */
void px_tty_set_color(px_tty_vga_color fore, px_tty_vga_color back);
/**
 * @brief Clears the TTY and resets the cursor position.
 *
 */
void px_clear_tty();
/**
 * @brief Sets the indicator in the top right corner.
 * Used mostly for debugging interrupts.
 *
 * @param color Indicator color
 */
void px_set_indicator(px_tty_vga_color color);

#endif /* PANIX_px_kprint_HPP */