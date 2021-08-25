#ifndef SWABIAN_H
#define SWABIAN_H 

#include <QThread>
#include <timetagger/TimeTagger.h> /* for timestamp_t */
#include <timetagger/Iterators.h> /* For SynchronizedMeasurements */

class Swabian : public QThread
{
    Q_OBJECT

public:
    Swabian(void);
    ~Swabian(void);
    int connect();
    int initialize_measurements(int start_channel, int click_channel_mask, int bin_width, timestamp_t time);
    int get_histogram(int channel, std::vector<timestamp_t> *data);

    TimeTagger *t;
    SynchronizedMeasurements *measurementGroup;
    std::vector<StartStop*> measurements;
    std::vector<int> channels;
};

#endif
