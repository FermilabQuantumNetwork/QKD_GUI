/*
This file is part of Time Tagger software defined digital data acquisition.

Copyright (C) 2011-2019 Swabian Instruments
All Rights Reserved

Unauthorized copying of this file is strictly prohibited.
*/

#ifndef TIMETAGGER_H_
#define TIMETAGGER_H_

#ifdef LIBTIMETAGGER_EXPORTS
#if defined(_WIN32)
#define TT_API __declspec(dllexport)
#pragma warning(disable : 4251)
#else
#define TT_API __attribute__((visibility("default")))
#endif
#else
#if defined(__linux) || defined(SWIG) || defined(NOEXPORT)
#define TT_API
#else
#define TT_API __declspec(dllimport)
#pragma warning(disable : 4251)
#endif
#endif

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <stdint.h>
#include <string>
#include <unordered_set>
#include <vector>

/*! \mainpage TimeTagger
 *
 * \brief backend for TimeTagger, an OpalKelly based single photon counting library
 *
 * \author Markus Wick <markus@swabianinstruments.com>
 * \author Helmut Fedder <helmut@swabianinstruments.com>
 * \author Michael Schlagmüller <michael@swabianinstruments.com>
 *
 * TimeTagger provides an easy to use and cost effective hardware solution for
 * time-resolved single photon counting applications.
 *
 * This document describes the C++ native interface to the TimeTagger device.
 */

class IteratorBase;
class IteratorBaseListNode;
class TimeTagger;
class TimeTaggerBase;
class TimeTaggerRunner;
class TimeTaggerVirtual;

#define timestamp_t long long
#define channel_t int

#define TIMETAGGER_VERSION "2.9.0"

/*
 *\brief Get the version of the TimeTagger cxx backend
 */

TT_API std::string getVersion();

/**
 * \brief Constant for unused channel.
 * Magic channel_t value to indicate an unused channel. So the iterators either
 * have to disable this channel, or to choose a default one.
 *
 * This value changed in version 2.1. The old value -1 aliases with falling events.
 * The old value will still be accepted for now if the old numbering scheme is active.
 */
constexpr channel_t CHANNEL_UNUSED = -134217728;
constexpr channel_t CHANNEL_UNUSED_OLD = -1;

/**
 * Allowed values for setTimeTaggerChannelNumberScheme().
 *
 * _ZERO will typically allocate the channel numbers 0 to 7 for the 8 input channels.
 * 8 to 15 will be allocated for the coresponding falling events.
 *
 * _ONE will typically allocate the channel numbers 1 to 8 for the 8 input channels.
 * -1 to -8 will be allocated for the coresponding falling events.
 *
 * _AUTO will choose the scheme based on the hardware revision and so based on the printed label.
 */
constexpr int TT_CHANNEL_NUMBER_SCHEME_AUTO = 0;
constexpr int TT_CHANNEL_NUMBER_SCHEME_ZERO = 1;
constexpr int TT_CHANNEL_NUMBER_SCHEME_ONE = 2;

#ifndef TIMETAGGER_NO_WRAPPER
/**
 * This are the default wrapper functions without any overloadings.
 */
#define GET_DATA_1D(function_name, type, argout, attribute)                                                            \
  attribute void function_name(std::function<type *(size_t)> argout)
#define GET_DATA_1D_OP1(function_name, type, argout, optional_type, optional_name, optional_default, attribute)        \
  attribute void function_name(std::function<type *(size_t)> argout, optional_type optional_name = optional_default)
#define GET_DATA_1D_OP2(function_name, type, argout, optional_type, optional_name, optional_default, optional_type2,   \
                        optional_name2, optional_default2, attribute)                                                  \
  attribute void function_name(std::function<type *(size_t)> argout, optional_type optional_name = optional_default,   \
                               optional_type2 optional_name2 = optional_default2)
#define GET_DATA_2D(function_name, type, argout, attribute)                                                            \
  attribute void function_name(std::function<type *(size_t, size_t)> argout)
#define GET_DATA_2D_OP1(function_name, type, argout, optional_type, optional_name, optional_default, attribute)        \
  attribute void function_name(std::function<type *(size_t, size_t)> argout,                                           \
                               optional_type optional_name = optional_default)
#define GET_DATA_2D_OP2(function_name, type, argout, optional_type, optional_name, optional_default, optional_type2,   \
                        optional_name2, optional_default2, attribute)                                                  \
  attribute void function_name(std::function<type *(size_t, size_t)> argout,                                           \
                               optional_type optional_name = optional_default,                                         \
                               optional_type2 optional_name2 = optional_default2)
#define GET_DATA_3D(function_name, type, argout, attribute)                                                            \
  attribute void function_name(std::function<type *(size_t, size_t, size_t)> argout)
#endif

/**
 * This enum selects the high resolution mode of the Time Tagger series.
 * If any high resolution mode is selected, the hardware will combine 2, 4 or even 8 input channels and average their
 * timestamps. This results in a discretization jitter improvement of factor sqrt(N) for N combined channels. The
 * averaging is implemented before any filter, buffer or USB transmission. So all of those features are available with
 * the averaged timestamps. Because of hardware limitations, only fixed combinations of channels are supported:
 * * HighResA: 1 : [1,2], 3 : [3,4], 5 : [5,6], 7 : [7,8], 10 : [10,11], 12 : [12,13], 14 : [14,15], 16 : [16,17], 9, 18
 * * HighResB: 1 : [1,2,3,4], 5 : [5,6,7,8], 10 : [10,11,12,13], 14 : [14,15,16,17], 9, 18
 * * HighResC: 5 : [1,2,3,4,5,6,7,8], 14 : [10,11,12,13,14,15,16,17], 9, 18
 * The inputs 9 and 18 are always available without averaging.
 * The number of channels available will be limited to the number of channels licensed.
 */
enum class Resolution { Standard = 0, HighResA = 1, HighResB = 2, HighResC = 3 };

/**
 * Enum for filtering the channel list returned by getChannelList.
 */
enum class ChannelEdge : int32_t {
// Bitwise filters, shall not be exported to wrapped languages
#ifndef SWIG
  NoFalling = 1 << 0,
  NoRising = 1 << 1,
  NoStandard = 1 << 2,
  NoHighRes = 1 << 3,
#endif

  All = 0,
  Rising = 1,
  Falling = 2,
  HighResAll = 4,
  HighResRising = 4 | 1,
  HighResFalling = 4 | 2,
  StandardAll = 8,
  StandardRising = 8 | 1,
  StandardFalling = 8 | 2
};
constexpr ChannelEdge TT_CHANNEL_RISING_AND_FALLING_EDGES = ChannelEdge::All;
constexpr ChannelEdge TT_CHANNEL_RISING_EDGES = ChannelEdge::Rising;
constexpr ChannelEdge TT_CHANNEL_FALLING_EDGES = ChannelEdge::Falling;

/**
 * \brief default constructor factory.
 *
 * \param serial serial number of FPGA board to use. if empty, the first board found is used.
 * \param resolution enum for how many channels shall be grouped. \see Resolution for details
 */
TT_API TimeTagger *createTimeTagger(std::string serial = "", Resolution resolution = Resolution::Standard);

/**
 * \brief default constructor factory for the createTimeTaggerVirtual class.
 */
TT_API TimeTaggerVirtual *createTimeTaggerVirtual();

/**
 * \brief set path and filename of the bitfile to be loeaded into the FPGA
 *
 * For debugging/development purposes the firmware loaded into the FPGA can be set manually with this function. To load
 * the default bitfile set bitFileName = "" \param bitFileName custom bitfile to use for the FPGA.
 */
TT_API void setCustomBitFileName(const std::string &bitFileName);

/**
 * \brief free a copy of a TimeTagger reference.
 * \param tagger the TimeTagger reference to free
 */
TT_API bool freeTimeTagger(TimeTaggerBase *tagger);

/**
 * \brief fetches a list of all available TimeTagger serials.
 *
 * This function may return serials blocked by other processes or already disconnected some milliseconds later.
 */
TT_API std::vector<std::string> scanTimeTagger();

TT_API std::string getTimeTaggerModel(const std::string &serial);

/**
 * \brief Configure the numbering scheme for new TimeTagger objects.
 * \param scheme new numbering scheme, must be TT_CHANNEL_NUMBER_SCHEME_AUTO, TT_CHANNEL_NUMBER_SCHEME_ZERO or
 * TT_CHANNEL_NUMBER_SCHEME_ONE
 *
 * This function sets the numbering scheme for newly created TimeTagger objects.
 * The default value is _AUTO.
 *
 * Note: TimeTagger objects are cached internally, so the scheme should be set before the first call of
 * createTimeTagger().
 *
 * _ZERO will typically allocate the channel numbers 0 to 7 for the 8 input channels.
 * 8 to 15 will be allocated for the coresponding falling events.
 *
 * _ONE will typically allocate the channel numbers 1 to 8 for the 8 input channels.
 * -1 to -8 will be allocated for the coresponding falling events.
 *
 * _AUTO will choose the scheme based on the hardware revision and so based on the printed label.
 */
TT_API void setTimeTaggerChannelNumberScheme(int scheme);

/**
 * \brief Fetch the currently configured global numbering scheme.
 *
 * Please see setTimeTaggerChannelNumberScheme() for details.
 * Please use TimeTagger::getChannelNumberScheme() to query the actual used numbering scheme,
 * this function here will just return the scheme a newly created TimeTagger object will use.
 */
TT_API int getTimeTaggerChannelNumberScheme();

/**
 * \brief Check if a license for the TimeTaggerVirtual is available
 */
TT_API bool hasTimeTaggerVirtualLicense();

/**
 * Update the license on the device.
 * Updated license may be fetched by getRemoteLicense.
 * The Time Tagger must not be instancated while updating the license.
 * \param serial the serial of the device to update the license. Must not be empty
 * \param license the binary license, encoded as a hexadecimal string
 */
TT_API void flashLicense(const std::string &serial, const std::string &license);

/**
 * Parses the binary license and return a human readable information about this license.
 * \param license the binary license, encoded as a hexadecimal string
 * \return a human readable string containing all information about this license
 */
TT_API std::string extractLicenseInfo(const std::string &license);

// log values are taken from https://docs.python.org/3/library/logging.html
enum LogLevel { LOGGER_ERROR = 40, LOGGER_WARNING = 30, LOGGER_INFO = 10 };
typedef void (*logger_callback)(LogLevel level, std::string msg);

/**
 * \brief Sets the notifier callback which is called for each log message
 * \returns The old callback
 *
 * If this function is called with nullptr, the default callback will be used.
 */
TT_API logger_callback setLogger(logger_callback callback);

/**
 * \brief Raise a new log message. Please use the XXXLog macro instead.
 */
TT_API void LogBase(LogLevel level, const char *file, int line, bool censored, const char *fmt, ...)
#ifdef __GNUC__
    __attribute__((format(printf, 5, 6)))
#endif
    ;
#define LogMessage(level, ...) LogBase(level, __FILE__, __LINE__, false, __VA_ARGS__);
#define ErrorLog(...) LogMessage(LOGGER_ERROR, __VA_ARGS__);
#define WarningLog(...) LogMessage(LOGGER_WARNING, __VA_ARGS__);
#define InfoLog(...) LogMessage(LOGGER_INFO, __VA_ARGS__);

// This censored methods are used when the log may contain private/confidential data and we
// don't want the usage statistics system to record such data.
#define LogMessageCensored(level, ...) LogBase(level, __FILE__, __LINE__, true, __VA_ARGS__);
#define ErrorLogCensored(...) LogMessage(LOGGER_ERROR, __VA_ARGS__);
#define WarningLogCensored(...) LogMessage(LOGGER_WARNING, __VA_ARGS__);
#define InfoLogCensored(...) LogMessage(LOGGER_INFO, __VA_ARGS__);

class TT_API CustomLogger {
public:
  CustomLogger();
  virtual ~CustomLogger();

  void enable();
  void disable();
  virtual void Log(int level, const std::string &msg) = 0;

private:
  static void LogCallback(LogLevel level, std::string msg);
  static CustomLogger *instance;
  static std::mutex instance_mutex;
};

class TT_API TimeTaggerBase {
  friend class IteratorBase;
  friend class TimeTaggerProxy;
  friend class TimeTaggerRunner;

public:
  /**
   * Generate a new fence object, which validates the current configuration and the current time.
   * This fence is uploaded to the earliest pipeline stage of the Time Tagger.
   * Waiting on this fence ensures that all hardware settings such as trigger levels, channel
   * registrations, etc., have propagated to the FPGA and are physically active. Synchronizes
   * the Time Tagger internal memory, so that all tags arriving after the waitForFence call were
   * actually produced after the getFence call. The waitForFence function waits until all tags,
   * which are present at the time of the function call within the internal memory of the Time
   * Tagger, are processed.
   * This call might block to limit the amount of active fences.
   * \param alloc_fence if false, a reference to the most recently created fence will be returned instead
   * \return the allocated fence
   */
  virtual unsigned int getFence(bool alloc_fence = true) = 0;

  /**
   * Wait for a fence in the data stream.
   * See getFence for more details.
   * \param fence fence object, which shall be waited on
   * \param timeout timeout in milliseconds. Negative means no timeout, zero returns immediately.
   * \return true if the fence has passed, false on timeout
   */
  virtual bool waitForFence(unsigned int fence, int64_t timeout = -1) = 0;

  /**
   * Sync the timetagger pipeline, so that all started iterators and their enabled channels are ready
   * This is a shortcut for calling getFence and waitForFence at once. See getFence for more details.
   * \param timeout timeout in milliseconds. Negative means no timeout, zero returns immediately.
   * \return true on success, false on timeout
   */
  virtual bool sync(int64_t timeout = -1) = 0;

  /**
   * \brief get the falling channel id for a raising channel and vice versa
   */
  virtual channel_t getInvertedChannel(channel_t channel) = 0;

  /**
   * \brief compares the provided channel with CHANNEL_UNUSED
   *
   * But also keeps care about the channel number scheme and selects
   * either CHANNEL_UNUSED or CHANNEL_UNUSED_OLD
   */
  virtual bool isUnusedChannel(channel_t channel) = 0;

  typedef std::function<void(IteratorBase *)> IteratorCallback;
  typedef std::map<IteratorBase *, IteratorCallback> IteratorCallbackMap;

  /**
   * \brief Run synchronized callbacks for a list of iterators
   *
   * This method has a list of callbacks for a list of iterators.
   * Those callbacks are called for a synchronized data set, but in parallel.
   * They are called from an internal worker thread.
   * As the data set is synchronized, this creates a bottleneck for one
   * worker thread, so only fast and non-blocking callbacks are allowed.
   *
   * \param callbacks Map of callbacks per iterator
   * \param block Shall this method block until all callbacks are finished
   */
  virtual void runSynchronized(const IteratorCallbackMap &callbacks, bool block = true) = 0;

  /**
   * Fetches the overall configuration status of the Time Tagger object.
   * \return a JSON serialized string with all configuration and status flags.
   */
  virtual std::string getConfiguration() = 0;

  /**
   * \brief set time delay on a channel
   *
   * When set, every event on this channel is delayed by the given delay in picoseconds.
   *
   * This method has the best performance with "small delays". The delay is considered
   * "small" when less than 100 events arrive within the time of the largest delay set.
   * For example, if the total event-rate over all channels used is 10 Mevent/s, the
   * signal can be delayed efficiently up to 10 microseconds. For large delays, please
   * use DelayedChannel instead.
   *
   * \param channel  the channel to set
   * \param delay    the delay in picoseconds
   */
  virtual void setInputDelay(channel_t channel, timestamp_t delay) = 0;

  /**
   * \brief set time delay on a channel
   *
   * When set, every event on this physical input channel is delayed by the given delay in picoseconds.
   * This delay is implemented on the hardware before any filter with no performance overhead.
   * The maximum delay on the Time Tagger Ultra series is 2 us.
   * This affects both the rising and the falling event at the same time.
   *
   * \param channel  the channel to set
   * \param delay    the delay in picoseconds
   */
  virtual void setDelayHardware(channel_t channel, timestamp_t delay) = 0;

  /**
   * \brief set time delay on a channel
   *
   * When set, every event on this channel is delayed by the given delay in picoseconds.
   * This happens on the computer and so after the on-device filters.
   * Please use setDelayHardware instead for better performance.
   * This affects either the the rising or the falling event only.
   *
   * This method has the best performance with "small delays". The delay is considered
   * "small" when less than 100 events arrive within the time of the largest delay set.
   * For example, if the total event-rate over all channels used is 10 Mevent/s, the
   * signal can be delayed efficiently up to 10 microseconds. For large delays, please
   * use DelayedChannel instead.
   *
   * \param channel  the channel to set
   * \param delay    the delay in picoseconds
   */
  virtual void setDelaySoftware(channel_t channel, timestamp_t delay) = 0;

  /**
   * \brief get time delay of a channel
   *
   * see setInputDelay
   *
   * \param channel   the channel
   */
  virtual timestamp_t getInputDelay(channel_t channel) = 0;

  /**
   * \brief get time delay of a channel
   *
   * see setDelaySoftware
   *
   * \param channel   the channel
   */
  virtual timestamp_t getDelaySoftware(channel_t channel) = 0;

  /**
   * \brief get time delay of a channel
   *
   * see setDelayHardware
   *
   * \param channel   the channel
   */
  virtual timestamp_t getDelayHardware(channel_t channel) = 0;

  /**
   * \brief set the deadtime between two edges on the same channel.
   *
   * This function sets the user configureable deadtime. The requested time will
   * be rounded to the nearest multiple of the clock time. The deadtime will also
   * be clamped to device specific limitations.
   *
   * As the actual deadtime will be altered, the real value will be returned.
   *
   * \param channel channel to be configured
   * \param deadtime new deadtime
   * \return the real configured deadtime
   */
  virtual timestamp_t setDeadtime(channel_t channel, timestamp_t deadtime) = 0;

  /**
   * \brief get the deadtime between two edges on the same channel.
   *
   * This function gets the user configureable deadtime.
   *
   * \param channel channel to be queried
   * \return the real configured deadtime
   */
  virtual timestamp_t getDeadtime(channel_t channel) = 0;

  /**
   * \brief enable the calibration on a channel.
   *
   * This will connect or disconnect the channel with the on-chip uncorrelated signal generator.
   *
   * \param channel  the channel
   * \param enabled  enabled / disabled flag
   */
  virtual void setTestSignal(channel_t channel, bool enabled) = 0;
  virtual void setTestSignal(std::vector<channel_t> channel, bool enabled) = 0;

  /**
   * \brief fetch the status of the test signal generator
   *
   * \param channel   the channel
   */
  virtual bool getTestSignal(channel_t channel) = 0;

  /**
   * \brief get overflow count
   *
   * Get the number of communication overflows occured
   *
   */
  virtual long long getOverflows() = 0;

  /**
   * \brief clear overflow counter
   *
   * Sets the overflow counter to zero
   */
  virtual void clearOverflows() = 0;

  /**
   * \brief get and clear overflow counter
   *
   * Get the number of communication overflows occured and sets them to zero
   */
  virtual long long getOverflowsAndClear() = 0;

protected:
  /**
   * \brief abstract interface class
   */
  TimeTaggerBase() {}

  /**
   * destructor
   */
  virtual ~TimeTaggerBase(){};

  // Non Copyable
  TimeTaggerBase(const TimeTaggerBase &) = delete;
  TimeTaggerBase &operator=(const TimeTaggerBase &) = delete;

  // Used by IteratorBase to add itself
  virtual std::shared_ptr<IteratorBaseListNode> addIterator(IteratorBase *it) = 0;

  // Used by IteratorBase to specify when it's being deleted.
  virtual void freeIterator(IteratorBase *it) = 0;

  // allocate a new virtual output channel
  virtual channel_t getNewVirtualChannel() = 0;

  // free a virtual channel being used.
  virtual void freeVirtualChannel(channel_t channel) = 0;

  /**
   * \brief register a FPGA channel.
   *
   * Only events on previously registered channels will be transfered over
   * the communication channel.
   *
   * \param channel  the channel
   */
  virtual void registerChannel(channel_t channel) = 0;

  /**
   * \brief release a previously registered channel.
   *
   * \param channel   the channel
   */
  virtual void unregisterChannel(channel_t channel) = 0;

  // Used by proxy time tagger to add itself as a dependant tagger.
  virtual void addChild(TimeTaggerBase *child) = 0;

  // Used by proxy time tagger to remove itself as a dependant tagger.
  virtual void removeChild(TimeTaggerBase *child) = 0;

  // Used by a proxy time tagger to allow its parent to release it and its dependancies.
  virtual void release() = 0;
};

/**
 * \brief virtual TimeTagger based on dump files
 *
 * The TimeTaggerVirtual class represents a virtual Time Tagger.
 * But instead of connecting to Swabians hardware, it replays all tags
 * from a recorded file.
 */
class TT_API TimeTaggerVirtual : virtual public TimeTaggerBase {
public:
  /**
   * \brief replay a given dump file on the disc
   *
   * This method adds the file to the replay queue.
   * If the flag 'queue' is false, the current queue will be flushed and this file will be replayed immediatelly.
   *
   * \param file the file to be replayed
   * \param begin amount of ps to skip at the begin of the file. A negativ time will generate a pause in the replay
   * \param duration time period in ps of the file. -1 replays till the last tag
   * \param queue flag if this file shall be queued
   * \return ID of the queued file
   */
  virtual uint64_t replay(const std::string &file, timestamp_t begin = 0, timestamp_t duration = -1,
                          bool queue = true) = 0;

  /**
   * \brief stops the current and all queued files.
   *
   * This method stops the current file and clears the replay queue.
   */
  virtual void stop() = 0;

  /**
   * \brief stops the all queued files and resets the TimeTaggerVirtual to its default settings
   *
   * This method stops the current file, clears the replay queue and resets the TimeTaggerVirtual to its default
   * settings.
   */
  virtual void reset() = 0;

  /**
   * \brief block the current thread until the replay finish
   *
   * This method blocks the current execution and waits till the given file has finished its replay.
   * If no ID is provided, it waits until all queued files are replayed.
   *
   * This function does not block on a zero timeout. Negative timeouts are interpreted as infinite timeouts.
   *
   * \param ID selects which file to wait for
   * \param timeout timeout in milliseconds
   * \return true if the file is complete, false on timeout
   */
  virtual bool waitForCompletion(uint64_t ID = 0, int64_t timeout = -1) = 0;

  /**
   * \brief configures the speed factor for the virtual tagger.
   *
   * This method configures the speed factor of this virtual Time Tagger.
   * A value of 1.0 will replay in real time.
   * All values < 0.0 will replay the data as fast as possible, but stops at the end of all data. This is the default
   * value.
   *
   * \param speed ratio of the replay speed and the real time
   */
  virtual void setReplaySpeed(double speed) = 0;

  /**
   * \brief fetches the speed factor
   *
   * Please see setReplaySpeed for more details.
   *
   * \return the speed factor
   */
  virtual double getReplaySpeed() = 0;

  /**
   * \brief configures the conditional filter
   *
   * After each event on the trigger channels, one event per filtered channel
   * will pass afterwards. This is handled in a very early stage in the pipeline,
   * so all event limitations but the deadtime are supressed. But the accuracy
   * of the order of those events is low.
   *
   * Refer the Manual for a description of this function.
   *
   * \param trigger the channels that sets the condition
   * \param filtered  the channels that are filtered by the condition
   */
  virtual void setConditionalFilter(std::vector<channel_t> trigger, std::vector<channel_t> filtered) = 0;

  /**
   * \brief deactivates the conditional filter
   *
   * equivilent to setConditionalFilter({},{})
   *
   */
  virtual void clearConditionalFilter() = 0;

  /**
   * \brief fetches the configuration of the conditional filter
   *
   * see setConditionalFilter
   */
  virtual std::vector<channel_t> getConditionalFilterTrigger() = 0;

  /**
   * \brief fetches the configuration of the conditional filter
   *
   * see setConditionalFilter
   */
  virtual std::vector<channel_t> getConditionalFilterFiltered() = 0;
};

/**
 * \brief backend for the TimeTagger.
 *
 * The TimeTagger class connects to the hardware, and handles the communication over the usb.
 * There may be only one instance of the backend per physical device.
 */
class TT_API TimeTagger : virtual public TimeTaggerBase {
public:
  /**
   * \brief reset the TimeTagger object to default settings and detach all iterators
   */
  virtual void reset() = 0;

  /**
   * \brief set the divider for the frequency of the test signal
   *
   * The base clock of the test signal oscillator for the Time Tagger Ultra is running at 100.8 MHz sampled down by an
   * factor of 2 to have a similar base clock as the Time Tagger 20 (~50 MHz). The default divider is 63 -> ~800
   * kEvents/s
   *
   * \param divider frequency divisor of the oscillator
   */
  virtual void setTestSignalDivider(int divider) = 0;

  /**
   * \brief get the divider for the frequency of the test signal
   */
  virtual int getTestSignalDivider() = 0;

  /**
   * \brief set the trigger voltage threshold of a channel
   *
   * \param channel   the channel to set
   * \param voltage    voltage level.. [0..1]
   */
  virtual void setTriggerLevel(channel_t channel, double voltage) = 0;

  /**
   * \brief get the trigger voltage threshold of a channel
   *
   * \param channel the channel
   */
  virtual double getTriggerLevel(channel_t channel) = 0;

  /**
   * \brief get hardware delay compensation of a channel
   *
   * The physical input delays are calibrated and compensated.
   * However this compensation is implemented after the conditional filter and so affects its result.
   * This function queries the effective input delay, which compensates the hardware delay.
   *
   * \param channel the channel
   * \return the hardware delay compensation in picoseconds
   */
  virtual timestamp_t getHardwareDelayCompensation(channel_t channel) = 0;

  /**
   * \brief configures the input multiplexer
   *
   * Every phyiscal input channel has an input multiplexer with 4 modes:
   * 0: normal input mode
   * 1: use the input from channel -1 (left)
   * 2: use the input from channel +1 (right)
   * 3: use the reference oscillator
   *
   * Mode 1 and 2 cascades, so many inputs can be configured to get the same input events.
   *
   * \param channel the phyiscal channel of the input multiplexer
   * \param mux_mode the configuration mode of the input multiplexer
   */
  virtual void setInputMux(channel_t channel, int mux_mode) = 0;

  /**
   * \brief fetches the configuration of the input multiplexer
   *
   * \param channel the phyiscal channel of the input multiplexer
   * \return the configuration mode of the input multiplexer
   */
  virtual int getInputMux(channel_t channel) = 0;

  /**
   * \brief configures the conditional filter
   *
   * After each event on the trigger channels, one event per filtered channel
   * will pass afterwards. This is handled in a very early stage in the pipeline,
   * so all event limitations but the deadtime are supressed. But the accuracy
   * of the order of those events is low.
   *
   * Refer the Manual for a description of this function.
   *
   * \param trigger the channels that sets the condition
   * \param filtered  the channels that are filtered by the condition
   * \param hardwareDelayCompensation if false, the physical hardware delay will not be compensated
   */
  virtual void setConditionalFilter(std::vector<channel_t> trigger, std::vector<channel_t> filtered,
                                    bool hardwareDelayCompensation = true) = 0;

  /**
   * \brief deactivates the conditional filter
   *
   * equivilent to setConditionalFilter({},{})
   *
   */
  virtual void clearConditionalFilter() = 0;

  /**
   * \brief fetches the configuration of the conditional filter
   *
   * see setConditionalFilter
   */
  virtual std::vector<channel_t> getConditionalFilterTrigger() = 0;

  /**
   * \brief fetches the configuration of the conditional filter
   *
   * see setConditionalFilter
   */
  virtual std::vector<channel_t> getConditionalFilterFiltered() = 0;

  /**
   * \brief enables or disables the normalization of the distribution.
   *
   * Refer the Manual for a description of this function.
   *
   * \param channel list of channels to modify
   * \param state the new state
   */
  virtual void setNormalization(std::vector<channel_t> channel, bool state) = 0;

  /**
   * \brief returns the the normalization of the distribution.
   *
   * Refer the Manual for a description of this function.
   *
   * \param channel the channel to query
   * \return if the normalization is enabled
   */
  virtual bool getNormalization(channel_t channel) = 0;

  /**
   * \brief sets the maximum USB buffer size
   *
   * This option controls the maximum buffer size of the USB connection.
   * This can be used to balance low input latency vs high (peak) throughput.
   *
   * \param size the maximum buffer size in events
   */
  virtual void setHardwareBufferSize(int size) = 0;

  /**
   * \brief queries the size of the USB queue
   *
   * See setHardwareBufferSize for more information.
   *
   * \return the actual size of the USB queue in events
   */
  virtual int getHardwareBufferSize() = 0;

  /**
   * \brief sets the maximum events and latency for the stream block size
   *
   * This option controls the latency and the block size of the data stream.
   * The default values are max_events = 131072 events and max_latency = 20 ms.
   * Depending on which of the two parameters is exceeded first, the block stream size is adjusted accordingly.
   * The block size will be reduced automatically for blocks when no signal is arriving for 512 ns on the Time Tagger
   * Ultra and 1536 ns for the Time Tagger 20.   *
   *
   * \param max_events  maximum number of events
   * \param max_latency maximum latency in ms
   */
  virtual void setStreamBlockSize(int max_events, int max_latency) = 0;
  virtual int getStreamBlockSizeEvents() = 0;
  virtual int getStreamBlockSizeLatency() = 0;

  /**
   * \brief Divides the amount of transmitted edge per channel
   *
   * This filter decimates the events on a given channel by a specified.
   * factor. So for a divider n, every nth event is transmitted through
   * the filter and n-1 events are skipped between consecutive
   * transmitted events. If a conditional filter is also active, the event
   * divider is applied after the conditional filter, so the conditional
   * is applied to the complete event stream and only events which pass the
   * conditional filter are forwarded to the divider.
   *
   * As it is a hardware filter, it reduces the required USB bandwidth and
   * CPU processing power, but it cannot be configured for virtual channels.
   *
   * \param channel channel to be configured
   * \param divider new divider, must be smaller than 65536
   */
  virtual void setEventDivider(channel_t channel, unsigned int divider) = 0;

  /**
   * \brief Returns the factor of the dividing filter
   *
   * See setEventDivider for further details.
   *
   * \param channel channel to be queried
   * \return the configured divider
   */
  virtual unsigned int getEventDivider(channel_t channel) = 0;

  /**
   * \brief runs a calibrations based on the on-chip uncorrelated signal generator.
   */
  GET_DATA_1D(autoCalibration, double, array_out, virtual) = 0;

  /**
   * \brief identifies the hardware by serial number
   */
  virtual std::string getSerial() = 0;

  /**
   * \brief identifies the hardware by Time Tagger Model
   */
  virtual std::string getModel() = 0;

  /**
   * \brief Fetch the configured numbering scheme for this TimeTagger object
   *
   * Please see setTimeTaggerChannelNumberScheme() for details.
   */
  virtual int getChannelNumberScheme() = 0;

  /**
   * \brief returns the minumum and the maximum voltage of the DACs as a trigger reference
   */
  virtual std::vector<double> getDACRange() = 0;

  /**
   * \brief get internal calibration data
   */
  GET_DATA_2D(getDistributionCount, uint64_t, array_out, virtual) = 0;

  /**
   * \brief get internal calibration data
   * This method is not supported any more on the Time Tagger Ultra series
   * \deprecated
   */
  GET_DATA_2D(getDistributionPSecs, timestamp_t, array_out, virtual) = 0;

  /**
   * \brief fetch a vector of all physical input channel ids
   *
   * The function returns the channel of all rising and falling edges.
   * For example for the Time Tagger 20 (8 input channels)
   * TT_CHANNEL_NUMBER_SCHEME_ZERO: {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
   * and for
   * TT_CHANNEL_NUMBER_SCHEME_ONE: {-8,-7,-6,-5,-4,-3,-2,-1,1,2,3,4,5,6,7,8}
   *
   * TT_CHANNEL_RISING_EDGES returns only the rising edges
   * SCHEME_ONE: {1,2,3,4,5,6,7,8}
   * and
   * TT_CHANNEL_FALLING_EDGES resturn only the falling edges
   * SCHEME_ONE: {-1,-2,-3,-4,-5,-6,-7,-8}
   * which are the invertedChannels of the rising edges.
   */
  virtual std::vector<channel_t> getChannelList(ChannelEdge type = ChannelEdge::All) = 0;

  /**
   * \brief fetch the duration of each clock cycle in picoseconds
   */
  virtual timestamp_t getPsPerClock() = 0;

  /**
   * \brief Return the hardware version of the PCB board. Version 0 is everything before mid 2018
   * and with the channel configuration ZERO. version >= 1 is channel configuration ONE
   */
  virtual std::string getPcbVersion() = 0;

  /**
   * \brief Return an unique identifier for the applied firmware.
   *
   * This function returns a comma separated list of the firmware version with
   * - the device identifier: TT-20 or TT-Ultra
   * - the firmware identifier: FW 3
   * - optional the timestamp of the assembling of the firmware
   * - the firmware indentifier of the USB chip: OK 1.30
   * eg "TT-Ultra, FW 3, TS 2018-11-13 22:57:32, OK 1.30"
   */
  virtual std::string getFirmwareVersion() = 0;

  /**
   * \brief Show the status of the sensor data from the FPGA and peripherals on the console
   */
  virtual std::string getSensorData() = 0;

  /**
   * \brief Enforce a state to the LEDs
   * 0: led_status[R]      16: led_status[R] - mux
   * 1: led_status[G]      17: led_status[G] - mux
   * 2: led_status[B]      18: led_status[B] - mux
   * 3: led_power[R]       19: led_power[R]  - mux
   * 4: led_power[G]       20: led_power[G]  - mux
   * 5: led_power[B]       21: led_power[B]  - mux
   * 6: led_clock[R]       22: led_clock[R]  - mux
   * 7: led_clock[G]       23: led_clock[G]  - mux
   * 8: led_clock[B]       24: led_clock[B]  - mux
   */
  virtual void setLED(uint32_t bitmask) = 0;

  /**
   * Fetches and parses the current installed license on this device
   * \return a human readable string containing all information about the license on this device
   */
  virtual std::string getLicenseInfo() = 0;

  /**
   * \brief Direct read/write access to WireIn/WireOuts in FPGA (mask==0 for readonly)
   *
   * DO NOT USE. Only for internal debug purposes.
   */
  virtual uint32_t factoryAccess(uint32_t pw, uint32_t addr, uint32_t data, uint32_t mask) = 0;

  /**
   * \brief Set the Time Taggers internal buzzer to a frequency in Hz (freq_hz==0 to disable)
   * \param freq_hz the generated audio frequency
   */
  virtual void setSoundFrequency(uint32_t freq_hz) = 0;
};

/**
 * \brief a single event on a channel
 *
 * Channel events are passed from the backend to registered iterators
 * by the IteratorBase::next() callback function.
 *
 * A Tag describes a single event on a channel.
 */
struct Tag {
  /** This enum marks what kind of event this object represents:
   * TimeTag: a normal event from any input channel
   * Error: an error in the internal data processing, e.g. on plugging the external clock. This invalidates the global
   * time OverflowBegin: this marks the begin of an interval with incomplete data because of too high data rates
   * OverflowEnd: this marks the end of the interval. All events, which were lost in this interval, have been handled
   * MissedEvents: this virtual event signals the amount of lost events per channel within an overflow interval.
   * Repeated usage for higher amounts of events
   */
  enum class Type : unsigned char { TimeTag = 0, Error = 1, OverflowBegin = 2, OverflowEnd = 3, MissedEvents = 4 } type;

  // 8 bit padding, reserved for future use
  char reserved;

  // Within overflow intervals, the timing of all events is skipped. However, the total amount of events is still
  // recorded. For events with type = MissedEvents, this indicates that a given amount of tags for this channel have
  // been skipped in the interval. Note: There might be many missed events tags per overflow interval and channel. The
  // accumulated amount represents the total skipped events.
  unsigned short missed_events;

  // the channel number
  channel_t channel;

  // the timestamp of the event in picoseconds
  timestamp_t time;
};

class TT_API OrderedBarrier {
public:
  class TT_API OrderInstance {
  public:
    OrderInstance();
    OrderInstance(OrderedBarrier *parent, uint64_t instance_id);
    ~OrderInstance();
    void sync();
    void release();

  private:
    friend class OrderedBarrier;

    OrderedBarrier *parent{};
    bool obtained{};
    uint64_t instance_id{};
  };

  OrderedBarrier();
  ~OrderedBarrier();

  OrderInstance queue();
  void waitUntilFinished();

private:
  friend class OrderInstance;

  void release(uint64_t index);
  void obtain(uint64_t index);

  uint64_t accumulator{};
  uint64_t current_state{};
  std::mutex inner_mutex;
  std::condition_variable cv;
};

class TT_API OrderedPipeline {
public:
  OrderedPipeline();
  ~OrderedPipeline();

private:
  friend class IteratorBase;

  bool initialized = false;
  std::list<OrderedBarrier>::iterator stage;
};

/**
 * \brief Base class for all iterators
 *
 *
 */
class TT_API IteratorBase {
  friend class TimeTaggerRunner;
  friend class TimeTaggerProxy;
  friend class SynchronizedMeasurements;

private:
  // Abstract class
  IteratorBase() = delete;

  // Non Copyable
  IteratorBase(const IteratorBase &) = delete;
  IteratorBase &operator=(const IteratorBase &) = delete;
  void clearWithoutLock();

protected:
  /**
   * \brief standard constructor
   *
   * will register with the TimeTagger backend.
   */
  IteratorBase(TimeTaggerBase *tagger, std::string base_type_ = "IteratorBase", std::string extra_info_ = "");

public:
  /**
   * \brief destructor
   *
   * will stop and unregister prior finalization.
   */
  virtual ~IteratorBase();

  /**
   * \brief start the iterator
   *
   * The default behavior for iterators is to start automatically on creation.
   */
  void start();

  /**
   * \brief start the iterator, and stops it after the capture_duration
   * \param capture_duration capture duration until the meassurement is stopped
   * \param clear resets the data aquired
   *
   * When the startFor is called before the previous measurement has ended and the clear
   * parameter is set to false, then the passed capture_duration will be added on top to the current
   * max_capture_duration
   */
  void startFor(timestamp_t capture_duration, bool clear = true);

  /**
   * \brief wait until the iterator has finished running.
   *
   * \param timeout     time in milliseconds to wait for the measurements. If negative, wait until finished.
   *
   * waitUntilFinished will wait according to the timeout and return true if the iterator finished or false if not.
   * Furthermore, when waitUntilFinished is called on a iterator running indefinetly, it will log an error and
   * return inmediatly.
   */
  bool waitUntilFinished(int64_t timeout = -1);

  /**
   * \brief stop the iterator
   *
   * The iterator is put into the STOPPED state, but will still be registered with the backend.
   */
  void stop();

  /**
   * \brief clear Iterator state.
   */
  void clear();

  /**
   * \brief query the Iterator state.
   *
   * Fetches if this iterator is running.
   */
  bool isRunning();

  /**
   * \brief query the evaluation time
   *
   * Query the total capture duration since the last call to clear.
   * This might have a wrong amount of time if there were some overflows within this range.
   * \return capture duration of the data
   */
  timestamp_t getCaptureDuration();

protected:
  /**
   * \brief register a channel
   *
   * Only channels registered by any iterator attached to a backend are delivered over the usb.
   *
   * \param channel  the channel
   */
  void registerChannel(channel_t channel);

  /**
   * \brief unregister a channel
   *
   * \param channel  the channel
   */
  void unregisterChannel(channel_t channel);

  /**
   * \brief allocate a new virtual output channel for this iterator
   */
  channel_t getNewVirtualChannel();

  /**
   * \brief method to call after finishing the initialization of the measurement
   */
  void finishInitialization();

  /**
   * \brief clear Iterator state.
   *
   * Each Iterator should implement the clear_impl() method to reset
   * its internal state.
   * The clear_impl() function is guarded by the update lock.
   */
  virtual void clear_impl(){};

  /**
   * \brief callback when the measurement class is started
   *
   * This function is guarded by the update lock.
   */
  virtual void on_start(){};

  /**
   * \brief callback when the measurement class is stopped
   *
   * This function is guarded by the update lock.
   */
  virtual void on_stop(){};

  /**
   * \brief aquire update lock
   *
   * All mutable operations on a iterator are guarded with an update mutex.
   * Implementers are adviced to lock() an iterator, whenever internal state
   * is queried or changed.
   *
   * \deprecated use getLock
   */
  void lock();

  /**
   * \brief release update lock
   *
   * see lock()
   *
   * \deprecated use getLock
   */
  void unlock();

  /**
   * \brief release lock and continue work in parallel
   *
   * The measurement's lock is released, allowing this measurement to continue,
   * while still executing work in parallel.
   *
   * \return a ordered barrier instance that can be synced afterwards.
   */
  OrderedBarrier::OrderInstance parallelize(OrderedPipeline &pipeline);

  /**
   * \brief aquire update lock
   *
   * All mutable operations on a iterator are guarded with an update mutex.
   * Implementers are adviced to lock an iterator, whenever internal state
   * is queried or changed.
   *
   * \return a lock object, which releases the lock when this instance is freed
   */
  std::unique_lock<std::mutex> getLock();

  /**
   * \brief update iterator state
   *
   * Each Iterator must implement the next_impl() method.
   * The next_impl() function is guarded by the update lock.
   *
   * The backend delivers each Tag on each registered channel
   * to this callback function.
   *
   * \param incoming_tags block of events
   * \param begin_time earliest event in the block
   * \param end_time begin_time of the next block, not including in this block
   * \return true if the content of this block was modified, false otherwise
   */
  virtual bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) = 0;

  void finish_running();

  /// list of channels used by the iterator
  std::set<channel_t> channels_registered;

  /// running state of the iterator
  bool running;

  bool autostart;

  TimeTaggerBase *tagger;

  timestamp_t capture_duration; // duration the iterator has already captured data

private:
  struct TelemetryData {
    uint64_t duration;
    bool is_on;
  };

  void next(std::unique_lock<std::mutex> &lock, std::vector<Tag> &incoming_tags, timestamp_t begin_time,
            timestamp_t end_time, uint32_t fence, TelemetryData &telem_data);

  void pre_stop();

  std::shared_ptr<IteratorBaseListNode> iter;
  timestamp_t max_capture_duration; // capture duration at which the .stop() method will be called, <0 for infinity
  std::mutex pre_stop_mutex;
  uint32_t min_fence;
  std::unordered_set<channel_t> virtual_channels;
  std::string base_type;
  std::string extra_info;
  uint64_t id{};
};
using _Iterator = IteratorBase;

enum class LanguageUsed : std::uint32_t {
  Cpp = 0,
  Python,
  Csharp,
  Matlab,
  Labview,
  Mathematica,
  // Add more languages/Platforms
  Unknown = 255,
};

enum class FrontendType : std::uint32_t {
  Undefined = 0,
  WebApp,
  Firefly,
  Pyro5RPC,
  UserFrontend,
};

/**
 * \brief sets the language being used currently for usage statistics system.
 *
 * \param pw  password for authporization to change the language.
 * \param language  programming language being used.
 * \param version  version of the programming langugae being used.
 */
TT_API void setLanguageInfo(std::uint32_t pw, LanguageUsed language, std::string version);

/**
 * \brief sets the frontend being used currently for usage statistics system.
 *
 * \param frontend  the frontend currently being used.
 */
TT_API void setFrontend(FrontendType frontend);

enum class UsageStatisticsStatus {
  Disabled,               // User Opted out
  Collecting,             // User enabled it to collect for debug purpose
  CollectingAndUploading, // User gave their consent to collect and upload
};

/**
 * \brief sets the status of the usage statistics system.
 *
 * This fuinctionality allows configuring the usage statistics system.
 *
 * \param new_status  new status of the usage statistics system.
 */
TT_API void setUsageStatisticsStatus(UsageStatisticsStatus new_status);

/**
 * \brief gets the status of the usage statistics system.
 *
 * \return the current status of the usage statistics system.
 */
TT_API UsageStatisticsStatus getUsageStatisticsStatus();

/**
 * \brief gets the current recorded data by the usage statistics system.
 *
 * Use this function to see what data has been collected so far and what will be sent to Swabian Instruments if
 * 'CollectingAndUploading' is enabled. All data is pseudonymize.
 *
 * \note if no data has been collected or due to a system error, the database was corrupted, it will return an error.
 * else it will be a database in json format.
 *
 * \return the current recorded data by the usage statistics system.
 */
TT_API std::string getUsageStatisticsReport();

#endif /* TIMETAGGER_H_ */
