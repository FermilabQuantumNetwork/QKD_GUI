#include "swabian.h"
#include <timetagger/Iterators.h>
#include <timetagger/TimeTagger.h>

swabian::swabian()
{
}

/* Connect to a Swabian time tagger.
 *
 * Returns 0 on success, -1 on error. */
int swabian::connect()
{
    std::vector<std::string> taggers = scanTimeTagger();

    if (taggers.empty()) {
        fprintf(stderr, "No time tagger found. Please attach a Time Tagger.\n";
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
 *
 * Returns 0 on success, -1 on error. */
int swabian::initialize_measurements(int start_channel, int click_channel_mask, int bin_width)
{
    if (!t) {
        fprintf(stderr, "error: get_histogram() called but no time tagger connected!\n");
        return -1;
    }

    if (measurementGroup != NULL)
        delete measurementGroup

    measurementGroup = SynchronizedMeasurements(self->t);

    measurements.clear()
    for (i = 0; i < 5; i++) {
        if (!(click_channel_mask & (1 << i)))
            continue;
        measurements.append(StartStop(measurementGroup->getTagger(), click_channel, start_channel, bin_width));
        channels.append(i);
    }

    // This will run these measurements simultaneously.
    // Because of the asynchronous processing, they will neither start nor stop at once in real time, but they will
    // process exact the same data.
    measurementGroup.startFor(time);
    measurementGroup.waitUntilFinished();

    return 0;
}

/* Sets the histogram of the time differences between channel `channel` and the
 * start channel specified when calling initialize_measurements(). Returns 0 on
 * success, -1 on error. */
int swabian::get_histogram(int channel, std::vector<timestamp_t> *data)
{
    if (!t) {
        fprintf(stderr, "error: get_histogram() called but no time tagger connected!\n");
        return -1;
    }

    for (i = 0; i < measurements.size(); i++) {
        if (channels[i] == channel)
            // Fetch both vectors of data.
            measurements[i].getData([&data](size_t size1, size_t size2) {
              data.resize(size1 * size2);
              return data.data();
            });
            return 0;
        }
    }
    
    fprintf(stderr, "no histogram found for channel %i\n", channel);

    return -1;
}
