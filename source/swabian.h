#ifndef SWABIAN_H
#define SWABIAN_H 

#include <timetagger/TimeTagger.h> /* for timestamp_t */
#include <timetagger/Iterators.h> /* For SynchronizedMeasurements */
#include <pthread.h> /* for pthread_mutex_t */

class Swabian
{
public:
    Swabian(void);
    ~Swabian(void);
    std::vector<std::string> check_for_devices(void);
    int connect(std::string serial="");
    int get_histograms(int start_channel, int chan_a, int chan_b, int chan_c, int bin_width, timestamp_t time, std::vector<double> &dataA, std::vector<double> &dataB, std::vector<double> &dataC);
    int set_delay(int channel, int delay);
    int set_trigger_level(int channel, float level);
    int get_count_rates(int *channels, double *out, int n);
    int set_test_signal(int channel, int value);
    void set_rising_mask(int _rising_channel_mask);
    int disconnect(void);

    TimeTagger *t;
    int rising_channel_mask;

    pthread_mutex_t m;
};

#endif
