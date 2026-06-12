/**
 * @fn logger
 * @brief Log a message in std::in.
 * @param level The log level of the message.
 * @param message The message to print.
 * @author Joachim Rey <joachim@meowf.fr>
 *
 * Display a message in std::in given a log level between 0 (lowset)
 * and 3 (highest).
 * The log level of the program is defined at compile time with LOG_LEVEL (eg.
 * -DLOG_LEVEL=2).
 * 0 -> debug
 * 1 -> info
 * 2 -> warn
 * 3 -> error
 * 4 -> fatal
 */
void logger( int level, const char* message );