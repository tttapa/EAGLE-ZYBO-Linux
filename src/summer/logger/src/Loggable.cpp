#include <logger/Loggable.hpp>

Loggable::Loggable() {
    // Append to the linked list of instances
    if (first == nullptr)
        first = this;
    this->previous = last;
    if (this->previous != nullptr)
        this->previous->next = this;
    last       = this;
    this->next = nullptr;
}

Loggable::~Loggable() {
    // Remove from the linked list of instances
    if (this->previous != nullptr)
        this->previous->next = this->next;
    if (this == last)
        last = this->previous;
    if (this->next != nullptr)
        this->next->previous = this->previous;
    if (this == first)
        first = this->next;
}

bool Loggable::logAll(Logger &logger) {
    bool success = true;
    for (Loggable *el = first; el != nullptr; el = el->next)
        success &= el->log(logger);
    return success;
}

Loggable *Loggable::first = nullptr;
Loggable *Loggable::last  = nullptr;