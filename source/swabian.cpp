#include "swabian.h"
#include <timetagger/Iterators.h>
#include <timetagger/TimeTagger.h>
#include <stdio.h>
#include <thread>
#include <pthread.h> /* for pthread_mutex_t */

Swabian::Swabian(void)
{
    t = NULL;
    measurementGroup = NULL;
    rising_channel_mask = 0;
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

    if (this->t)
        freeTimeTagger(this->t);
}

std::vector<std::string> Swabian::check_for_devices(void)
{
    pthread_mutex_lock(&this->m);
    return scanTimeTagger();
    pthread_mutex_unlock(&this->m);
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
 *     - click_channel_mask: A bitmask of the channels to calculate time differences from
 *     - bin_width: The bin width of the histogram in ps
 *     - time: The time of the acquisition
 *
 * Returns 0 on success, -1 on error. */
int Swabian::initialize_measurements(int start_channel, int click_channel_mask, int bin_width, timestamp_t time)
{
    int i;

    if (!this->t) {
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

    pthread_mutex_lock(&this->m);

    for (i = 0; i < measurements.size(); i++) {
        if (channels[i] == channel) {
            // Fetch both vectors of data.
            measurements[i]->getData([data](size_t size1, size_t size2) {
              data->resize(size1 * size2);
              return data->data();
            });
            pthread_mutex_unlock(&this->m);

            return 0;
        }
    }

    pthread_mutex_unlock(&this->m);
    
    fprintf(stderr, "no histogram found for channel %i\n", channel);

    return -1;
}

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

int Swabian::get_count_rates(int *channels, double *out, size_t n)
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
        if (this->rising_channel_mask & (1 << i) == 0) {
            channels_corrected[i] = -channels[i];
        } else {
            channels_corrected[i] = channels[i];
        }
    }

    pthread_mutex_lock(&this->m);

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

    pthread_mutex_unlock(&this->m);
    
    return 0;
}

void Swabian::set_rising_mask(int _rising_channel_mask)
{
    this->rising_channel_mask = _rising_channel_mask;
}

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
