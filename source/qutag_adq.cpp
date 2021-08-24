#include <qutag_adq.h>
#include <iostream>//entradas y salidas por consola
#include <fstream>//archivos.txt
#include <time.h>
#include <unistd.h> /* For exit() */

#define SLEEP(x) usleep(x)

qutagadq::qutagadq()
{
    timetags.reserve(TIMESTAMP_COUNT+1);
    channelsTDC.reserve(TIMESTAMP_COUNT+1);
    break_=false;
    adqpause_=false;
    histodataA=NULL;
    histodataB=NULL;
    histodataC=NULL;

    anlAvilable=false;

    printf( ">>> tdcbase version: %f\n", TDC_getVersion() );

    /* Accept every device */
    rc = TDC_init(-1);

    if (rc) {
        fprintf(stderr, "Error connecting to qutag: %s\n", TDC_perror(rc));
        exit(1);
    }

    rc = TDC_getTimebase( &timeBase );
    checkRc( "TDC_getTimebase", rc );
    printf( ">>> timebase: %g ps\n", timeBase * 1.e12 );
    fflush(stdout);


    rc = TDC_enableChannels( 0xff );                     /* Use all channels */
    checkRc( "TDC_enableChannels", rc );

    rc = TDC_setTimestampBufferSize( TIMESTAMP_COUNT );
    checkRc( "TDC_setTimestampBufferSize", rc );

    rc = TDC_setExposureTime( 1000 );
    checkRc( "TDC_setExposureTime", rc );

    /*RoF[1]=1;
    RoF[2]=0;
    RoF[3]=0;
    RoF[4]=0;*/



    //rc = TDC_enableMarkers(0);        //WHY IS THIS NOT WORKING??
    //checkRc( "TDC_enableMarkers", rc);

    //rc = TDC_setExposureTime( EXP_TIME );
    //checkRc( "TDC_setExposureTime", rc );

    rc = TDC_setCoincidenceWindow( 90000 ); /* 30ns */
    checkRc( "TDC_setCoincidenceWindow", rc );


    SLEEP( 1e3 );
    TDC_getCoincCounters( coincCnt, NULL );
    SLEEP( 1e3 );
    TDC_getCoincCounters( coincCnt, NULL );
    SLEEP(1e6);
    TDC_getCoincCounters( coincCnt, NULL );



    //req.tv_sec = 0;
    //req.tv_nsec = microsec * 1000L;

    fflush(stdout);



    rc = TDC_enableStartStop( 1 );
    checkRc( "TDC_enableStartStop", rc );
}

void qutagadq::run()
{
    lautrun();
}

/* Check return code and exit on error */
void qutagadq::checkRc(const char *fctname, int rc)
{
    if (rc) {
        fprintf(stderr, "error calling %s: %s\n", fctname, TDC_perror(rc));

        /* Don't quit on "not connected" to allow for demo mode */
        if (rc != TDC_NotConnected) {
            printf("error\n");
            TDC_deInit();
            exit(1);
        }
    }
}


void qutagadq::adqui()
{

}

qutagadq::~qutagadq()
{
    TDC_deInit();
}

int qutagadq::filterset(){

    int maskA=0x00 , maskB=0x00, maskC=0x00;

    maskA|=0x1<<(in_PlotACh2);
    maskA|=0x1<<(in_startChan);

    maskB|=0x1<<(in_PlotBCh1);
    maskB|=0x1<<(in_startChan);

    maskC|=0x1<<(in_PlotACh2);
    maskC|=0x1<<(in_PlotBCh2);
    maskC|=0x1<<(in_PlotCCh2);
    //maskC|=0x1<<(in_startChan);

   /* std::cout<<"filter channel : "<<in_startChan<<"  , mask : "<<maskC<<std::endl;
    std::cout<<"filter channel : "<<in_PlotACh2<<"  , mask : "<<maskA<<std::endl;
    std::cout<<"filter channel : "<<in_PlotBCh2<<"  , mask : "<<maskB<<std::endl;*/
    rc = TDC_configureFilter(in_startChan, FILTER_SYNC , maskC);
    checkRc( "TDC_configureFilter clock", rc );
    rc = TDC_configureFilter(in_PlotACh2, FILTER_NONE, maskA);
    checkRc( "TDC_configureFilter mask A", rc );
    rc = TDC_configureFilter(in_PlotBCh2, FILTER_NONE, maskB);
    checkRc( "TDC_configureFilter mask B", rc );
    rc = TDC_configureFilter(in_PlotCCh2, FILTER_NONE, maskB);
    checkRc( "TDC_configureFilter mask B", rc );

    return 0;
}


 void qutagadq::lautrun(){
     setHistograms();

  TDC_clearAllHistograms ();

  //filterset();
  //Int32 coincWin = in_histEnd-in_histStart;
  //rc =TDC_setCoincidenceWindow(coincWin);
  //checkRc( "TDC_enableStartStop", rc );
    double previous_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
    double current_time;

     while(!break_){
         current_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
        // std::cout<< current_time-previous_time <<std::endl;
         if((current_time-previous_time) > 1000*in_adqtime){           
             getHisto();//TDC_clearAllHistograms ();
             previous_time = current_time;
         }
         if(anlAvilable)getTimeStamps();
         QThread::msleep(10);

     }

 }

 void qutagadq::getHisto(){

    //std::cout<<"gethisto"<<std::endl;
    if(paramschange)setHistograms();
    if(rc)return;

     if (histodataA != 0) {
         delete [] histodataA;
     }

     histodataA = new Int32 [in_binsinplot];

     if (histodataB != 0) {
         delete [] histodataB;
     }

     histodataB = new Int32 [in_binsinplot];

     if (histodataC != 0) {
         delete [] histodataC;
     }

     histodataC = new Int32 [in_binsinplot];

    /* std::cout<<"binsinplot adq  :  "<< in_binsinplot<<std::endl;
     std::cout<<"plot 1A  :  "<< in_PlotACh1<<std::endl;
     std::cout<<"plot 2A  :  "<< in_PlotACh2<<std::endl;
*/

     // Retreive and print selected start stop histogram
     if(in_PlotACh1==in_PlotBCh1 && in_PlotACh2==in_PlotBCh2)
       rc = TDC_getHistogram(in_PlotACh1, in_PlotACh2, 0, histodataA, &count, &tooSmall, &tooBig, &eventsA, &eventsB, &expTime );
     else rc = TDC_getHistogram(in_PlotACh1, in_PlotACh2, 1, histodataA, &count, &tooSmall, &tooBig, &eventsA, &eventsB, &expTime );

       checkRc( "TDC_getHistogram A", rc );


        count1=count;

       QVector<double> dataA(in_binsinplot);
       for (int i=0; i<in_binsinplot; ++i){

           dataA[i]=double(histodataA[i]);
       }


      /* std::cout<<"in_binsinplot  :  "<< in_binsinplot<<std::endl;
       std::cout<<"plot 1B  :  "<< in_PlotBCh1<<std::endl;
        std::cout<<"plot 2B  :  "<< in_PlotBCh2<<std::endl;*/


       rc = TDC_getHistogram(in_PlotBCh1, in_PlotBCh2, 1, histodataB, &count, &tooSmall, &tooBig, &eventsA, &eventsB, &expTime );
       checkRc( "TDC_getHistogram B", rc );


       count2=count;

       QVector<double> dataB(in_binsinplot);

       for (int i=0; i<in_binsinplot; ++i){

           dataB[i]=double( histodataB[i] );
       }



       rc = TDC_getHistogram(in_PlotCCh1, in_PlotCCh2, 1, histodataC, &count, &tooSmall, &tooBig, &eventsA, &eventsB, &expTime );
       checkRc( "TDC_getHistogram C", rc );


       count3=count;

       QVector<double> dataC(in_binsinplot);

       for (int i=0; i<in_binsinplot; ++i){

           dataC[i]=double (histodataC[i]);
       }

//std::cout<<count1<<"  "<<count2<<"  "<< count3<<std::endl;
       if(count1 != 0 || count2 !=0 || count3 !=0)emit(qutaghist(dataA, dataB, dataC));

       dataA.clear();
       dataB.clear();
       dataC.clear();
 }


 void qutagadq::getTimeStamps(){
     //std::cout<<"gethisto"<<std::endl;
     timetags.clear();channelsTDC.clear();

     rc = TDC_getLastTimestamps( 1, timestamps, channels, &tsValid );
     checkRc( "TDC_getLastTimestamps", rc );
     std::copy(timestamps, timestamps + tsValid, std::back_inserter(timetags));
     std::copy(channels, channels + tsValid, std::back_inserter(channelsTDC));     
     if(tsValid>0 && anlAvilable){
        // QThread::msleep(1);



         /*for ( int i=0; i < 20; i++ ) {
         //   printf("channel original:  %hd",channels[i]);
           std::cout<<"     channel :"<<(int)channels[i]<<"\t TTS: "<<timestamps[i]<<"       "<<timestamps[i+1]-timestamps[i]<<std::endl;
         }*/
        emit dataready(timetags, channelsTDC, (int)tsValid);
     }
//std::cout<<"gethisto5"<<std::endl;
 }

 void qutagadq::setHistograms(){


     /////////calculate histogram parameters///////////

     if(in_binsinplot>0)HIST_BINWIDTH=(int)(in_histEnd-in_histStart)/in_binsinplot;
     HIST_BINCOUNT=in_binsinplot;
     filterset();

    /* std::cout<<"HIST_BINWIDTH  :  "<< HIST_BINWIDTH<<std::endl;
     std::cout<<"HIST_BINCOUNT  :  "<< HIST_BINCOUNT<<std::endl;
     std::cout<<"HIST_BINWIDTH  *  HIST_BINCOUNT  :  "<< HIST_BINWIDTH*HIST_BINCOUNT<<std::endl;*/
     bin2ns = HIST_BINWIDTH * timeBase * 1.e9;

     /////////////create the histograms on the FPGA only if is necesary/////////////

     if(ActHist[in_PlotACh1][in_PlotACh2]==0){
         firstChanHist=in_PlotACh1;
         secondChanHist=in_PlotACh2;
         //thirdChanHist=in_PlotACh2;
         ActHist[in_PlotACh1][in_PlotACh2]=1;
        // std::cout<<"quewepasahermano  "<<std::endl;
         rc = TDC_addHistogram( firstChanHist, secondChanHist, 1 );
         checkRc( "TDC_addHistogram", rc );
         //std::cout<<"quewepasahermano 2 "<<std::endl;
     }


     if(in_PlotACh1!=in_PlotBCh1 || in_PlotACh2!=in_PlotBCh2){
         if(ActHist[in_PlotBCh1][in_PlotBCh2]==0){
             firstChanHist=in_PlotBCh1;
             secondChanHist=in_PlotBCh2;
             ActHist[in_PlotBCh1][in_PlotBCh2]=1;
             rc = TDC_addHistogram( firstChanHist, secondChanHist, 1 );
             checkRc( "TDC_addHistogram", rc );
         }
     }
     if(in_PlotBCh1!=in_PlotCCh1 || in_PlotBCh2!=in_PlotCCh2){
         if(ActHist[in_PlotCCh1][in_PlotCCh2]==0){
             firstChanHist=in_PlotCCh1;
             secondChanHist=in_PlotCCh2;
             ActHist[in_PlotCCh1][in_PlotCCh2]=1;
             rc = TDC_addHistogram( firstChanHist, secondChanHist, 1 );
             checkRc( "TDC_addHistogram", rc );
         }
     }
    if((HIST_BINWIDTH!= HIST_BINWIDTH_out || HIST_BINCOUNT!=HIST_BINCOUNT_out) && HIST_BINWIDTH>=1){
      HIST_BINWIDTH_out= HIST_BINWIDTH;
      HIST_BINCOUNT_out = HIST_BINCOUNT;
      //std::cout<<"HIST_BINCOUNT_out :"<<HIST_BINCOUNT_out <<"  HIST_BINWIDTH_out: "<<HIST_BINWIDTH_out<<std::cout;
      rc = TDC_setHistogramParams( HIST_BINWIDTH_out, HIST_BINCOUNT_out );
      checkRc( "TDC_setHistogramParams", rc );

    }

    //////////////fix the delay////////////

    //std::cout<<in_PlotACh1 << "||||||||"<<in_PlotACh1 <<std::endl;
    for (int i=0;i<5;i++) {
        if(i==in_PlotACh1 || i==in_PlotBCh1|| i==in_PlotCCh1)delays[i]=in_histStart;
        else delays[i]=0;
        std::cout<<delays[i]<<"   ";

    }
    std::cout<<std::endl;
    rc = TDC_setChannelDelays(delays);
    checkRc( "TDC_setChannelDelays", rc );

    //filterset();


     paramschange=false;
    //std::cout<<" paramschange  :  "<<  paramschange<<std::endl;

 }

 void qutagadq::changThreshold(int ch, double val){

     rc = TDC_configureSignalConditioning(ch, SCOND_MISC, RoF[ch], val);
     checkRc( "TDC_configureSignalConditioning", rc );

 }


