#pragma once

#include <cstddef>
#include <cstdint>

/// Class for iterating over the elements in the buffer of a LogEntry.
class LogEntryIterator {
  public:
    /// Default constructor. Used as "end" iterator.
    LogEntryIterator();
    /// Create LogEntryIterator that iterates over the elements in the buffer.
    /// Only the pointer is saved, the buffer itself is not copied over, so its
    /// lifetime must be longer than all iterators that point to it.
    LogEntryIterator(const uint8_t *buffer, size_t length);

    /// Advance the iterator
    LogEntryIterator &operator++();
    /// Compare the iterator to the "end" iterator.
    /// (Only compares the remaining buffer length, which is zero for the "end"
    /// iterator.)
    bool operator!=(const LogEntryIterator &other);

    LogEntryIterator &operator*() { return *this; }
    const LogEntryIterator &operator*() const { return *this; }
    LogEntryIterator *operator->() { return this; }
    const LogEntryIterator *operator->() const { return this; }

    LogEntryIterator begin() { return *this; }
    static LogEntryIterator end() { return {}; }

    /// Get the type ID of the current element.
    uint8_t getTypeID() const { return type; }
    /// Get the length of the identifier / key of the current element.
    uint16_t getIDLength() const { return idLength; }
    /// Get the length of the data of the current element.
    uint16_t getDataLength() const { return dataLength; }
    /// Get a pointer to the beginning of the current element.
    const uint8_t *getBuffer() const { return buffer; }
    /// Get the identifier / key of the current element.
    const char *getID() const { return (const char *) buffer + 4; }
    /// Get a pointer to the data of the current element.
    const uint8_t *getData() const;

  private:
    const uint8_t *buffer;
    size_t remainingBufferLength;
    const uint8_t *data;
    uint16_t dataLength;
    uint8_t idLength;
    uint8_t type;

    /// Read the length and type of the element from the buffer.
    bool parse();
};