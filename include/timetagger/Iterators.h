/*
This file is part of Time Tagger software defined digital data acquisition.

Copyright (C) 2011-2019 Swabian Instruments
All Rights Reserved

Unauthorized copying of this file is strictly prohibited.
*/

#ifndef TT_ITERATORS_H_
#define TT_ITERATORS_H_

#include <algorithm>
#include <array>
#include <assert.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <stdint.h>
#include <stdio.h>
#include <unordered_map>
#include <vector>

// Include mulh helpers on MSVC
#if !defined(__SIZEOF_INT128__) && (defined(_M_X64) || defined(_M_ARM64))
#include <intrin.h>
#endif

#include "TimeTagger.h"

/**
 * Helper class for fast division with a constant divisor.
 * It chooses the method on initialization time and precompile the evaluation functions for all methods.
 */
class TT_API FastBinning {
public:
  enum class Mode {
    ConstZero,
    Dividend,
    PowerOfTwo,
    FixedPoint_32,
    FixedPoint_64,
    Divide_32,
    Divide_64,
  };

  FastBinning() {}

  FastBinning(uint64_t divisor, uint64_t max_duration_);

  template <Mode mode> uint64_t divide(uint64_t duration) const {
    assert(duration <= max_duration);
    uint64_t out;
    switch (mode) {
    case Mode::ConstZero:
      out = 0;
      break;
    case Mode::Dividend:
      out = duration;
      break;
    case Mode::PowerOfTwo:
      out = duration >> bits_shift;
      break;
    case Mode::FixedPoint_32:
      out = (duration * factor) >> 32;
      break;
    case Mode::FixedPoint_64:
      out = MulHigh(duration, factor);
      break;
    case Mode::Divide_32:
      out = uint32_t(duration) / uint32_t(divisor);
      break;
    case Mode::Divide_64:
      out = duration / divisor;
      break;
    }
    assert(out == duration / divisor);
    return out;
  }

  Mode getMode() const { return mode; }

private:
  // returns (a*b) >> 64 in a generic but accelerated way
  uint64_t MulHigh(uint64_t a, uint64_t b) const {
#ifdef __SIZEOF_INT128__
    return ((unsigned __int128)a * (unsigned __int128)b) >> 64; // GCC, clang, ...
#elif defined(_M_X64) || defined(_M_ARM64)
    return __umulh(a, b); // MSVC
#else
    // Generic fallback
    uint64_t a_lo = uint32_t(a);
    uint64_t a_hi = a >> 32;
    uint64_t b_lo = uint32_t(b);
    uint64_t b_hi = b >> 32;

    uint64_t a_x_b_hi = a_hi * b_hi;
    uint64_t a_x_b_mid = a_hi * b_lo;
    uint64_t b_x_a_mid = b_hi * a_lo;
    uint64_t a_x_b_lo = a_lo * b_lo;

    uint64_t carry_bit = ((uint64_t)(uint32_t)a_x_b_mid + (uint64_t)(uint32_t)b_x_a_mid + (a_x_b_lo >> 32)) >> 32;

    uint64_t multhi = a_x_b_hi + (a_x_b_mid >> 32) + (b_x_a_mid >> 32) + carry_bit;

    return multhi;
#endif
  }

  uint64_t divisor;
  uint64_t max_duration;
  uint64_t factor;
  int bits_shift;
  Mode mode;
};

/// FastBinning caller helper
#define BINNING_TEMPLATE_HELPER(fun_name, binner, ...)                                                                 \
  switch (binner.getMode()) {                                                                                          \
  case FastBinning::Mode::ConstZero:                                                                                   \
    fun_name<FastBinning::Mode::ConstZero>(__VA_ARGS__);                                                               \
    break;                                                                                                             \
  case FastBinning::Mode::Dividend:                                                                                    \
    fun_name<FastBinning::Mode::Dividend>(__VA_ARGS__);                                                                \
    break;                                                                                                             \
  case FastBinning::Mode::PowerOfTwo:                                                                                  \
    fun_name<FastBinning::Mode::PowerOfTwo>(__VA_ARGS__);                                                              \
    break;                                                                                                             \
  case FastBinning::Mode::FixedPoint_32:                                                                               \
    fun_name<FastBinning::Mode::FixedPoint_32>(__VA_ARGS__);                                                           \
    break;                                                                                                             \
  case FastBinning::Mode::FixedPoint_64:                                                                               \
    fun_name<FastBinning::Mode::FixedPoint_64>(__VA_ARGS__);                                                           \
    break;                                                                                                             \
  case FastBinning::Mode::Divide_32:                                                                                   \
    fun_name<FastBinning::Mode::Divide_32>(__VA_ARGS__);                                                               \
    break;                                                                                                             \
  case FastBinning::Mode::Divide_64:                                                                                   \
    fun_name<FastBinning::Mode::Divide_64>(__VA_ARGS__);                                                               \
    break;                                                                                                             \
  }

/**
 * \defgroup ITERATOR base iterators
 * \brief base iterators for photon counting applications
 */

class CombinerImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief Combine some channels in a virtual channel which has a tick for each tick in the input channels
 *
 * This iterator can be used to get aggregation channels, eg if you want to monitor the countrate
 * of the sum of two channels.
 */
class TT_API Combiner : public IteratorBase {
public:
  /**
   * \brief construct a combiner
   *
   * \param tagger    reference to a TimeTagger
   * \param channels    vector of channels to combine
   */
  Combiner(TimeTaggerBase *tagger, std::vector<channel_t> channels);

  ~Combiner();

  /**
   * \brief get sum of counts
   *
   * For reference, this iterators sums up how much ticks are generated because of which input channel.
   * So this functions returns an array with one value per input channel.
   *
   */
  GET_DATA_1D(getData, int64_t, array_out, );

  /**
   * \brief the new virtual channel
   *
   * This function returns the new allocated virtual channel.
   * It can be used now in any new iterator.
   *
   */
  channel_t getChannel();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;

private:
  friend class CombinerImpl;
  std::unique_ptr<CombinerImpl> impl;
};

class CountBetweenMarkersImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief a simple counter where external marker signals determine the bins
 *
 * Counter with external signals that trigger beginning and end of each counter
 * accumulation. This can be used to implement counting triggered by a pixel
 * clock and gated counting. The thread waits for the first time tag on the
 * 'begin_channel', then begins counting time tags on the 'click_channel'.
 * It ends counting when a tag on the 'end_channel' is detected.
 *
 */
class TT_API CountBetweenMarkers : public IteratorBase {
public:
  /**
   * \brief constructor of CountBetweenMarkers
   *
   *
   * @param tagger         reference to a TimeTagger
   * @param click_channel  channel that increases the count
   * @param begin_channel  channel that triggers beginning of counting and stepping to the next value
   * @param end_channel    channel that triggers end of counting
   * @param n_values       the number of counter values to be stored
   */
  CountBetweenMarkers(TimeTaggerBase *tagger, channel_t click_channel, channel_t begin_channel,
                      channel_t end_channel = CHANNEL_UNUSED, int32_t n_values = 1000);

  ~CountBetweenMarkers();

  /**
   * \brief tbd
   *
   */
  bool ready();

  /**
   * \brief tbd
   */
  GET_DATA_1D(getData, int32_t, array_out, );

  /**
   * \brief fetches the widths of each bins
   */
  GET_DATA_1D(getBinWidths, timestamp_t, array_out, );

  /**
   * \brief fetches the starting time of each bin
   */
  GET_DATA_1D(getIndex, timestamp_t, array_out, );

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;

private:
  friend class CountBetweenMarkersImpl;
  std::unique_ptr<CountBetweenMarkersImpl> impl;
};

class CounterImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief a simple counter on one or more channels
 *
 * Counter with fixed binwidth and circular buffer output. This class
 * is suitable to generate a time trace of the count rate on one or more channels.
 * The thread repeatedly counts clicks on a single channel over a
 * given time interval and stores the results in a two-dimensional array.
 * The array is treated as a circular buffer. I.e., once the array is
 * full, each new value shifts all previous values one element to the left.
 *
 */
class TT_API Counter : public IteratorBase {
public:
  /**
   * \brief construct a counter
   *
   * \param tagger       reference to a TimeTagger
   * \param channels     channels to count on
   * \param binwidth     counts are accumulated for binwidth picoseconds
   * \param n_values     number of counter values stored (for each channel)
   */
  Counter(TimeTaggerBase *tagger, std::vector<channel_t> channels, timestamp_t binwidth = 1000000000,
          int32_t n_values = 1);

  ~Counter();

  /**
   * \brief get counts
   *
   * the counts are copied to a newly allocated allocated memory, an the
   * pointer to this location is returned.
   *
   */
  GET_DATA_2D(getData, int32_t, array_out, );

  GET_DATA_1D(getIndex, timestamp_t, array_out, );

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;
  void on_start() override;

private:
  friend class CounterImpl;
  std::unique_ptr<CounterImpl> impl;
};

/**
 * type of timestamp for the Coincidence virtual channel (Last, Average, First, ListedFirst)
 */
enum class CoincidenceTimestamp : uint32_t {
  Last = 0,        // time of the last event completing the coincidence (fastest option - default)
  Average = 1,     // average time of all tags completing the coincidence
  First = 2,       // time of the first event received of the coincidence
  ListedFirst = 3, // time of the first channel of the list with which the Coincidence was initialized
};

class CoincidencesImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief a coincidence monitor for one or more channel groups
 *
 * Monitor coincidences for given coincidence groups passed by the constructor.
 * A coincidence is hereby defined as for a given coincidence group
 * a) the incoming is part of this group
 * b) at least tag arrived within the coincidenceWindow [ps] for all other channels of this coincidence group
 * Each coincidence will create a virtual event.
 * The block of event IDs for those coincidence group can be fetched.
 */
class TT_API Coincidences : public IteratorBase {
public:
  /**
   * \brief construct a Coincidences
   *
   * \param tagger               reference to a TimeTagger
   * \param coincidenceGroups    a vector of channels defining the coincidences
   * \param coincidenceWindow    the size of the coincidence window in picoseconds
   * \param timestamp            type of timestamp for virtual channel (Last, Average, First, ListedFirst)
   */
  Coincidences(TimeTaggerBase *tagger, std::vector<std::vector<channel_t>> coincidenceGroups,
               timestamp_t coincidenceWindow, CoincidenceTimestamp timestamp = CoincidenceTimestamp::Last);

  ~Coincidences();

  /**
   * \brief fetches the block of virtual channels for those coincidence groups
   */
  std::vector<channel_t> getChannels();

  void setCoincidenceWindow(timestamp_t coincidenceWindow);

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;

private:
  friend class CoincidencesImpl;
  std::unique_ptr<CoincidencesImpl> impl;
};

/**
 * \ingroup ITERATOR
 *
 * \brief a coincidence monitor for one or more channel groups
 *
 * Monitor coincidences for a given channel groups passed by the constructor.
 * A coincidence is event is detected when all slected channels have a click
 * within the given coincidenceWindow [ps]
 * The coincidence will create a virtual events on a virtual channel with
 * the channel number provided by getChannel().
 * For multiple coincidence channel combinations use the class Coincidences
 * which outperformes multiple instances of Conincdence.
 */
class TT_API Coincidence : public Coincidences {
public:
  /**
   * \brief construct a coincidence
   *
   * \param tagger    reference to a TimeTagger
   * \param channels    vector of channels to match
   * \param coincidenceWindow    max distance between all clicks for a coincidence [ps]
   * \param timestamp  type of timestamp for virtual channel (Last, Average, First, ListedFirst)
   */
  Coincidence(TimeTaggerBase *tagger, std::vector<channel_t> channels, timestamp_t coincidenceWindow = 1000,
              CoincidenceTimestamp timestamp = CoincidenceTimestamp::Last)
      : Coincidences(tagger, {channels}, coincidenceWindow, timestamp) {}

  /**
   * \brief virtual channel which contains the coincidences
   */
  channel_t getChannel() { return getChannels()[0]; }
};

class CountrateImpl;
/**
 * \ingroup ITERATOR
 * \brief count rate on one or more channels
 *
 * Measures the average count rate on one or more channels. Specifically, it counts
 * incoming clicks and determines the time between the initial click and the latest click.
 * The number of clicks divided by the time corresponds to the average countrate
 * since the initial click.
 *
 */
class TT_API Countrate : public IteratorBase {
public:
  /**
   * \brief constructor of Countrate
   *
   * @param tagger    reference to a TimeTagger
   * @param channels    the channels to count on
   */
  Countrate(TimeTaggerBase *tagger, std::vector<channel_t> channels);

  ~Countrate();

  /**
   * \brief get the count rates
   *
   * Returns the average rate of events per second per channel as an array.
   */
  GET_DATA_1D(getData, double, array_out, );

  /**
   * \brief get the total amount of events
   *
   * Returns the total amount of events per channel as an array.
   */
  GET_DATA_1D(getCountsTotal, int64_t, array_out, );

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;
  void on_start() override;

private:
  friend class CountrateImpl;
  std::unique_ptr<CountrateImpl> impl;
};

class DelayedChannelImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief a simple delayed queue
 *
 * A simple first-in first-out queue of delayed event timestamps.
 */
class TT_API DelayedChannel : public IteratorBase {
public:
  /**
   * \brief constructor of a DelayedChannel
   *
   * \param tagger                reference to a TimeTagger
   * \param input_channel         channel which is delayed
   * \param delay                 amount of time to delay
   */
  DelayedChannel(TimeTaggerBase *tagger, channel_t input_channel, timestamp_t delay);

#ifndef SWIG
  /**
   * \brief constructor of a DelayedChannel for delaying many channels at once
   *
   * This function is not exposed to Python/C#/Matlab/Labview
   *
   * \param tagger                reference to a TimeTagger
   * \param input_channels        channels which will be delayed
   * \param delay                 amount of time to delay
   */
  DelayedChannel(TimeTaggerBase *tagger, std::vector<channel_t> input_channels, timestamp_t delay);
#endif

  ~DelayedChannel();

  /**
   * \brief the first new virtual channel
   *
   * This function returns the first of the new allocated virtual channels.
   * It can be used now in any new iterator.
   */
  channel_t getChannel();

#ifndef SWIG
  /**
   * \brief the new virtual channels
   *
   * This function returns the new allocated virtual channels.
   * It can be used now in any new iterator.
   */
  std::vector<channel_t> getChannels();
#endif

  /**
   * \brief set the delay time delay for the cloned tags in the virtual channels. A negative delay will delay all other
   * events.
   *
   * Note: When the delay is the same or greater than the previous value all incoming tags will be visible at virtual
   * channel. By applying a shorter delay time, the tags stored in the local buffer will be flushed and won't be visible
   * in the virtual channel.
   */
  void setDelay(timestamp_t delay);

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void on_start() override;

private:
  friend class DelayedChannelImpl;
  std::unique_ptr<DelayedChannelImpl> impl;
};

class TriggerOnCountrateImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief Inject trigger events when exceeding or falling below a given count rate within a rolling time window.
 *
 * Measures the count rate inside a rolling time window and emits tags when a given reference_countrate is crossed.
 * A `TriggerOnCountrate` object provides two virtual channels: The `above` channel is triggered when the count rate
 * exceeds the threshold (transition from `below` to `above`). The `below` channel is triggered when the count rate
 * falls below the threshold (transition from `above` to `below`). To avoid the emission of multiple trigger tags in the
 * transition area, the `hysteresis` count rate modifies the threshold with respect to the transition direction: An
 * event in the `above` channel will be triggered when the channel is in the `below` state and rises to
 * `reference_countrate + hysteresis` or above. Vice versa, the `below` channel fires when the channel is in the `above`
 * state and falls to the limit of `reference_countrate - hysteresis` or below.
 *
 * The time-tags are always injected at the end of the integration window.
 * You can use the `DelayedChannel` to adjust the temporal position of the trigger tags with respect to the integration
 * time window.
 *
 * The very first tag of the virtual channel will be emitted `time_window` after the instantiation of the object and
 * will reflect the current state, so either `above` or `below`.
 */
class TT_API TriggerOnCountrate : public IteratorBase {
public:
  /**
   * \brief constructor of a TriggerOnCountrate
   *
   * \param tagger                Reference to a TimeTagger object.
   * \param input_channel         Channel number of the channel whose count rate will control the trigger channels.
   * \param reference_countrate   The reference count rate in Hz that separates the `above` range from the `below`
   * range.
   * \param hysteresis            The threshold count rate in Hz for transitioning to the `above` threshold state
   *                              is `countrate >= reference_countrate + hysteresis`, whereas it is `countrate <=
   *                              reference_countrate - hysteresis` for transitioning to the `below` threshold state.
   *                              The hysteresis avoids the emission of multiple trigger tags upon a single transition.
   * \param time_window           Rolling time window size in ps. The count rate is analyzed within this time window and
   *                              compared to the threshold count rate.
   */
  TriggerOnCountrate(TimeTaggerBase *tagger, channel_t input_channel, double reference_countrate, double hysteresis,
                     timestamp_t time_window);

  ~TriggerOnCountrate();

  /**
   * \brief Get the channel number of the `above` channel.
   */
  channel_t getChannelAbove();

  /**
   * \brief Get the channel number of the `below` channel.
   */
  channel_t getChannelBelow();

  /**
   * \brief Get both virtual channel numbers: [`getChannelAbove()`, `getChannelBelow()`]
   */
  std::vector<channel_t> getChannels();

  /**
   * \brief Returns whether the Virtual Channel is currently in the `above` state.
   */
  bool isAbove();

  /**
   * \brief Returns whether the Virtual Channel is currently in the `below` state.
   */
  bool isBelow();

  /**
   * \brief Get the current count rate averaged within the `time_window`.
   */
  double getCurrentCountrate();

  /**
   * \brief Emit a time-tag into the respective channel according to the current state.
   *
   * Emit a time-tag into the respective channel according to the current state.
   * This is useful if you start a new measurement that requires the information.
   * The function returns whether it was possible to inject the event.
   * The injection is not possible if the Time Tagger is in overflow mode or the time window has not passed yet.
   * The function call is non-blocking.
   */
  bool injectCurrentState();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void on_start() override;

private:
  friend class TriggerOnCountrateImpl;
  std::unique_ptr<TriggerOnCountrateImpl> impl;
};

class GatedChannelImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief An input channel is gated by a gate channel.
 *
 * Note: The gate is edge sensitive and not level sensitive. That means that the gate will transfer data only when an
 * appropriate level change is detected on the gate_start_channel.
 */
class TT_API GatedChannel : public IteratorBase {
public:
  /**
   * \brief constructor of a GatedChannel
   *
   * \param tagger                reference to a TimeTagger
   * \param input_channel         channel which is gated
   * \param gate_start_channel    channel on which a signal detected will start the transmission of the input_channel
   * through the gate \param gate_stop_channel     channel on which a signal detected will stop the transmission of the
   * input_channel through the gate
   */
  GatedChannel(TimeTaggerBase *tagger, channel_t input_channel, channel_t gate_start_channel,
               channel_t gate_stop_channel);

  ~GatedChannel();

  /**
   * \brief the new virtual channel
   *
   * This function returns the new allocated virtual channel.
   * It can be used now in any new iterator.
   *
   */
  channel_t getChannel();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;

private:
  friend class GatedChannelImpl;
  std::unique_ptr<GatedChannelImpl> impl;
};

class FrequencyMultiplierImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief The signal of an input channel is scaled up to a higher frequency according to the multiplier passed as a
 * parameter.
 *
 * The FrequencyMultiplier inserts copies the original input events from the input_channel and adds additional events to
 * match the upscaling factor. The algorithm used assumes a constant frequency and calculates out of the last two
 * incoming events linearly the intermediate timestamps to match the upscaled frequency given by the multiplier
 * parameter.
 *
 * The FrequencyMultiplier can be used to restore the actual frequency applied to an input_channel which was reduces via
 * the EventDivider to lower the effective data rate. For example a 80 MHz laser sync signal can be scaled down via
 * setEventDivider(..., 80) to 1 MHz (hardware side) and an 80 MHz signal can be restored via FrequencyMultiplier(...,
 * 80) on the software side with some loss in precision. The FrequencyMultiplier is an alternative way to reduce the
 * data rate in comparison to the EventFilter, which has a higher precision but can be more difficult to use.
 */
class TT_API FrequencyMultiplier : public IteratorBase {
public:
  /**
   * \brief constructor of a FrequencyMultiplier
   *
   * \param tagger                reference to a TimeTagger
   * \param input_channel         channel on which the upscaling of the frequency is based on
   * \param multiplier            frequency upscaling factor
   */
  FrequencyMultiplier(TimeTaggerBase *tagger, channel_t input_channel, int32_t multiplier);

  ~FrequencyMultiplier();

  channel_t getChannel();
  int32_t getMultiplier();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;

private:
  friend class FrequencyMultiplierImpl;
  std::unique_ptr<FrequencyMultiplierImpl> impl;
};

class IteratorImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief a simple event queue
 *
 * A simple Iterator, just keeping a first-in first-out queue of event timestamps.
 *
 * \deprecated use TimeTagStream
 */
class TT_API Iterator : public IteratorBase {
public:
  /**
   * \brief standard constructor
   *
   * @param tagger        the backend
   * @param channel       the channel to get events from
   */
  Iterator(TimeTaggerBase *tagger, channel_t channel);

  ~Iterator();

  /**
   * \brief get next timestamp
   *
   * get the next timestamp from the queue.
   */
  timestamp_t next();

  /**
   * \brief get queue size
   */
  uint64_t size();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;

private:
  friend class IteratorImpl;
  std::unique_ptr<IteratorImpl> impl;
};

class TimeTagStreamImpl;
class FileReaderImpl;
class TT_API TimeTagStreamBuffer {
  friend class TimeTagStreamImpl;
  friend class FileReaderImpl;

public:
  GET_DATA_1D(getOverflows, unsigned char, array_out, ); // deprecated, please use getEventTypes instead
  GET_DATA_1D(getChannels, channel_t, array_out, );
  GET_DATA_1D(getTimestamps, timestamp_t, array_out, );
  GET_DATA_1D(getMissedEvents, unsigned short, array_out, );
  GET_DATA_1D(getEventTypes, unsigned char, array_out, );

  uint64_t size;
  bool hasOverflows;
  timestamp_t tStart;
  timestamp_t tGetData;

private:
  std::vector<channel_t> tagChannels;
  std::vector<timestamp_t> tagTimestamps;
  std::vector<unsigned short> tagMissedEvents;
  std::vector<Tag::Type> tagTypes;
};

/**
 * \ingroup ITERATOR
 * \brief access the time tag stream
 */
class TT_API TimeTagStream : public IteratorBase {
public:
  /**
   * \brief constructor of a TimeTagStream thread
   *
   * Gives access to the time tag stream
   *
   * @param tagger      reference to a TimeTagger
   * @param n_max_events    maximum number of tags stored
   * @param channels      channels which are dumped to the file (when empty or not passed all active channels are
   * dumped)
   */
  TimeTagStream(TimeTaggerBase *tagger, uint64_t n_max_events,
                std::vector<channel_t> channels = std::vector<channel_t>());

  /**
   * \brief tbd
   *
   */
  ~TimeTagStream();

  /**
   * \brief get incoming time tags
   *
   * All incoming time tags are stored in a buffer (max size: max_tags). The buffer is cleared after retrieving the data
   * with getData()
   *
   */

  /**
   * \brief return the number of stored tags
   *
   */
  uint64_t getCounts();

  /**
   * \brief fetches all stored tags and clears the internal state
   */
  TimeTagStreamBuffer getData();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;

private:
  friend class TimeTagStreamImpl;
  std::unique_ptr<TimeTagStreamImpl> impl;
};

class DumpImpl;
/**
 * \ingroup ITERATOR
 * \brief dump all time tags to a file
 *
 * @deprecated use FileWriter
 */
class TT_API Dump : public IteratorBase {
public:
  /**
   * \brief constructor of a Dump thread
   *
   * @param tagger        reference to a TimeTagger
   * @param filename      name of the file to dump to
   * @param max_tags      stop after this number of tags has been dumped. Negative values will dump forever
   * @param channels      channels which are dumped to the file (when empty or not passed all active channels are
   * dumped)
   */
  Dump(TimeTaggerBase *tagger, std::string filename, int64_t max_tags,
       std::vector<channel_t> channels = std::vector<channel_t>());

  /**
   * \brief tbd
   *
   */
  ~Dump();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;
  void on_start() override;
  void on_stop() override;

private:
  friend class DumpImpl;
  std::unique_ptr<DumpImpl> impl;
};

class StartStopImpl;
/**
 * @ingroup ITERATOR
 *
 * \brief simple start-stop measurement
 *
 * This class performs a start-stop measurement between two channels
 * and stores the time differences in a histogram. The histogram resolution
 * is specified beforehand (binwidth) but the histogram range is unlimited.
 * It is adapted to the largest time difference that was detected. Thus
 * all pairs of subsequent clicks are registered.
 *
 * Be aware, on long-running measurements this may considerably slow down
 * system performance and even crash the system entirely when attached to an
 * unsuitable signal source.
 *
 */
class TT_API StartStop : public IteratorBase {
public:
  /**
   * \brief constructor of StartStop
   *
   * @param tagger                reference to a TimeTagger
   * @param click_channel         channel for stop clicks
   * @param start_channel         channel for start clicks
   * @param binwidth              width of one histogram bin in ps
   */
  StartStop(TimeTaggerBase *tagger, channel_t click_channel, channel_t start_channel = CHANNEL_UNUSED,
            timestamp_t binwidth = 1000);

  ~StartStop();

  GET_DATA_2D(getData, timestamp_t, array_out, );

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;
  void on_start() override;

private:
  friend class StartStopImpl;
  std::unique_ptr<StartStopImpl> impl;
};

template <typename T> class TimeDifferencesImpl;
/**
 * @ingroup ITERATOR
 * \brief Accumulates the time differences between clicks on two channels in one or more histograms.
 *
 * A multidimensional histogram measurement with the option up to include three additional channels
 * that control how to step through the indices of the histogram array. This is a very powerful and
 * generic measurement. You can use it to record cross-correlation, lifetime measurements, fluorescence
 * lifetime imaging and many more measurements based on pulsed excitation. Specifically, the measurement
 * waits for a tag on the ‘start_channel’, then measures the time difference between the start tag and
 * all subsequent tags on the ‘click_channel’ and stores them in a histogram. If no ‘start_channel’ is
 * specified, the ‘click_channel’ is used as ‘start_channel’ corresponding to an auto-correlation
 * measurement. The histogram has a number ‘n_bins’ of bins of bin width ‘binwidth’. Clicks that fall
 * outside the histogram range are discarded. Data accumulation is performed independently for all start
 * tags. This type of measurement is frequently referred to as ‘multiple start, multiple stop’
 * measurement and corresponds to a full auto- or cross-correlation measurement.
 *
 * The data obtained from subsequent start tags can be accumulated into the same histogram (one-
 * dimensional measurement) or into different histograms (two-dimensional measurement). In this way, you
 * can perform more general two-dimensional time-difference measurements. The parameter ‘n_histograms’
 * specifies the number of histograms. After each tag on the ‘next_channel’, the histogram index is
 * incremented by one and reset to zero after reaching the last valid index. The measurement starts with
 * the first tag on the ‘next_channel’.
 *
 * You can also provide a synchronization trigger that resets the histogram index by specifying a
 * ‘sync_channel’. The measurement starts when a tag on the ‘sync_channel’ arrives with a subsequent tag
 * on ‘next_channel’. When a rollover occurs, the accumulation is stopped until the next sync and
 * subsequent next signal. A sync signal before a rollover will stop the accumulation, reset the
 * histogram index and a subsequent signal on the ‘next_channel’ starts the accumulation again.
 *
 * Typically, you will run the measurement indefinitely until stopped by the user. However, it is also
 * possible to specify the maximum number of rollovers of the histogram index. In this case the
 * measurement stops when the number of rollovers has reached the specified value. This means that for
 * both a one-dimensional and for a two-dimensional measurement, it will measure until the measurement
 * went through the specified number of rollovers / sync tags.
 */
class TT_API TimeDifferences : public IteratorBase {
public:
  /**
   * \brief constructor of a TimeDifferences measurement
   *
   *
   * \param tagger                reference to a TimeTagger
   * \param click_channel         channel that increments the count in a bin
   * \param start_channel         channel that sets start times relative to which clicks on the click channel are
   * measured \param next_channel          channel that increments the histogram index \param sync_channel channel that
   * resets the histogram index to zero \param binwidth              width of one histogram bin in ps \param n_bins
   * number of bins in each histogram \param n_histograms          number of histograms
   */
  TimeDifferences(TimeTaggerBase *tagger, channel_t click_channel, channel_t start_channel = CHANNEL_UNUSED,
                  channel_t next_channel = CHANNEL_UNUSED, channel_t sync_channel = CHANNEL_UNUSED,
                  timestamp_t binwidth = 1000, int32_t n_bins = 1000, int32_t n_histograms = 1);

  ~TimeDifferences();

  /**
   * \brief returns a two-dimensional array of size ‘n_bins’ by ‘n_histograms’ containing the histograms
   */
  GET_DATA_2D(getData, int32_t, array_out, );

  /**
   * \brief returns a vector of size ‘n_bins’ containing the time bins in ps
   */
  GET_DATA_1D(getIndex, timestamp_t, array_out, );

  /**
   * \brief set the number of rollovers at which the measurement stops integrating
   *
   * \param max_counts             maximum number of sync/next clicks
   */
  void setMaxCounts(uint64_t max_counts);

  /**
   * \brief returns the number of rollovers (histogram index resets)
   */
  uint64_t getCounts();

  /**
   * \brief returns ‘true’ when the required number of rollovers set by ‘setMaxCounts’ has been reached
   */
  bool ready();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;
  void on_start() override;

private:
  friend class TimeDifferencesImpl<TimeDifferences>;
  std::unique_ptr<TimeDifferencesImpl<TimeDifferences>> impl;
};

class Histogram2DImpl;
/**
 * @ingroup ITERATOR
 * \brief A 2-dimensional histogram of time differences. This can be used in measurements similar to 2D NRM spectrocopy.
 *
 * This measurement is a 2-dimensional version of the Histogram measurement.
 * The measurement accumulates two-dimensional histogram where stop signals from two
 * separate channels define the bin coordinate. For instance, this kind of measurement
 * is similar to that of typical 2D NMR spectroscopy.
 */
class TT_API Histogram2D : public IteratorBase {
public:
  /**
   * \brief constructor of a Histogram2D measurement
   * \param tagger time tagger object
   * \param start_channel channel on which start clicks are received
   * \param stop_channel_1 channel on which stop clicks for the time axis 1 are received
   * \param stop_channel_2 channel on which stop clicks for the time axis 2 are received
   * \param binwidth_1 bin width in ps for the time axis 1
   * \param binwidth_2 bin width in ps for the time axis 2
   * \param n_bins_1 the number of bins along the time axis 1
   * \param n_bins_2 the number of bins along the time axis 2
   */
  Histogram2D(TimeTaggerBase *tagger, channel_t start_channel, channel_t stop_channel_1, channel_t stop_channel_2,
              timestamp_t binwidth_1, timestamp_t binwidth_2, int32_t n_bins_1, int32_t n_bins_2);
  ~Histogram2D();

  /**
   * Returns a two-dimensional array of size n_bins_1 by n_bins_2 containing the 2D histogram.
   */
  GET_DATA_2D(getData, int32_t, array_out, );

  /**
   * Returns a 3D array containing two coordinate matrices (meshgrid) for time bins in ps for the time axes 1 and 2.
   * For details on meshgrid please take a look at the respective documentation either for Matlab or Python NumPy
   */
  GET_DATA_3D(getIndex, timestamp_t, array_out, );

  /**
   * Returns a vector of size n_bins_1 containing the bin locations in ps for the time axis 1.
   */
  GET_DATA_1D(getIndex_1, timestamp_t, array_out, );

  /**
   * Returns a vector of size `n_bins_2` containing the bin locations in ps for the time axis 2.
   */
  GET_DATA_1D(getIndex_2, timestamp_t, array_out, );

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;

private:
  friend class Histogram2DImpl;
  std::unique_ptr<Histogram2DImpl> impl;
};

class TimeDifferencesNDImpl;
/**
 * @ingroup ITERATOR
 * \brief Accumulates the time differences between clicks on two channels in a multi-dimensional histogram.
 *
 * This is a multidimensional implementation of the TimeDifferences measurement class. Please read their documentation
 * first.
 *
 * This measurement class extends the TimeDifferences interface for a multidimensional amount of histograms.
 * It captures many multiple start - multiple stop histograms, but with many asynchronous next_channel triggers.
 * After each tag on each next_channel, the histogram index of the associated dimension is
 * incremented by one and reset to zero after reaching the last valid index.  The elements of the parameter n_histograms
 * specifies the number of histograms per dimension. The accumulation starts when next_channel has been triggered on all
 * dimensions.
 *
 * You should provide a synchronization trigger by specifying a sync_channel per dimension.
 * It will stop the accumulation when an associated histogram index rollover occurs.
 * A sync event will also stop the accumulation, reset the histogram index of the associated dimension,
 * and a subsequent event on the corresponding next_channel starts the accumulation again.
 * The synchronization is done asynchronous, so an event on the next_channel increases the histogram index even if the
 * accumulation is stopped. The accumulation starts when a tag on the sync_channel arrives with a subsequent tag on
 * next_channel for all dimensions.
 *
 * Please use setInputDelay to adjust the latency of all channels. In general, the order of the provided triggers
 * including maximum jitter should be: old start trigger -- all sync triggers -- all next triggers -- new start trigger
 */
class TT_API TimeDifferencesND : public IteratorBase {
public:
  /**
   * \brief constructor of a TimeDifferencesND measurement
   *
   *
   * \param tagger                reference to a TimeTagger
   * \param click_channel         channel that increments the count in a bin
   * \param start_channel         channel that sets start times relative to which clicks on the click channel are
   *                              measured
   * \param next_channels         vector of channels that increments the histogram index \param sync_channels
   *                              vector of channels that resets the histogram index to zero
   * \param n_histograms          vector of numbers of histograms per dimension.
   * \param binwidth              width of one histogram bin in ps \param n_bins number of bins
   *                              in each histogram
   */
  TimeDifferencesND(TimeTaggerBase *tagger, channel_t click_channel, channel_t start_channel,
                    std::vector<channel_t> next_channels, std::vector<channel_t> sync_channels,
                    std::vector<int32_t> n_histograms, timestamp_t binwidth, int32_t n_bins);

  ~TimeDifferencesND();

  /**
   * \brief returns a two-dimensional array of size n_bins by all n_histograms containing the histograms
   */
  GET_DATA_2D(getData, int32_t, array_out, );

  /**
   * \brief returns a vector of size n_bins containing the time bins in ps
   */
  GET_DATA_1D(getIndex, timestamp_t, array_out, );

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;
  void on_start() override;

private:
  friend class TimeDifferencesNDImpl;
  std::unique_ptr<TimeDifferencesNDImpl> impl;
};

/**
 * @ingroup ITERATOR
 *
 * \brief Accumulate time differences into a histogram
 *
 * This is a simple multiple start, multiple stop measurement. This is a special
 * case of the more general 'TimeDifferences' measurement.
 *    Specifically, the thread waits for clicks on a first channel, the 'start channel',
 * then measures the time difference between the last start click and all subsequent
 * clicks on a second channel, the 'click channel', and stores them in a histogram.
 * The histogram range and resolution is specified by the number of bins and the binwidth.
 * Clicks that fall outside the histogram range are ignored.
 * Data accumulation is performed independently for all start clicks. This type of measurement
 * is frequently referred to as 'multiple start, multiple stop' measurement and corresponds to a
 * full auto- or cross-correlation measurement.
 */
class TT_API Histogram : public IteratorBase {
public:
  /**
   * \brief constructor of a Histogram measurement
   *
   *
   * \param tagger                reference to a TimeTagger
   * \param click_channel         channel that increments the count in a bin
   * \param start_channel         channel that sets start times relative to which clicks on the click channel are
   * measured \param binwidth              width of one histogram bin in ps \param n_bins                number of bins
   * in the histogram
   */
  Histogram(TimeTaggerBase *tagger, channel_t click_channel, channel_t start_channel = CHANNEL_UNUSED,
            timestamp_t binwidth = 1000, int32_t n_bins = 1000);

  ~Histogram();

  GET_DATA_1D(getData, int32_t, array_out, );

  GET_DATA_1D(getIndex, timestamp_t, array_out, );

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;
  void on_start() override;

private:
  friend class TimeDifferencesImpl<Histogram>;
  std::unique_ptr<TimeDifferencesImpl<Histogram>> impl;
};

class HistogramLogBinsImpl;
/**
 * @ingroup ITERATOR
 *
 * \brief Accumulate time differences into a histogram with logarithmic increasing bin sizes
 *
 * This is a multiple start, multiple stop measurement, and works the very same way as the histogram measurement but
 * with logarithmic increasing bin widths. After initializing the measurement (or after an overflow) no data is
 * accumulated in the histogram until the full histogram duration has passed to ensure a balanced count accumulation
 * over the full histogram.
 *
 */
class TT_API HistogramLogBins : public IteratorBase {
public:
  /**
   * \brief constructor of a HistogramLogBins measurement
   *
   * \param tagger                reference to a TimeTagger
   * \param click_channel         channel that increments the count in a bin
   * \param start_channel         channel that sets start times relative to which clicks on the click channel are
   *                              measured
   * \param exp_start             exponent for the lowest time diffrences in the histogram: 10^exp_start s,
   *                              lowest exp_start: -12 => 1ps
   * \param exp_stop              exponent for the highest time diffrences in the histogram: 10^exp_stop s
   * \param n_bins                total number of bins in the histogram
   */
  HistogramLogBins(TimeTaggerBase *tagger, channel_t click_channel, channel_t start_channel, double exp_start,
                   double exp_stop, int32_t n_bins);
  ~HistogramLogBins();

  /**
   * returns the absolute counts for the bins
   */
  GET_DATA_1D(getData, uint64_t, array_out, );

  /**
   * returns the counts normalized by the binwidth of each bin
   */
  GET_DATA_1D(getDataNormalizedCountsPerPs, double, array_out, );

  /**
   * returns the counts normalized by the binwidth and the average count rate.
   * This matches the implementation of Correlation::getDataNormalized
   */
  GET_DATA_1D(getDataNormalizedG2, double, array_out, );

  /**
   * returns the edges of the bins in ps
   */
  GET_DATA_1D(getBinEdges, timestamp_t, array_out, );

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;

private:
  friend class HistogramLogBinsImpl;
  std::unique_ptr<HistogramLogBinsImpl> impl;
};

class CorrelationImpl;
/**
 * @ingroup ITERATOR
 *
 * \brief cross-correlation between two channels
 *
 * Accumulates time differences between clicks on two channels into
 * a histogram, where all ticks are considered both as start and stop
 * clicks and both positive and negative time differences are considered.
 * The histogram is determined by the number of total bins and the binwidth.
 *
 */
class TT_API Correlation : public IteratorBase {
public:
  /**
   * \brief constructor of a correlation measurement
   *
   * If channel_2 is left empty or set to CHANNEL_UNUSED, an auto-correlation measurement is performed.
   * This is the same as setting channel_2 = channel_1.
   *
   * \param tagger                reference to a TimeTagger
   * \param channel_1             first click channel
   * \param channel_2             second click channel
   * \param binwidth              width of one histogram bin in ps
   * \param n_bins                the number of bins in the resulting histogram
   */
  Correlation(TimeTaggerBase *tagger, channel_t channel_1, channel_t channel_2 = CHANNEL_UNUSED,
              timestamp_t binwidth = 1000, int n_bins = 1000);

  ~Correlation();

  /**
   * \brief returns a one-dimensional array of size n_bins containing the histogram
   */
  GET_DATA_1D(getData, int32_t, array_out, );

  /**
   * \brief get the histogram - normalized such that a perfectly uncorrelated signals would be flat at a height of one
   */
  GET_DATA_1D(getDataNormalized, double, array_out, );

  /**
   * \brief returns a vector of size n_bins containing the time bins in ps
   */
  GET_DATA_1D(getIndex, timestamp_t, array_out, );

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;

private:
  friend class CorrelationImpl;
  std::unique_ptr<CorrelationImpl> impl;
};

enum State {
  UNKNOWN,
  HIGH,
  LOW,
};
struct Event {
  timestamp_t time;
  State state;
};
class ScopeImpl;
/**
 * @ingroup ITERATOR
 * \brief
 *
 */
class TT_API Scope : public IteratorBase {
public:
  /**
   * \brief constructor of a Scope measurement
   *
   * \param tagger                reference to a TimeTagger
   * \param event_channels        channels which are captured
   * \param trigger_channel       channel that starts a new trace
   * \param window_size           window time of each trace
   * \param n_traces              amount of traces (n_traces < 1, automatic retrigger)
   * \param n_max_events          maximum number of tags in each trace
   */
  Scope(TimeTaggerBase *tagger, std::vector<channel_t> event_channels, channel_t trigger_channel,
        timestamp_t window_size = 1000000000, int32_t n_traces = 1, int32_t n_max_events = 1000);

  ~Scope();

  bool ready();

  int32_t triggered();

  std::vector<std::vector<Event>> getData();

  timestamp_t getWindowSize();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;

private:
  friend class ScopeImpl;
  std::unique_ptr<ScopeImpl> impl;
};

class TimeTaggerProxy;
/**
 * \ingroup ITERATOR
 * \brief start, stop and clear several measurements synchronized
 *
 * For the case that several measurements should be started, stopped or cleared at the very same time,
 * a SynchronizedMeasurements object can be create to which all the measurements (also called iterators)
 * can be registered with .registerMeasurement(measurement).
 * Calling .stop(), .start() or .clear() on the SynchronizedMeasurements object will call the respective
 * method on each of the registered measurements at the very same time. That means that all measurements
 * taking part will have processed the very same time tags.
 *
 **/
class TT_API SynchronizedMeasurements {
public:
  /**
   * \brief construct a SynchronizedMeasurements object
   *
   * \param tagger reference to a TimeTagger
   */
  SynchronizedMeasurements(TimeTaggerBase *tagger);

  ~SynchronizedMeasurements();

  /**
   * \brief register a measurement (iterator) to the SynchronizedMeasurements-group.
   *
   * All available methods called on the SynchronizedMeasurements will happen at the very same time for all the
   * registered measurements.
   */
  void registerMeasurement(IteratorBase *measurement);

  /**
   * \brief unregister a measurement (iterator) from the SynchronizedMeasurements-group.
   *
   * Stops synchronizing calls on the selected measurement, if the measurement is not within
   * this synchronized group, the method does nothing.
   */
  void unregisterMeasurement(IteratorBase *measurement);

  /**
   * \brief clear all registered measurements synchronously
   */
  void clear();

  /**
   * \brief start all registered measurements synchronously
   */
  void start();

  /**
   * \brief stop all registered measurements synchronously
   */
  void stop();

  /**
   * \brief start all registered measurements synchronously, and stops them after the capture_duration
   */
  void startFor(timestamp_t capture_duration, bool clear = true);

  /**
   * \brief wait until all registered measurements have finished running.
   *
   * \param timeout     time in milliseconds to wait for the measurements. If negative, wait until finished.
   *
   * waitUntilFinished will wait according to the timeout and return true if all measurements finished or false if not.
   * Furthermore, when waitUntilFinished is called on a set running indefinetly, it will log an error and return
   * inmediatly.
   */
  bool waitUntilFinished(int64_t timeout = -1);

  /**
   * \brief check if any iterator is running
   */
  bool isRunning();

  /**
   * Returns a proxy tagger object, which shall be used to create immediately registered measurements.
   * Those measurements will not start automatically.
   */
  TimeTaggerBase *getTagger();

protected:
  /**
   * \brief run a callback on all registered measurements synchronously
   *
   * Please keep in mind that the callback is copied for each measurement.
   * So please avoid big captures.
   */
  void runCallback(TimeTaggerBase::IteratorCallback callback, std::unique_lock<std::mutex> &lk, bool block = true);

private:
  friend class TimeTaggerProxy;

  void release();

  std::set<IteratorBase *> registered_measurements;
  std::mutex measurements_mutex;
  TimeTaggerBase *tagger;
  bool has_been_released = false;
  std::unique_ptr<TimeTaggerProxy> proxy;
};

class ConstantFractionDiscriminatorImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief a virtual CFD implementation which returns the mean time between a raising and a falling pair of edges
 *
 */
class TT_API ConstantFractionDiscriminator : public IteratorBase {
public:
  /**
   * \brief constructor of a ConstantFractionDiscriminator
   *
   * \param tagger                reference to a TimeTagger
   * \param channels              list of channels for the CFD, the formers of the raising+falling pairs must be given
   * \param search_window         interval for the CFD window, must be positive
   */
  ConstantFractionDiscriminator(TimeTaggerBase *tagger, std::vector<channel_t> channels, timestamp_t search_window);

  ~ConstantFractionDiscriminator();

  /**
   * \brief the list of new virtual channels
   *
   * This function returns the list of new allocated virtual channels.
   * It can be used now in any new measurement class.
   */
  std::vector<channel_t> getChannels();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void on_start() override;

private:
  friend class ConstantFractionDiscriminatorImpl;
  std::unique_ptr<ConstantFractionDiscriminatorImpl> impl;
};

class FileWriterImpl;
/**
 * \ingroup ITERATOR
 * \brief compresses and stores all time tags to a file
 */
class TT_API FileWriter : public IteratorBase {
public:
  /**
   * \brief constructor of a FileWriter
   * \param tagger        reference to a TimeTagger
   * \param filename      name of the file to store to
   * \param channels      channels which are stored to the file
   */
  FileWriter(TimeTaggerBase *tagger, const std::string &filename, std::vector<channel_t> channels);
  ~FileWriter();

  /**
   * Close the current file and create a new one
   * \param new_filename filename of the new file. If empty, the old one will be used.
   */
  void split(const std::string &new_filename = "");

  /**
   * Set the maximum file size on disk and so when the automatical split happens.
   * Note: This is a rough limit, the actual file might be larger by one block.
   * \param max_file_size new maximum file size in bytes
   */
  void setMaxFileSize(uint64_t max_file_size);

  /**
   * fetches the maximum file size. Please see setMaxFileSize for more details.
   * \return the maximum file size in bytes
   */
  uint64_t getMaxFileSize();

  /**
   * queries the total amount of events stored in all files
   * \return the total amount of events stored
   */
  uint64_t getTotalEvents();

  /**
   * queries the total amount of bytes stored in all files
   * \return the total amount of bytes stored
   */
  uint64_t getTotalSize();

  /**
   * writes a marker in the file. While parsing the file, the last marker can be extracted again.
   * \param marker the marker to write into the file
   */
  void setMarker(const std::string &marker);

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;
  void on_start() override;
  void on_stop() override;

private:
  friend class FileWriterImpl;
  std::unique_ptr<FileWriterImpl> impl;
};

/**
 * Reads tags from the disk files, which has been created by FileWriter.
 * Its usage is compatible with the TimeTagStream.
 */
class TT_API FileReader {
public:
  /**
   * Creates a file reader with the given filename.
   * The file reader automatically continues to read split FileWriter Streams
   * In case multiple filenames are given, the files will be read in successively.
   * \param filenames list of files to read
   */
  FileReader(std::vector<std::string> filenames);

  /**
   * Creates a file reader with the given filename.
   * The file reader automatically continues to read split FileWriter Streams
   * \param filename file to read
   */
  FileReader(const std::string &filename);
  ~FileReader();

  /**
   * Checks if there are still events in the FileReader
   * \return false if no more events can be read from this FileReader
   */
  bool hasData();

  /**
   * Fetches and delete the next tags from the internal buffer.
   * Every tag is returned exactly once. If less than n_events
   * are returned, the reader is at the end-of-files.
   * \param n_events maximum amount of elements to fetch
   * \return a TimeTagStreamBuffer with up to n_events events
   */
  TimeTagStreamBuffer getData(uint64_t n_events);

  /**
   * Low level file reading.
   * This function will return the next non-empty buffer in a raw format.
   * \param tag_buffer a buffer, which will be filled with the new events
   * \return true if fetching the data was successfully
   */
  bool getDataRaw(std::vector<Tag> &tag_buffer);

  /**
   * Fetches the overall configuration status of the Time Tagger object,
   * which was serialized in the current file.
   * \return a JSON serialized string with all configuration and status flags.
   */
  std::string getConfiguration();

  /**
   * return the last processed marker from the file.
   * \return the last marker from the file
   */
  std::string getLastMarker();

private:
  friend class FileReaderImpl;
  std::unique_ptr<FileReaderImpl> impl;
};

class EventGeneratorImpl;
/**
 * \ingroup ITERATOR
 *
 * \brief Generate predefined events in a virtual channel relative to a trigger event.
 *
 * This iterator can be used to generate a predefined series of events, the pattern, relative to a trigger event on a
 * defined channel. A trigger_divider can be used to fire the pattern not on every, but on every n'th trigger received.
 * The trigger_offset can be used to select on which of the triggers the pattern will be generated when trigger
 * trigger_divider is greater than 1. To abort the pattern being generated, a stop_channel can be defined. In case it is
 * the very same as the trigger_channel, the subsequent generated patterns will not overlap.
 */
class TT_API EventGenerator : public IteratorBase {
public:
  /**
   * \brief construct a event generator
   *
   * \param tagger          reference to a TimeTagger
   * \param trigger_channel trigger for generating the pattern
   * \param pattern         vector of time stamp generated relativ to the trigger event
   * \param trigger_divider establishes every how many trigger events a pattern is generated
   * \param divider_offset  the offset of the divided trigger when the pattern shall be emitted
   * \param stop_channel    channel on which a received event will stop all pending patterns from being generated
   */
  EventGenerator(TimeTaggerBase *tagger, channel_t trigger_channel, std::vector<timestamp_t> pattern,
                 uint64_t trigger_divider = 1, uint64_t divider_offset = 0, channel_t stop_channel = CHANNEL_UNUSED);

  ~EventGenerator();

  /**
   * \brief the new virtual channel
   *
   * This function returns the new allocated virtual channel.
   * It can be used now in any new iterator.
   *
   */
  channel_t getChannel();

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;
  void on_start() override;

private:
  friend class EventGeneratorImpl;
  std::unique_ptr<EventGeneratorImpl> impl;
};

class TT_API CustomMeasurementBase : public IteratorBase {
protected:
  // Only useable for subclasses.
  CustomMeasurementBase(TimeTaggerBase *tagger);

public:
  ~CustomMeasurementBase() override;

  // Stop all running custom measurements. Use this to avoid races on shutdown the target language.
  static void stop_all_custom_measurements();

  // Forward the public API of the measurement
  void register_channel(channel_t channel);
  void unregister_channel(channel_t channel);
  void finalize_init();
  bool is_running() const;
  void _lock();
  void _unlock();

protected:
  // By default, this calls next_impl_cs
  virtual bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;

  // Handler with easier to wrap API. By default, this does nothing
  virtual void next_impl_cs(void *tags_ptr, uint64_t num_tags, timestamp_t begin_time, timestamp_t end_time);

  // Forward the public handlers for swig to detect this virtual methods. By default, they do nothing
  virtual void clear_impl() override;
  virtual void on_start() override;
  virtual void on_stop() override;
};

class TT_API FlimAbstract : public IteratorBase {
public:
  /**
   * \brief construct a FlimAbstract object, Flim and FlimBase classes inherit from it
   *
   * \param tagger                   reference to a TimeTagger
   * \param start_channel            channel on which start clicks are received for the time differences histogramming
   * \param click_channel            channel on which clicks are received for the time differences histogramming
   * \param pixel_begin_channel      start of a pixel (histogram)
   * \param n_pixels                 number of pixels (histograms) of one frame
   * \param n_bins                   number of histogram bins for each pixel
   * \param binwidth                 bin size in picoseconds
   * \param pixel_end_channel        end marker of a pixel - incoming clicks on the click_channel will be ignored
   *                                 afterwards
   * \param frame_begin_channel      (optional) start the frame, or reset the pixel index
   * \param finish_after_outputframe (optional) sets the number of frames stored within the measurement class. After
   *                                 reaching the number, the measurement will stop. If the number is 0 (default value),
   *                                 one frame is stored and the measurement runs continuously.
   * \param n_frame_average          (optional) average multiple input frames into one output frame, default: 1
   * \param pre_initialize           (optional) initializes the measurement on constructing.
   */
  FlimAbstract(TimeTaggerBase *tagger, channel_t start_channel, channel_t click_channel, channel_t pixel_begin_channel,
               uint32_t n_pixels, uint32_t n_bins, timestamp_t binwidth, channel_t pixel_end_channel = CHANNEL_UNUSED,
               channel_t frame_begin_channel = CHANNEL_UNUSED, uint32_t finish_after_outputframe = 0,
               uint32_t n_frame_average = 1, bool pre_initialize = true);

  ~FlimAbstract();

  /**
   * \brief tells if the data aquisition has finished reaching finish_after_outputframe
   *
   * This function returns a boolean which tells the user if the class is still aquiring data.
   * It can only reach the false state for finish_after_outputframe > 0.
   *
   * \note This can differ from isRunning. The return value of isRunning state depends only on start/startFor/stop.
   */
  bool isAcquiring() const { return acquiring; }

protected:
  template <FastBinning::Mode bin_mode> void process_tags(const std::vector<Tag> &incoming_tags);
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;
  void clear_impl() override;
  void on_start() override;

  virtual void on_frame_end() = 0;

  const channel_t start_channel;
  const channel_t click_channel;
  const channel_t pixel_begin_channel;
  const uint32_t n_pixels;
  const uint32_t n_bins;
  const timestamp_t binwidth;
  const channel_t pixel_end_channel;
  const channel_t frame_begin_channel;
  const uint32_t finish_after_outputframe;
  const uint32_t n_frame_average;

  const timestamp_t time_window;

  timestamp_t current_frame_begin;
  timestamp_t current_frame_end;

  // state
  bool acquiring{};
  bool frame_acquisition{};
  bool pixel_acquisition{};

  uint32_t pixels_processed{};
  uint32_t frames_completed{};
  uint32_t ticks{};
  size_t data_base{};

  std::vector<uint32_t> frame;

  std::vector<timestamp_t> pixel_begins;
  std::vector<timestamp_t> pixel_ends;
  std::deque<timestamp_t> previous_starts;

  FastBinning binner;

  std::recursive_mutex acquisition_lock;
  bool initialized;
};

class TT_API FlimBase : public FlimAbstract {
public:
  /**
   * \brief construct a basic Flim measurement, containing a minimum featureset for efficienty purposes
   *
   * \param tagger                   reference to a TimeTagger
   * \param start_channel            channel on which start clicks are received for the time differences histogramming
   * \param click_channel            channel on which clicks are received for the time differences histogramming
   * \param pixel_begin_channel      start of a pixel (histogram)
   * \param n_pixels                 number of pixels (histograms) of one frame
   * \param n_bins                   number of histogram bins for each pixel
   * \param binwidth                 bin size in picoseconds
   * \param pixel_end_channel        end marker of a pixel - incoming clicks on the click_channel will be ignored
   *                                 afterwards
   * \param frame_begin_channel      (optional) start the frame, or reset the pixel index
   * \param finish_after_outputframe (optional) sets the number of frames stored within the measurement class. After
   *                                 reaching the number, the measurement will stop. If the number is 0 (default value),
   *                                 one frame is stored and the measurement runs continuously.
   * \param n_frame_average          (optional) average multiple input frames into one output frame, default: 1
   * \param pre_initialize           (optional) initializes the measurement on constructing.
   */
  FlimBase(TimeTaggerBase *tagger, channel_t start_channel, channel_t click_channel, channel_t pixel_begin_channel,
           uint32_t n_pixels, uint32_t n_bins, timestamp_t binwidth, channel_t pixel_end_channel = CHANNEL_UNUSED,
           channel_t frame_begin_channel = CHANNEL_UNUSED, uint32_t finish_after_outputframe = 0,
           uint32_t n_frame_average = 1, bool pre_initialize = true);

  ~FlimBase();

  /**
   * \brief initializes and starts measuring this Flim measurement
   *
   * This function initializes the Flim measurement and starts executing it. It does
   * nothing if preinitialized in the constructor is set to true.
   *
   */
  void initialize();

protected:
  void on_frame_end() override;

  virtual void frameReady(uint32_t frame_number, std::vector<uint32_t> &data,
                          std::vector<timestamp_t> &pixel_begin_times, std::vector<timestamp_t> &pixel_end_times,
                          timestamp_t frame_begin_time, timestamp_t frame_end_time);

  uint32_t total_frames;
};

class TT_API FlimFrameInfo {
  friend class Flim;

public:
  /**
   * \brief index of this frame
   *
   * This function returns the frame number, starting from 0 for the very first frame acquired.
   * If the index is -1, it is an invalid frame which is returned on error.
   *
   */
  int32_t getFrameNumber() const { return frame_number; }

  /**
   * \brief tells if this frame is valid
   *
   * This function returns a boolean which tells if this frame is valid or not.
   * Invalid frames are possible on errors, such as asking for the last completed frame when
   * no frame has been completed so far.
   *
   */
  bool isValid() const { return frame_number >= 0; }

  /**
   * \brief number of pixels aquired on this frame
   *
   * This function returns a value which tells how many pixels were processed
   * for this frame.
   *
   */
  uint32_t getPixelPosition() const { return pixel_position; }

  GET_DATA_2D(getHistograms, uint32_t, array_out, );
  GET_DATA_1D(getIntensities, float, array_out, );
  GET_DATA_1D(getSummedCounts, uint64_t, array_out, );
  GET_DATA_1D(getPixelBegins, timestamp_t, array_out, );
  GET_DATA_1D(getPixelEnds, timestamp_t, array_out, );

private:
  std::vector<uint32_t> histograms;
  std::vector<timestamp_t> pixel_begins;
  std::vector<timestamp_t> pixel_ends;

public:
  uint32_t pixels;
  uint32_t bins;
  int32_t frame_number;
  uint32_t pixel_position;
};

/**
 * @ingroup ITERATOR
 *
 * \brief Fluorescence lifetime imaging
 *
 * Successively acquires n histograms (one for each pixel in the image), where
 * each histogram is determined by the number of bins and the binwidth.
 * Clicks that fall outside the histogram range are ignored.
 *
 * Fluorescence-lifetime imaging microscopy or Flim is an imaging technique for producing an image based on
 * the differences in the exponential decay rate of the fluorescence from a fluorescent sample.
 *
 * Fluorescence lifetimes can be determined in the time domain by using a pulsed source. When a population
 * of fluorophores is excited by an ultrashort or delta pulse of light, the time-resolved fluorescence will
 * decay exponentially.
 *
 */
class TT_API Flim : public FlimAbstract {
public:
  /**
   * \brief construct a Flim measurement with a variaty of high-level functionality
   *
   * \param tagger                   reference to a TimeTagger
   * \param start_channel            channel on which start clicks are received for the time differences histogramming
   * \param click_channel            channel on which clicks are received for the time differences histogramming
   * \param pixel_begin_channel      start of a pixel (histogram)
   * \param n_pixels                 number of pixels (histograms) of one frame
   * \param n_bins                   number of histogram bins for each pixel
   * \param binwidth                 bin size in picoseconds
   * \param pixel_end_channel        end marker of a pixel - incoming clicks on the click_channel will be ignored
   *                                 afterwards
   * \param frame_begin_channel      (optional) start the frame, or reset the pixel index
   * \param finish_after_outputframe (optional) sets the number of frames stored within the measurement class. After
   *                                 reaching the number, the measurement will stop. If the number is 0 (default value),
   *                                 one frame is stored and the measurement runs continuously.
   * \param n_frame_average          (optional) average multiple input frames into one output frame, default: 1
   * \param pre_initialize           (optional) initializes the measurement on constructing.
   */
  Flim(TimeTaggerBase *tagger, channel_t start_channel, channel_t click_channel, channel_t pixel_begin_channel,
       uint32_t n_pixels, uint32_t n_bins, timestamp_t binwidth, channel_t pixel_end_channel = CHANNEL_UNUSED,
       channel_t frame_begin_channel = CHANNEL_UNUSED, uint32_t finish_after_outputframe = 0,
       uint32_t n_frame_average = 1, bool pre_initialize = true);

  ~Flim();

  /**
   * \brief initializes and starts measuring this Flim measurement
   *
   * This function initializes the Flim measurement and starts executing it. It does
   * nothing if preinitialized in the constructor is set to true.
   *
   */
  void initialize();

  /**
   * \brief obtain for each pixel the histogram for the given frame index
   *
   * This function returns the histograms for all pixels according to the frame index
   * given. If the index is -1, it will return the last frame, which has been completed.
   * When finish_after_outputframe is 0, the index value must be -1.
   * If index >= finish_after_outputframe, it will throw an error.
   *
   * \param array_out callback for the array output allocation
   * \param index  index of the frame to be obtained. if -1, the last frame which has been completed is returned
   */
  GET_DATA_2D_OP1(getReadyFrame, uint32_t, array_out, int32_t, index, -1, );

  /**
   * \brief obtain an array of the pixel intensity of the given frame index
   *
   * This function returns the intensities according to the frame index
   * given. If the index is -1, it will return the intensity of the last frame, which has been completed.
   * When finish_after_outputframe is 0, the index value must be -1.
   * If index >= finish_after_outputframe, it will throw an error.
   *
   * The pixel intensity is defined by the number of counts acquired within the pixel divided by the respective
   * integration time.
   *
   * \param array_out callback for the array output allocation
   * \param index  index of the frame to be obtained. if -1, the last frame which has been completed is returned
   */
  GET_DATA_1D_OP1(getReadyFrameIntensity, float, array_out, int32_t, index, -1, );

  /**
   * \brief obtain for each pixel the histogram for the frame currently active
   *
   * This function returns the histograms for all pixels of the currently active frame
   */
  GET_DATA_2D(getCurrentFrame, uint32_t, array_out, );

  /**
   * \brief obtain the array of the pixel intensities of the frame currently active
   *
   * This function returns the intensities of all pixels of the currently active frame
   *
   * The pixel intensity is defined by the number of counts acquired within the pixel divided by the respective
   * integration time.
   */
  GET_DATA_1D(getCurrentFrameIntensity, float, array_out, );

  /**
   * \brief obtain for each pixel the histogram from all frames acquired so far
   *
   * This function returns the histograms for all pixels. The counts within the histograms are integrated since the
   * start or the last clear of the measurement.
   *
   * \param array_out callback for the array output allocation
   * \param only_ready_frames  if true, only the finished frames are added. On false, the currently active frame is
   * aggregated.
   * \param clear_summed       if true, the summed frames memory will be cleared.
   */
  GET_DATA_2D_OP2(getSummedFrames, uint32_t, array_out, bool, only_ready_frames, true, bool, clear_summed, false, );

  /**
   * \brief obtain the array of the pixel intensities from all frames acquired so far
   *
   * The pixel intensity is the number of counts within the pixel divided by the integration time.
   *
   * This function returns the intensities of all pixels summed over all acquired frames.
   *
   * \param array_out callback for the array output allocation
   * \param only_ready_frames  if true only the finished frames are added. On false, the currently active frame is
   * aggregated.
   * \param clear_summed       if true, the summed frames memory will be cleared.
   */
  GET_DATA_1D_OP2(getSummedFramesIntensity, float, array_out, bool, only_ready_frames, true, bool, clear_summed,
                  false, );

  /**
   * \brief obtain a frame information object, for the given frame index
   *
   * This function returns a frame information object according to the index
   * given. If the index is -1, it will return the last completed frame. When finish_after_outputframe
   * is 0, index must be -1.
   * If index >= finish_after_outputframe, it will throw an error.
   *
   * \param index  index of the frame to be obtained. if -1, last completed frame will be returned
   */
  FlimFrameInfo getReadyFrameEx(int32_t index = -1);

  /**
   * \brief obtain a frame information object, for the currently active frame
   *
   * This function returns the frame information object for the currently active frame
   */
  FlimFrameInfo getCurrentFrameEx();

  /**
   * \brief obtain a frame information object, that represents the sum of all frames acquired so for.
   *
   * This function returns the frame information object that represents the sum of all acquired frames.
   *
   * \param only_ready_frames  if true only the finished frames are added. On false, the currently active is aggregated.
   * \param clear_summed       if true, the summed frames memory will be reset and all frames stored prior
   * will be unaccounted in the future.
   */
  FlimFrameInfo getSummedFramesEx(bool only_ready_frames = true, bool clear_summed = false);

  /**
   * \brief total number of frames completed so far
   *
   * This function returns the amount of frames that have been completed so far, since the creation / last clear
   * of the object.
   */
  uint32_t getFramesAcquired() const { return total_frames; }

  /**
   * \brief a vector of size n_bins containing the time bins in ps
   *
   * This function returns a vector of size n_bins containing the time bins in ps.
   */
  GET_DATA_1D(getIndex, timestamp_t, array_out, );

protected:
  void on_frame_end() override;
  void clear_impl() override;

  uint32_t get_ready_index(int32_t index);

  virtual void frameReady(uint32_t frame_number, std::vector<uint32_t> &data,
                          std::vector<timestamp_t> &pixel_begin_times, std::vector<timestamp_t> &pixel_end_times,
                          timestamp_t frame_begin_time, timestamp_t frame_end_time);

  std::vector<std::vector<uint32_t>> back_frames;
  std::vector<std::vector<timestamp_t>> frame_begins;
  std::vector<std::vector<timestamp_t>> frame_ends;
  std::vector<uint32_t> pixels_completed;
  std::vector<uint32_t> summed_frames;
  std::vector<timestamp_t> accum_diffs;
  uint32_t captured_frames;
  uint32_t total_frames;
  int32_t last_frame;

  std::mutex swap_chain_lock;
};

class SyntheticSingleTagImpl;
/**
 * @ingroup ITERATOR
 *
 * \brief synthetic trigger timetag generator.
 *
 * Creates timetags based on a trigger method. Whenever the user calls the 'trigger'
 * method, a timetag will be added to the base_channel.
 *
 * This synthetic channel can inject timetags into an existing channel or create a new virtual channel.
 *
 */
class TT_API SyntheticSingleTag : public IteratorBase {
public:
  /**
   * \brief Construct a pulse event generator.
   *
   * \param tagger        reference to a TimeTagger
   * \param base_channel  base channel to which this signal will be added. If unused, a new channel will be
   *                      created.
   */
  SyntheticSingleTag(TimeTaggerBase *tagger, channel_t base_channel = CHANNEL_UNUSED);
  ~SyntheticSingleTag();

  /**
   * \brief Generate a timetag for each call of this method.
   */
  void trigger();

  channel_t getChannel() const;

protected:
  bool next_impl(std::vector<Tag> &incoming_tags, timestamp_t begin_time, timestamp_t end_time) override;

private:
  friend class SyntheticSingleTagImpl;
  std::unique_ptr<SyntheticSingleTagImpl> impl;
};

#endif /* TT_ITERATORS_H_ */
