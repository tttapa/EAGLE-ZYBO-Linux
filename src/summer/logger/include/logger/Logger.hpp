/** @file */  // Tell Doxygen to document this file

#pragma once

#include <logger/LogEntryIterator.hpp>

#include <array>    // std::array, size_t
#include <cstring>  // strlen
#include <iosfwd>   // std::ostream forward declaration

/// The maximum size of a LogEntry, in bytes.
constexpr size_t LOGGER_BUFFER_SIZE = 2048;
/// Save the logger buffers on the heap instead of on the stack.
/// Uses the std::vector container instead of std::array.
#define HEAP_LOGGER_BUFFER

#ifdef HEAP_LOGGER_BUFFER
#include <vector>  // std::vector
#endif

class Loggable;

/**
 * @brief   A class for serializing key-value/array data for logging and 
 *          sending to the GUI.
 */
class Logger {
  public:
#ifdef HEAP_LOGGER_BUFFER
    Logger() = default;
    Logger(size_t buffersize) : buffer{std::vector<uint8_t>(buffersize)} {}
#endif

    /**
     * @brief   Add a log element to the log, or update the existing element
     *          with the same identifier.
     * 
     * If the identifier can't be found in the LogEntry, the new element is 
     * append%ed. If an element with the same identifier is found in the 
     * LogEntry, and if the type and size are the same, the existing element is
     * updated with the new value. If the type or size don't match, the log is
     * not altered.
     * 
     * @tparam  T 
     *          The type of the value to log.
     * @param   identifier 
     *          The identifier / key for this element.
     *          The maximum length is 255 characters.
     * @param   data
     *          A pointer to the (array of) value(s) to log.
     * @param   count
     *          The number of elements in the array.
     *          The maximum size is 65535 bytes, so the maximum count is 65535
     *          divided by the size of one element.
     * @retval  true
     *          The element was added to the log successfully, or the existing 
     *          element with the same identifier was updated with the new value. 
     * @retval  false 
     *          The buffer is full, the identifier or data length is too large, 
     *          or the type and size don't match the ones of the existing 
     *          element with the same identifier.
     */
    template <class T>
    bool log(const char *identifier, const T *data, size_t count) {
        LogEntryIterator found = find(identifier);
        if (found != LogEntryIterator::end())
            // if the logentry already contains an element with the same id
            return overwrite(found, data, count);
        else
            // this is a new id
            return append(identifier, data, count);
    }

    /**
     * @brief   Add a log element to the log, or update the existing element
     *          with the same identifier. The data of the element is a single 
     *          value.
     * 
     * If the identifier can't be found in the LogEntry, the new element is 
     * append%ed. If an element with the same identifier is found in the 
     * LogEntry, and if the type and size are the same, the existing element is
     * updated with the new value. If the type or size don't match, the log is
     * not altered.
     * 
     * @tparam  T
     *          The type of the value to log.
     * @param   identifier 
     *          The identifier / key for this element.
     *          The maximum length is 255 characters.
     * @param   value
     *          The value to log.
     *          The maximum size is 65535 bytes.
     * @retval  true
     *          The element was added to the log successfully, or the existing 
     *          element with the same identifier was updated with the new value. 
     * @retval  false 
     *          The buffer is full, the identifier or data length is too large, 
     *          or the type and size don't match the ones of the existing 
     *          element with the same identifier.
     */
    template <class T>
    bool log(const char *identifier, const T &value) {
        return log(identifier, &value, 1);
    }

    /**
     * @brief   Add a log element to the log, or update the existing element
     *          with the same identifier. The data of the element is a 
     *          null-terminated string.
     * 
     * If the identifier can't be found in the LogEntry, the new element is 
     * append%ed. If an element with the same identifier is found in the 
     * LogEntry, and if the type and size are the same, the existing element is
     * updated with the new value. If the type or size don't match, the log is
     * not altered.
     * 
     * @param   identifier 
     *          The identifier / key for this element.
     *          The maximum length is 255 characters.
     * @param   data 
     *          The string to log.
     *          The maximum length is 65535 characters.
     * @retval  true
     *          The element was added to the log successfully, or the existing 
     *          element with the same identifier was updated with the new value. 
     * @retval  false 
     *          The buffer is full, the identifier or data length is too large, 
     *          or the type and size don't match the ones of the existing 
     *          element with the same identifier.
     */
    bool log(const char *identifier, const char *data) {
        return log(identifier, data, strlen(data));
        // TODO: +1? Should I copy NULL terminator?
    }

    /**
     * @brief   Add a log element to the log, or update the existing element
     *          with the same identifier. The data of the element is an array 
     *          of values.
     * 
     * If the identifier can't be found in the LogEntry, the new element is 
     * append%ed. If an element with the same identifier is found in the 
     * LogEntry, and if the type and size are the same, the existing element is
     * updated with the new value. If the type or size don't match, the log is
     * not altered.
     * 
     * @tparam  T
     *          The type of the values to log.
     * @tparam  N
     *          The number of values to log.
     * @param   identifier 
     *          The identifier / key for this element.
     *          The maximum length is 255 characters.
     * @param   array 
     *          The array of values to log.
     *          The maximum size is 65535 bytes.
     * @retval  true
     *          The element was added to the log successfully, or the existing 
     *          element with the same identifier was updated with the new value. 
     * @retval  false 
     *          The buffer is full, the identifier or data length is too large, 
     *          or the type and size don't match the ones of the existing 
     *          element with the same identifier.
     */
    template <class T, size_t N>
    bool log(const char *identifier, const T (&array)[N]) {
        return log(identifier, array, N);
    }

    /**
     * @brief   Add a log element to the log, or update the existing element
     *          with the same identifier. The data of the element is an 
     *          std::array of values.
     * 
     * If the identifier can't be found in the LogEntry, the new element is 
     * append%ed. If an element with the same identifier is found in the 
     * LogEntry, and if the type and size are the same, the existing element is
     * updated with the new value. If the type or size don't match, the log is
     * not altered.
     * 
     * @tparam  T
     *          The type of the values to log.
     * @tparam  N
     *          The number of values to log.
     * @param   identifier 
     *          The identifier / key for this element.
     *          The maximum length is 255 characters.
     * @param   array 
     *          The array of values to log.
     *          The maximum size is 65535 bytes.
     * @retval  true
     *          The element was added to the log successfully, or the existing 
     *          element with the same identifier was updated with the new value. 
     * @retval  false 
     *          The buffer is full, the identifier or data length is too large, 
     *          or the type and size don't match the ones of the existing 
     *          element with the same identifier.
     */
    template <class T, size_t N>
    bool log(const char *identifier, const std::array<T, N> &array) {
        return log(identifier, array.data(), N);
    }

    /**
     * @brief   Add a log element to the log, or update the existing element
     *          with the same identifier. The data of the element is an 
     *          std::vector of values.
     * 
     * If the identifier can't be found in the LogEntry, the new element is 
     * append%ed. If an element with the same identifier is found in the 
     * LogEntry, and if the type and size are the same, the existing element is
     * updated with the new value. If the type or size don't match, the log is
     * not altered.
     * 
     * @tparam  T
     *          The type of the values to log.
     * @param   identifier 
     *          The identifier / key for this element.
     *          The maximum length is 255 characters.
     * @param   vector 
     *          The vector of values to log.
     *          The maximum size is 65535 bytes.
     * @retval  true
     *          The element was added to the log successfully, or the existing 
     *          element with the same identifier was updated with the new value. 
     * @retval  false 
     *          The buffer is full, the identifier or data length is too large, 
     *          or the type and size don't match the ones of the existing 
     *          element with the same identifier.
     */
    template <class T>
    bool log(const char *identifier, const std::vector<T> &vector) {
        return log(identifier, vector.data(), vector.size());
    }

    /**
     * @brief   Log a given Loggable to this Logger.
     * 
     * @param   loggable 
     *          The loggable to log.
     * @retval  true 
     *          The Loggable was added to the log successfully.
     * @retval  false
     *          Otherwise.
     * @see     Loggable::log(Logger &)
     */
    bool log(Loggable &loggable);

    /**
     * @brief   Clear all elements of the log.
     */
    void clear() {
        bufferwritelocation = buffer.data();
        maxLen              = LOGGER_BUFFER_SIZE;
        std::fill(buffer.begin(), buffer.end(), 0);
    }

    /**
     * @brief   Dump the log buffer to the given output stream in a 
     *          human-readable format (offset + hexadecimal + ASCII).
     * 
     * @param   os
     *          The stream to print to.
     */
    void print(std::ostream &os) const;

    /**
     * @brief   Dump the log buffer to the given output stream as a Python bytes
     *          object.
     * 
     * @param   os
     *          The stream to print to.
     */
    void printPython(std::ostream &os);

  private:
    size_t maxLen = LOGGER_BUFFER_SIZE;
#ifdef HEAP_LOGGER_BUFFER
    std::vector<uint8_t> buffer = std::vector<uint8_t>(LOGGER_BUFFER_SIZE);
#else
    std::array<uint8_t, LOGGER_BUFFER_SIZE> buffer = {{}};
#endif
    uint8_t *bufferwritelocation = buffer.data();

    /// Write the header of a new element into the buffer, advance the write
    /// pointer, and return a pointer to where the data should be written.
    /// Returns a null pointer if the element is too large for the buffer, and
    /// in that case, the write pointer is unchanged.
    uint8_t *writeHeader(const char *identifier, uint8_t typeID, size_t length);

    /// Append the new element to the buffer.
    /// Returns false if the element is too large for the buffer.
    template <class T>
    bool append(const char *identifier, const T *data, size_t count);

    /// Overwrite the existing log element referenced by @p existing with the
    /// new data, if the type and size match.
    /// Returns false if the type or size doesn't match, and in that case,
    /// nothing is overwritten.
    template <class T>
    bool overwrite(LogEntryIterator &existing, const T *data, size_t count);

    /// (Over)write the data of a log element to the buffer.
    template <class T>
    void overwrite(uint8_t *buffer, const T *data, size_t count);

  public:
    /// Get the buffer containing the logging data.
    auto getBuffer() const -> const decltype(buffer) & { return buffer; }
    /// Get the length of the used part of the buffer.
    size_t getLength() const { return LOGGER_BUFFER_SIZE - maxLen; }

    /** 
     * @brief   Get the element with the given key.
     * 
     * @param   key
     *          The unique identifier / key of the element to retreive.
     * @return  An iterator to the element with the given key, or a 
     *          default-constructed LogEntryIterator if the key was not found in 
     *          the log.
     */
    LogEntryIterator find(const char *key) const;
};

/// Macro for easily logging variables with the variable name as the identifier.
#define LOG_VARIABLE(logger, var) logger.log(#var, var)

/// A global instance of the logger.
extern Logger MainLogger;

#include <logger/Logger.ipp>