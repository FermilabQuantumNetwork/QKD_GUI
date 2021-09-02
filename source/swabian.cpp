#include "swabian.h"
#include <timetagger/Iterators.h>
#include <timetagger/TimeTagger.h>
#include <stdio.h>
#include <thread>
#include <pthread.h> /* for pthread_mutex_t */

Swabian::Swabian(void)
{
    t = NULL;
    rising_channel_mask = 0;
    pthread_mutex_init(&this->m,NULL);
}

Swabian::~Swabian(void)
{
    if (this->t)
        freeTimeTagger(this->t);
}

/* Returns a vector of strings for all connected Swabian devices. */
std::vector<std::string> Swabian::check_for_devices(void)
{
    return scanTimeTagger();
}

/* Connect to a Swabian time tagger.
 *
 * Returns 0 on success, -1 on error. */
int Swabian::connect(std::string serial)
{
    std::vector<std::string> taggers = scanTimeTagger();

    if (taggers.empty()) {
        fprintf(stderr, "No time tagger found. Please attach a Time Tagger.\n");
        return -1;
    }

    // connect to a time tagger
    this->t = createTimeTagger(serial);

    if (!this->t) return -1;

    return 0;
}

/* Initialize the measurements for a histogram of time differences between multiple channels and a start channel. Arguments:
 *
 *     - start_channel: The clock signal
 *     - channels: A bitmask of the channels to calculate time differences from
 *     - n: the number of channels
 *     - bin_width: The bin width of the histogram in ps
 *     - time: The time of the acquisition in seconds
 *
 * Returns 0 on success, -1 on error. */
int Swabian::get_histograms(int start_channel, int chan_a, int chan_b, int chan_c, int bin_width, timestamp_t time, std::vector<double> &dataA, std::vector<double> &dataB, std::vector<double> &dataC)
{
    int i, j;

    if (!this->t) {
        fprintf(stderr, "error: initialize_histogram() called but no time tagger connected!\n");
        return -1;
    }

    //pthread_mutex_lock(&this->m);

    SynchronizedMeasurements measurementGroup(this->t);

    std::vector<StartStop*> measurements;
    measurements.push_back(new StartStop(measurementGroup.getTagger(), chan_a, start_channel, bin_width));
    measurements.push_back(new StartStop(measurementGroup.getTagger(), chan_b, start_channel, bin_width));
    measurements.push_back(new StartStop(measurementGroup.getTagger(), chan_c, start_channel, bin_width));

    // This will run these measurements simultaneously.
    // Because of the asynchronous processing, they will neither start nor stop at once in real time, but they will
    // process exact the same data.
    measurementGroup.startFor(time);
    measurementGroup.waitUntilFinished();

    for (i = 0; i < (int) measurements.size(); i++) {
        std::vector<timestamp_t> data;

        // Fetch both vectors of data.
        measurements[i]->getData([&data](size_t size1, size_t size2) {
          data.resize(size1 * size2);
          return data.data();
        });

        switch (i) {
        case 0:
            for (j = 0; j < (int) data.size(); j++)
                dataA.push_back((double) data[j]);
            break;
        case 1:
            for (j = 0; j < (int) data.size(); j++)
                dataB.push_back((double) data[j]);
            break;
        case 2:
            for (j = 0; j < (int) data.size(); j++)
                dataC.push_back((double) data[j]);
            break;
        }

        delete(measurements[i]);
    }


    //pthread_mutex_unlock(&this->m);
    
    return 0;
}

/* Set the delay on channel `channel` to `delay` in ps.
 *
 * The maximum delay is +/- 2000000 (or +/- 2 us).
 *
 * Returns 0 on success and -1 on error. */
int Swabian::set_delay(int channel, int delay)
{
    if (!this->t) {
        fprintf(stderr, "error: set_delay() called but no time tagger connected!\n");
        return -1;
    }

    if (abs(delay) > 2000000) {
        fprintf(stderr, "delay must be between +/- 2000000 ps\n");
        return -1;
    }

    pthread_mutex_lock(&this->m);

    this->t->setDelayHardware(channel,delay);

    pthread_mutex_unlock(&this->m);
    
    return 0;
}

/* Set the trigger level on channel `channel` to `level` in V.
 *
 * Returns 0 on success and -1 on error. */
int Swabian::set_trigger_level(int channel, float level)
{
    if (!this->t) {
        fprintf(stderr, "error: set_trigger_level() called but no time tagger connected!\n");
        return -1;
    }

    pthread_mutex_lock(&this->m);

    this->t->setTriggerLevel(channel,level);

    pthread_mutex_unlock(&this->m);
    
    return 0;
}

/* Get the count rate on channels `channels`. The number of channels is
 * specified by `n`, and `out` should be an array with at least `n` elements.
 *
 * Returns 0 on success and -1 on error. */
int Swabian::get_count_rates(int *channels, double *out, int n)
{
    int i;
    int channels_corrected[18];

    if (!this->t) {
        fprintf(stderr, "error: get_count_rates() called but no time tagger connected!\n");
        return -1;
    }

    /* The Swabian actually triggers on *both* rising and falling edges at all
     * times. In order to get the times from a rising edge you need to specify
     * the channel number as normal, but to get the falling edges, you need to
     * specify a negative channel number. */
    for (i = 0; i < n; i++) {
        if ((this->rising_channel_mask & (1 << i)) == 0) {
            channels_corrected[i] = -channels[i];
        } else {
            channels_corrected[i] = channels[i];
        }
    }

    //pthread_mutex_lock(&this->m);

    std::vector<channel_t> v(channels_corrected,channels_corrected+n);
    Countrate c(t, v);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::vector<double> data;
    c.getData([&data](size_t size) {
      data.resize(size);
      return data.data();
    });

    for (i = 0; i < n; i++)
        out[i] = data[i];

    //pthread_mutex_unlock(&this->m);
    
    return 0;
}

void Swabian::set_rising_mask(int _rising_channel_mask)
{
    this->rising_channel_mask = _rising_channel_mask;
}

/* Enable/Disable the test signal on channel `channel`.
 *
 * Returns 0 on success, -1 on error. */
int Swabian::set_test_signal(int channel, int value)
{
    if (!this->t) {
        fprintf(stderr, "error: set_test_signal() called but no time tagger connected!\n");
        return -1;
    }

    pthread_mutex_lock(&this->m);

    if (value)
        this->t->setTestSignal(channel,true);
    else
        this->t->setTestSignal(channel,false);

    pthread_mutex_unlock(&this->m);
    
    return 0;
}
