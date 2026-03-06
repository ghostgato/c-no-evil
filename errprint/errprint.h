#ifndef ERRPRINT_H
#define ERRPRINT_H

// Message ANSI Color codes
#define ERR   "\033[91m"    // Error:   Red
#define WARN  "\33[93m"    // Warning: Yellow
#define INFO  "\033[94m"    // Info:    Blue
#define USAGE "\033[1;36m"  // Usage:   Cyan bold
#define RESET "\033[0m"     // RESET ANSI

/**
 * @brief Prints a colored message to stderr. Operates like printf() and is 
 *        able to substitute variadic arguments into placeholders ('%').
 *
 * @param p_color ANSI color code prefix to use (macro defines in header file)
 * @param p_msg format string 
 * @param ... arguments linked to placeholders in format string
 */
void errprint (const char *p_color, const char *p_msg, ...)
    __attribute__((format(printf, 2, 3)));

#endif
