#ifndef SWABIAN_H
#define SWABIAN_H 


class swabian : public QThread
{
    Q_OBJECT

public:
    void run();

    explicit swabian();
    ~swabian();
    void loadtdcfiltertype();
    bool anlAvilable =false;

    int connect();
    int initialize_measurements(int start_channel, int click_channel_mask, int bin_width);
    int get_histogram(int channel, std::vector<timestamp_t> *data);

public slots:

  void adqui();
  void setHistograms();

  void adqpausechange(bool chang){adqpause_=chang;}
  void Break(){break_= true;}

  void updatefiltertype(int ch);
  void setfilter(int ch);
  void Chang_in_binsinplot(int val){this->in_binsinplot=val;paramschange=true;}

  void Chang_in_histStart(int val){this->in_histStart=val;paramschange=true;}
  void Chang_in_binWidth(int val){this->in_binWidth=val;paramschange=true;}
  void Chang_in_startChan(int val){this->in_startChan=val;paramschange=true;}

  void Chang_in_adqtime(double val){this->in_adqtime=val;}

  void Chang_in_PlotAChn1(int val){this->in_PlotACh1=val;paramschange=true;std::cout<<"A1: "<<val<<std::endl;}
  void Chang_in_PlotAChn2(int val){this->in_PlotACh2=val;paramschange=true;}
  void Chang_in_PlotBChn1(int val){this->in_PlotBCh1=val;paramschange=true;std::cout<<"B1: "<<val<<std::endl;}
  void Chang_in_PlotBChn2(int val){this->in_PlotBCh2=val;paramschange=true;std::cout<<"B2: "<<val<<std::endl;}
  void Chang_in_PlotCChn1(int val){this->in_PlotCCh1=val;paramschange=true;std::cout<<"C1: "<<val<<std::endl;}
  void Chang_in_PlotCChn2(int val){this->in_PlotCCh2=val;paramschange=true;std::cout<<"C2: "<<val<<std::endl;}

  void Chang_anlAvilable(bool val){this->anlAvilable =val;}

  void Chang_in_thch1(double val){thresholds[1]=val;changThreshold(1);}
  void Chang_in_thch2(double val){thresholds[2]=val;changThreshold(2);}
  void Chang_in_thch3(double val){thresholds[3]=val;changThreshold(3);}
  void Chang_in_thch4(double val){thresholds[4]=val;changThreshold(4);}
  void Chang_in_cw(int val){this->in_cw=val; TDC_setCoincidenceWindow(val);}

  void Chang_rof1(QString text){if(text=="Rise")RoF[1]=1;else RoF[1]=0;changThreshold(1);}
  void Chang_rof2(QString text){if(text=="Rise")RoF[2]=1;else RoF[2]=0;changThreshold(2);}
  void Chang_rof3(QString text){if(text=="Rise")RoF[3]=1;else RoF[3]=0;changThreshold(3);}
  void Chang_rof4(QString text){if(text=="Rise")RoF[4]=1;else RoF[4]=0;changThreshold(4);}

  void Chang_delay1(double val){delays[1]=int(1000*val);set_delays();}
  void Chang_delay2(double val){delays[2]=int(1000*val);set_delays();}
  void Chang_delay3(double val){delays[3]=int(1000*val);set_delays();}
  void Chang_delay4(double val){delays[4]=int(1000*val);set_delays();}
  void set_delays();

  void Chang_filtertype1(QString text){filtertypeSTR[1]=text; updatefiltertype(1);}
  void Chang_filtertype2(QString text){filtertypeSTR[2]=text; updatefiltertype(2);}
  void Chang_filtertype3(QString text){filtertypeSTR[3]=text; updatefiltertype(3);}
  void Chang_filtertype4(QString text){filtertypeSTR[4]=text; updatefiltertype(4);}

  void Chang_filtermask1_1(int state){if(state)ch_filtermask[1]|=0x1<<1;else ch_filtermask[1]&=!(0x1<<1);setfilter(1); }
  void Chang_filtermask1_2(int state){if(state)ch_filtermask[1]|=0x1<<2;else ch_filtermask[1]&=!(0x1<<2);setfilter(1); }
  void Chang_filtermask1_3(int state){if(state)ch_filtermask[1]|=0x1<<3;else ch_filtermask[1]&=!(0x1<<3);setfilter(1); }
  void Chang_filtermask1_4(int state){if(state)ch_filtermask[1]|=0x1<<4;else ch_filtermask[1]&=!(0x1<<4);setfilter(1); }

  void Chang_filtermask2_1(int state){if(state)ch_filtermask[2]|=0x1<<1;else ch_filtermask[2]&=!(0x1<<1);setfilter(2); }
  void Chang_filtermask2_2(int state){if(state)ch_filtermask[2]|=0x1<<2;else ch_filtermask[2]&=!(0x1<<2);setfilter(2); }
  void Chang_filtermask2_3(int state){if(state)ch_filtermask[2]|=0x1<<3;else ch_filtermask[2]&=!(0x1<<3);setfilter(2); }
  void Chang_filtermask2_4(int state){if(state)ch_filtermask[2]|=0x1<<4;else ch_filtermask[2]&=!(0x1<<4);setfilter(2); }

  void Chang_filtermask3_1(int state){if(state)ch_filtermask[3]|=0x1<<1;else ch_filtermask[3]&=!(0x1<<1);setfilter(3); }
  void Chang_filtermask3_2(int state){if(state)ch_filtermask[3]|=0x1<<2;else ch_filtermask[3]&=!(0x1<<2);setfilter(3); }
  void Chang_filtermask3_3(int state){if(state)ch_filtermask[3]|=0x1<<3;else ch_filtermask[3]&=!(0x1<<3);setfilter(3); }
  void Chang_filtermask3_4(int state){if(state)ch_filtermask[3]|=0x1<<4;else ch_filtermask[3]&=!(0x1<<4);setfilter(3); }

  void Chang_filtermask4_1(int state){if(state)ch_filtermask[3]|=0x1<<1;else ch_filtermask[4]&=!(0x1<<1);setfilter(4); }
  void Chang_filtermask4_2(int state){if(state)ch_filtermask[3]|=0x1<<2;else ch_filtermask[4]&=!(0x1<<2);setfilter(4); }
  void Chang_filtermask4_3(int state){if(state)ch_filtermask[3]|=0x1<<3;else ch_filtermask[4]&=!(0x1<<3);setfilter(4); }
  void Chang_filtermask4_4(int state){if(state)ch_filtermask[3]|=0x1<<4;else ch_filtermask[4]&=!(0x1<<4);setfilter(4); }

signals:
    void dataready(const vectorInt64 &vectorTimetags, const vectorInt8 &vectorChannels, int tsvalid);
    void qutaghist(const vectorDouble &TTdata1, const vectorDouble &TTdata2, const vectorDouble &TTdata3);
    void TDCerror(QString text);

private:
    QVector<double> dataA;
    QVector<double> dataB;
    QVector<double> dataC;
    bool paramschange = false;
    bool adqpause_;
    bool break_;
    void checkRc( const char * fctname, int rc );
    void getHisto();
    float rate(int ch_rate);
    int get_max_collection_time( float rate );
    int count1, count2, count3;
    int firstChanHist, secondChanHist;//thirdChanHist;
    int ActHist[5][5]={{0}};
    //QVector<int> ch1[4];QVector<int> ch2[4];QVector<int> ch3[4];QVector<int> ch4[4];

    QVector<int64_t> timetags;
    QVector<int8_t> channelsTDC;
    QVector<int32_t> histo1vector, histo2vector;

    int debug = DEBUG;
    bool _stop;
    int ret;
    Int32 rc, count, tooSmall, tooBig, tsValid, eventsA, eventsB, eventsC, i, j, it, ch;
    Int64 expTime, lastTimestamp = 0;    
    Int8  channels[TIMESTAMP_COUNT];
    int   coincCnt[TDC_COINC_CHANNELS];
    double bin2ns = 0, timeBase = 0.;
    double simPara[2] = { 1000., 1000. };
    float TOTAL_RATE;
    int adqclock;
    int notfirstrun = 0;
    int sleeptime = 10;
    int debug3 = DEBUG3;
    int a = 0;

    int microsec = 10000; // length of time to sleep, in microseconds

    void lautrun();
    void getTimeStamps();
    void changThreshold(int);
    int HIST_BINWIDTH;
    int HIST_BINCOUNT;
    int HIST_BINWIDTH_out, HIST_BINCOUNT_out;
    Int32 hist1[5000], hist2[5000];

    Int32 *histodataA;
    Int32 *histodataB;
    Int32 *histodataC;
    Int64 timestamps[TIMESTAMP_COUNT];

    int delays[5] = {0,0,0,0,0};
    /////////////////tab 1 variables///////////////
    int in_binsinplot, in_startChan, in_histStart, in_binWidth;
    double in_adqtime;
    int in_PlotACh1, in_PlotACh2, in_PlotBCh1, in_PlotBCh2, in_PlotCCh1, in_PlotCCh2;

public:
    clock_t begin, end;
    double cpu_time_used;
    bool initdone = 0;
    double thresholds[5];
    int in_cw;
    bool RoF[5];
    int ch_filtermask[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    TDC_FilterType filtertype[5];
    QString filtertypeSTR[5] = {"NONE", "NONE", "NONE", "NONE", "NONE"};

    TimeTagger *t;
    SynchronizedMeasurements *measurementGroup;
    vector<StartStop> measurements;
    vector<int> channels;
};

#endif
