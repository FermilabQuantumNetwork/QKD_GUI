#include "swabian.h"
#include <timetagger/Iterators.h>
#include <timetagger/TimeTagger.h>
#include <stdio.h>

Swabian::Swabian(void)
{
}

Swabian::~Swabian(void)
{
    int i;

    if (measurementGroup != NULL)
        delete measurementGroup;

    for (i = 0; i < measurements.size(); i++)
        delete measurements[i];

    measurements.clear();
    channels.clear();
}

/* Connect to a Swabian time tagger.
 *
 * Returns 0 on success, -1 on error. */
int Swabian::connect()
{
    std::vector<std::string> taggers = scanTimeTagger();

    if (taggers.empty()) {
        fprintf(stderr, "No time tagger found. Please attach a Time Tagger.\n");
        return -1;
    }

    // connect to a time tagger
    t = createTimeTagger();

    return 0;
}

/* Initialize the measurements for a histogram of time differences between multiple channels and a start channel. Arguments:
 *
 *     - start_channel: The clock signal
 *     - click_channel_mask: A bitmask of the channels to calculate time differences from
 *     - bin_width: The bin width of the histogram in ps
 *     - time: The time of the acquisition
 *
 * Returns 0 on success, -1 on error. */
int Swabian::initialize_measurements(int start_channel, int click_channel_mask, int bin_width, timestamp_t time)
{
    int i;

    if (!t) {
        fprintf(stderr, "error: initialize_histogram() called but no time tagger connected!\n");
        return -1;
    }

    if (measurementGroup != NULL)
        delete measurementGroup;

    measurementGroup = new SynchronizedMeasurements(this->t);

    for (i = 0; i < measurements.size(); i++)
        delete measurements[i];

    measurements.clear();
    channels.clear();
    for (i = 0; i < 5; i++) {
        if (!(click_channel_mask & (1 << i)))
            continue;
        measurements.push_back(new StartStop(measurementGroup->getTagger(), i, start_channel, bin_width));
        channels.push_back(i);
    }

    // This will run these measurements simultaneously.
    // Because of the asynchronous processing, they will neither start nor stop at once in real time, but they will
    // process exact the same data.
    measurementGroup->startFor(time);
    measurementGroup->waitUntilFinished();

    return 0;
}

/* Sets the histogram of the time differences between channel `channel` and the
 * start channel specified when calling initialize_measurements(). Returns 0 on
 * success, -1 on error. */
int Swabian::get_histogram(int channel, std::vector<timestamp_t> *data)
{
    int i;

    if (!t) {
        fprintf(stderr, "error: get_histogram() called but no time tagger connected!\n");
        return -1;
    }

    for (i = 0; i < measurements.size(); i++) {
        if (channels[i] == channel) {
            // Fetch both vectors of data.
            measurements[i]->getData([data](size_t size1, size_t size2) {
              data->resize(size1 * size2);
              return data->data();
            });
            return 0;
        }
    }
    
    fprintf(stderr, "no histogram found for channel %i\n", channel);

    return -1;
}

int Swabian::set_delay(int channel, int delay)
{
    if (!t) {
        fprintf(stderr, "error: set_delay() called but no time tagger connected!\n");
        return -1;
    }

    if (abs(delay) > 2000000) {
        fprintf(stderr, "delay must be between +/- 2000000 ps\n");
        return -1;
    }

    t->setDelayHardware(channel,delay);

    return 0;
}

int Swabian::set_trigger_level(int channel, float level)
{
    if (!t) {
        fprintf(stderr, "error: set_delay() called but no time tagger connected!\n");
        return -1;
    }

    t->setTriggerLevel(channel,level);

    return 0;
}

/* Slots. */

int Swabian::set_delay_ch1(int delay)
{
    return set_delay(1,delay);
}

int Swabian::set_delay_ch2(int delay)
{
    return set_delay(2,delay);
}

int Swabian::set_delay_ch3(int delay)
{
    return set_delay(3,delay);
}

int Swabian::set_delay_ch4(int delay)
{
    return set_delay(4,delay);
}

int Swabian::set_trigger_level_ch1(float level)
{
    return set_trigger_level(1,level);
}

int Swabian::set_trigger_level_ch2(float level)
{
    return set_trigger_level(2,level);
}

int Swabian::set_trigger_level_ch3(float level)
{
    return set_trigger_level(3,level);
}

int Swabian::set_trigger_level_ch4(float level)
{
    return set_trigger_level(4,level);
}

