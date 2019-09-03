#pragma once

class Logger;

/**
 * @brief   An abstract class / interface for objects that expose information 
 *          to be collected by the logger.
 * 
 * A list of all instances is maintained, so the logger can always request the 
 * logging data of all Loggable%s.
 */
class Loggable {
  protected:
    /**
     * @brief   Construct a new Loggable object.
     * 
     * A pointer to this new instance is saved to the list of instances.
     */
    Loggable();

  public:
    /**
     * @brief   Destroy the Loggable object.
     * 
     * The instance is removed from the list of instances.
     */
    virtual ~Loggable();

    /**
     * @brief   Write all of the logging data that this Loggable has available
     *          to the given logger.
     * 
     * @param   logger 
     *          The logger to log all data to.
     * @retval  true
     *          All data was successfully added to the logger's buffer.
     * @retval  false
     *          Either the buffer is full, or there are LogElement%s with the 
     *          same the same identifier, but a different size or type, or some
     *          other error occurred.
     */
    virtual bool log(Logger &logger) = 0;

    /**
     * @brief   Iterate over the list of all Loggable instances, and log all of
     *          their data to the given logger.
     * 
     * @param   logger 
     * @retval  true
     *          All data was successfully added to the logger's buffer.
     * @retval  false
     *          Either the buffer is full, or there are LogElement%s with the 
     *          same the same identifier, but a different size or type, or some
     *          other error occurred.
     */
    static bool logAll(Logger &logger);

  private:
    static Loggable *first;
    static Loggable *last;
    Loggable *next;
    Loggable *previous;
};