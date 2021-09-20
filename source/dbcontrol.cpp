#include "dbcontrol.h"
#include <iostream>
#include <time.h>

DBControl::DBControl(){

}

DBControl::~DBControl(){
    if(db.isOpen())db.close();
    std::cout<<"log off from DB"<<std::endl;
    delete fileh5;
    delete dataset;
    delete mspace1;
}

void DBControl::run(){

    this->DBConnect("localhost", 3306, "INQNET_GUI", "GUI2", "Teleport1536!");

}

void DBControl::DBConnect(QString server, int port, QString database, QString login, QString password){
    db = QSqlDatabase();
    db.removeDatabase("example-connection"); // remove old connection if exists
    db = QSqlDatabase::addDatabase("QMYSQL", "example-connection");

    connection_succesfull = connectToServerMySQL(server, port, database, login, password);
    if(connection_succesfull){
        std::cout<<"connection DB success"<<std::endl;
       QStringList tables_names = db.tables();
       emit MYtables(tables_names);
       /*for (int i = 0; i < tables_names.size(); ++i)
                 std::cout << tables_names.at(i).toLocal8Bit().constData() << std::endl;*/
       QSqlQuery query("create table if not exists inqnet_gui_tab2gates_V3(id int not null auto_increment primary key, and1 int,and2 int, and3 int, orgate int, bsm1 int, bsm2 int, and_adqtime float(7,2), delayline int,currentdelay double, attenuation double, datetime datetime);",db);
       QSqlQuery query2("create table if not exists inqnet_gui_historates(id int not null auto_increment primary key, Ra1 int,Ra2 int, Ra3 int, Rb1 int,Rb2 int, Rb3 int, Rc1 int,Rc2 int, Rc3 int ,hist_adqtime float(7,2), datetime datetime);",db);
       QSqlQuery query3("create table if not exists QKD_results(id int not null auto_increment primary key, det1ok int, det1err int, det1rand int, det1bkgnd int, det2ok int, det2err int, det2rand int, det2bkgnd int, det3ok int, det3err int, det3rand int, det3bkgnd int, datetime datetime)", db);
       QSqlQuery query4("create table if not exists QKD_stats_V3(id int not null auto_increment primary key, sifted_time int, sifted_phase int, error_rate_time double, error_rate_phase double, datetime datetime);",db);
       usleep(1000);
    }
    else
        std::cout << "connection to the database failed" << std::endl;
    //this->readQubits();
   /* this->createHDF5forQKDdata("test1.h5");
    QVector<int> apa(5);
    for (int i=0;i<5;i++) {
        this->appendQKDdata2HDF5(apa);
    }*/
}


bool DBControl::connectToServerMySQL(QString server, int port, QString database,
                                        QString login, QString password)
{
    db.setHostName(server);
    db.setPort(port);
    db.setDatabaseName(database);
    db.setUserName(login);
    db.setPassword(password);

    return db.open();
}

void DBControl::SaveAndValues(int and1, int and2, int and3, int orgate, int bsm1, int bsm2, float andTime, int delayline){
    QString s= "insert into inqnet_gui_tab2gates_V3(and1,and2, and3, orgate, bsm1, bsm2, and_adqtime, delayline, datetime) values("+QString::number(and1)+","+QString::number(and2)+","+QString::number(and3)+","+QString::number(orgate)+","+QString::number(bsm1)+","+QString::number(bsm2)+","+QString::number(andTime)+","+QString::number(delayline)+","+"now());";
    //std::cout<<s.toStdString()<<std::endl;
    if(connection_succesfull){QSqlQuery query(s,db);
       // std::cout<<"gud"<<std::endl;
    }

}

void DBControl::SaveRateValues( int Ra1, int Ra2, int Ra3, int Rb1, int Rb2, int Rb3,int Rc1 , int Rc2, int Rc3, float hist_adqtime){
    QString s= "insert into inqnet_gui_historates(Ra1, Ra2, Ra3, Rb1, Rb2, Rb3, Rc1, Rc2, Rc3, hist_adqtime, datetime) values("+QString::number(Ra1)+","+QString::number(Ra2)+","+QString::number(Ra3)+","+QString::number(Rb1)+","+QString::number(Rb2)+","+QString::number(Rb3)+","+QString::number(Rc1)+","+QString::number(Rc2)+","+QString::number(Rc3)+","+QString::number(hist_adqtime)+","+"now());";
   //std::cout<<s.toStdString()<<std::endl;
    if(connection_succesfull){
       QSqlQuery query(db);
   }
}

void DBControl::SaveQKDresults(double okA,double errA,double randA,double bkgndA,double okB,double errB,double randB, double bkgndB,double okC,double errC,double randC,double bkgndC){
    QString s= "insert into QKD_results(id int not null auto_increment primary key, det1ok , det1err , det1rand , det1bkgnd , det2ok , det2err , det2rand , det2bkgnd , det3ok , det3err , det3rand , det3bkgnd, datetime) values("+QString::number(okA)+","+QString::number(errA)+","+QString::number(randA)+","+QString::number(bkgndA)+","+QString::number(okB)+","+QString::number(errB)+","+QString::number(randB)+","+QString::number(bkgndB)+","+QString::number(okC)+","+QString::number(errC)+","+QString::number(randC)+","+QString::number(bkgndC)+", now());";
    //std::cout<<s.toStdString()<<std::endl;
    if(connection_succesfull){QSqlQuery query(s,db);
       // std::cout<<"gud"<<std::endl;
    }
}
void DBControl::SaveQKDstats(int sifted_time, int sifted_phase, double error_rate_time, double error_rate_phase){
    QString s= "insert into QKD_stats_V3(sifted_time, sifted_phase, error_rate_time, error_rate_phase, datetime) values("+QString::number(sifted_time)+","+QString::number(sifted_phase)+","+QString::number(error_rate_time)+","+QString::number(error_rate_phase)+","+"now());";
    std::cout<<s.toStdString()<<std::endl;
    if(connection_succesfull){QSqlQuery query(s,db);
       // std::cout<<"gud"<<std::endl;
    }
}


void DBControl::readQubits(QString tablename){
        std::cout<<tablename.toStdString()<<std::endl;
    if(connection_succesfull){
       QString s="SELECT * from " + tablename;
       QSqlQuery q(s,db);
       int a = q.record().count();//columns
       int b = q.size();//rows
       QVector<QVector<bool>> qubits;
       //int fieldNo = q.record().indexOf("det1ok1");
       QVector<bool> aux;
       while(q.next()){

           for (int i=1;i<a;i++) {
               aux.append(q.value(i).toBool());
           }
           qubits.append(aux);
           aux.clear();
       }
       /*for (int i=0;i<10;i++) {
           for(int j=0;j<a-1;j++){
                std::cout<<qubits[i][j]<<"||";
           }
           std::cout<<std::endl;
       }*/
       emit qubitsfromDB(qubits,a,b);
    }
}

void DBControl::createHDF5forQKDdata(QString name){

    //const H5std_string FILE_NAME( "SDSextendible.h5" );
    try{

        QString as = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH:mm:ss");
        const H5std_string DATASET_NAME(as.toLocal8Bit().data());
        //const int      NX = 10;
        //const int      NY = 5;

        hsize_t      dims[2]  = { 3, 3};  // dataset dimensions at creation
        hsize_t      maxdims[2] = {H5S_UNLIMITED, H5S_UNLIMITED};
        mspace1 = new H5::DataSpace( RANK, dims, maxdims);
        /*
         * Create a new file. If file exists its contents will be overwritten.
         */
        QString nameh5 = name + ".h5";
        fileh5 = new H5::H5File(nameh5.toLocal8Bit().data(), H5F_ACC_TRUNC);
        /*
         * Modify dataset creation properties, i.e. enable chunking.
         */
        H5::DSetCreatPropList cparms;
        hsize_t      chunk_dims[2] ={12, 500};
        cparms.setChunk( RANK, chunk_dims );
        /*
         * Set fill value for the dataset
         */
        int fill_val = 0;
        cparms.setFillValue( H5::PredType::NATIVE_INT, &fill_val);

        /*
         * Create a new dataset within the file using cparms
         * creation properties.
         */

       /*
        * Define datatype for the data in the file.
        * We will store little endian INT numbers.
        */
        H5::IntType datatype( H5::PredType::NATIVE_INT );
        datatype.setOrder( H5T_ORDER_LE );

        dataset = new H5::DataSet(fileh5->createDataSet( DATASET_NAME, datatype, *mspace1, cparms));

        size[1]   = 500;//
    }
    catch( H5::FileIException &error )
     {
        fprintf(stderr, "%s\n", error.getCDetailMsg());
        return ;
     }
    //QVector<int> data;
    //appendQKDdata2HDF5(data);
}

void DBControl::appendQKDdata2HDF5(QVector<int> dataokA, QVector<int> dataerrA, QVector<int> datarandA, QVector<int> databkgndA, QVector<int> dataokB, QVector<int> dataerrB, QVector<int> datarandB, QVector<int> databkgndB, QVector<int> dataokC, QVector<int> dataerrC, QVector<int> datarandC, QVector<int> databkgndC){

    if(fileh5 == 0)return;

  try{
    size[0] += 12;
    //std::cout<<size[0]<<"    "<<size[1]<<std::endl;
    dataset->extend( size );
    /*
     * Select a hyperslab.
     */
    H5::DataSpace fspace1 = dataset->getSpace ();
    hsize_t     offset[2];
    offset[1] = 0;
    offset[0] = chunkcounter*12;
    hsize_t      dims1[2] = { 12, 500 };            /* data1 dimensions */
    //std::cout<<offset[0]<<"    "<<offset[1]<<std::endl;
    fspace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

    hsize_t   dims2[2] = { 12, 500};
    H5::DataSpace mspace2( RANK, dims2 );
    /*
     * Write the data to the hyperslab.
    */
    int data1[12][500]={{0}};
    /*for (int i =0; i<4;i++) {
        for (int j=0;j<500;j++) {
            data1[i][j]=int(chunkcounter);
        }
    }*/
    //while(data.)
    for (int i=0;i<dataokA.size();i++)data1[0][i]= dataokA[i];
    for (int i=0;i<dataerrA.size();i++)data1[1][i]= dataerrA[i];
    for (int i=0;i<datarandA.size();i++)data1[2][i]= datarandA[i];
    for (int i=0;i<databkgndA.size();i++)data1[3][i]= databkgndA[i];

    for (int i=0;i<dataokB.size();i++)data1[4][i]= dataokB[i];
    for (int i=0;i<dataerrB.size();i++)data1[5][i]= dataerrB[i];
    for (int i=0;i<datarandB.size();i++)data1[6][i]= datarandB[i];
    for (int i=0;i<databkgndB.size();i++)data1[7][i]= databkgndB[i];

    for (int i=0;i<dataokC.size();i++)data1[8][i]= dataokC[i];
    for (int i=0;i<dataerrC.size();i++)data1[9][i]= dataerrC[i];
    for (int i=0;i<datarandC.size();i++)data1[10][i]= datarandC[i];
    for (int i=0;i<databkgndC.size();i++)data1[11][i]= databkgndC[i];

    dataset->write( data1, H5::PredType::NATIVE_INT, mspace2, fspace1 );
    chunkcounter++;

  }
    catch( H5::FileIException &error )
     {
        fprintf(stderr, "%s\n", error.getCDetailMsg());
        return ;
     }
}


void DBControl::disconnectFromServer()
{
    db.close();
}


