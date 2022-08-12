#include "dbcontrol.h"
#include <iostream>
#include <time.h>

#include "logging.h"

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

void DBControl::createHDF5forQKDdata(QString file_name){

    //const H5std_string FILE_NAME( "SDSextendible.h5" );
    try{
        /*
        QString as = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH:mm:ss");
        const H5std_string DATASET_NAME(as.toLocal8Bit().data());
        */
        //const int      NX = 10;
        //const int      NY = 5;

        /*
        hsize_t      dims[2]  = { 3, 3};  // dataset dimensions at creation
        hsize_t      maxdims[2] = {H5S_UNLIMITED, H5S_UNLIMITED};
        mspace1 = new H5::DataSpace( RANK, dims, maxdims);
        */

        /*
         * Create a new file.
         * If file is opened, do nothing if it has the same name as the previous one.
         * Otherwise, we close the previous one and open a new one.
         */
        H5::Exception::dontPrint();
        if (fileh5 != 0) {
            if (file_name == fileh5_name) {
                return;
            } else {
                delete fileh5;
            }
        }

        fileh5_name = file_name;
        QString file_path = "../data/" + file_name + ".h5";
        // Try to open a file for read write. Otherwise, create the file.
        try {
            fileh5 = new H5::H5File(file_path.toLocal8Bit().data(), H5F_ACC_RDWR);
        } catch(H5::FileIException &file_exists_err) {
            fileh5 = new H5::H5File(file_path.toLocal8Bit().data(), H5F_ACC_TRUNC);
        }

        /*
         * Modify dataset creation properties, i.e. enable chunking.
         */
        /*
        H5::DSetCreatPropList cparms;
        hsize_t      chunk_dims[2] ={12, 500};
        cparms.setChunk( RANK, chunk_dims );
        */
        /*
         * Set fill value for the dataset
         */
        /*
        int fill_val = 0;
        cparms.setFillValue( H5::PredType::NATIVE_INT, &fill_val);
        */

        /*
         * Create a new dataset within the file using cparms
         * creation properties.
         */

       /*
        * Define datatype for the data in the file.
        * We will store little endian INT numbers.
        */
        /*
        H5::IntType datatype( H5::PredType::NATIVE_INT );
        datatype.setOrder( H5T_ORDER_LE );
        */

        // dataset = new H5::DataSet(fileh5->createDataSet( DATASET_NAME, datatype, *mspace1, cparms));

        // size[1]   = 500;
    }
    catch( H5::FileIException &error )
    {
        fprintf(stderr, "%s\n", error.getCDetailMsg());
        return;
    }
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
    H5::DataSpace fspace1 = dataset->getSpace();
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

void DBControl::savePlotToHDF5(QCustomPlot *plot, QString plot_name, QString group_path)
{
    if (fileh5 == 0) return;

    try {
        H5::Exception::dontPrint();

        // open group or make it if it doesn't exist
        H5::Group *group;
        try {
            group = new H5::Group(fileh5->openGroup(group_path.toLocal8Bit().data()));
        }
        catch (...) {
            group = new H5::Group(fileh5->createGroup(group_path.toLocal8Bit().data()));
        }

        // Given plot, number of "graphs" is dims[0]
        // number of data points in a graph is dims[1]
        hsize_t dims[2];
        size_t graph_count = plot->graphCount();
        dims[0] = 2 * graph_count;
        dims[1] = 0;
        // Take the largest dataCount for the other dimension
        size_t curr_d_count;
        for (size_t i=0; i < graph_count; i++) {
            curr_d_count = plot->graph(i)->dataCount();
            if (curr_d_count > dims[1]) {
                dims[1] = curr_d_count;
            }
        }

        H5::DataSpace dspace(RANK, dims);

        /*
         * Modify dataset creation properties, i.e. enable chunking.
         */
        H5::DSetCreatPropList cparms;
        hsize_t      chunk_dims[2] ={12, 500};
        cparms.setChunk( RANK, chunk_dims );
        // Set fill value for the dataset
        int fill_val = 0;
        cparms.setFillValue( H5::PredType::NATIVE_FLOAT, &fill_val);
        /*
         * Define datatype for the data in the file.
         * We will store little endian DOUBLE numbers.
         */
        H5::FloatType datatype( H5::PredType::NATIVE_FLOAT );
        datatype.setOrder( H5T_ORDER_LE );

        QString dataset_name = group_path.append(plot_name.prepend("/"));
        H5::DataSet datasetp = fileh5->createDataSet(dataset_name.toLocal8Bit().data(), datatype, dspace);

        /*
         * Method for storing and writing data borrowed from:
         * https://support.hdfgroup.org/ftp/HDF5/examples/misc-examples/h5_writedyn.c
         */
        float **data = (float**) malloc(dims[0]*sizeof(float*));
        data[0] = (float*)malloc( dims[0]*dims[1]*sizeof(float));
        for (size_t i=1; i<dims[0]; i++) data[i] = data[0]+i*dims[1];
        // Load graph data into data vector for writing
        for (size_t g=0; g < graph_count; g++) {
            QCPGraph *graph = plot->graph(g);

            for (size_t i=0; i < dims[1]; i++) {
                data[2*g][i] = (float) graph->dataMainKey(i);
                data[2*g + 1][i] = (float) graph->dataMainValue(i);
            }
        }

        datasetp.write( &data[0][0], H5::PredType::NATIVE_FLOAT );

        free(data[0]);
        free(data);
        delete group;
    }

    catch (...) {

    }
}

// Make a new group if it doesn't already exist.
void DBControl::tryMakeGroupHDF5(QString group_path) {
    H5::Exception::dontPrint();

    H5::Group *group;
    try {
        group = new H5::Group(fileh5->openGroup(group_path.toLocal8Bit().data()));
        delete group;
    }
    catch (...) {
        group = new H5::Group(fileh5->createGroup(group_path.toLocal8Bit().data()));
        delete group;
    }
}

/*
 * Writes the following attributes to the H5 file:
 * timestamp : simple timestamp of when this function is entered.
 * qubit_seq : qubit sequence used for histogram matching and entered on phase stabilization page.
 * qkd_param_* : every field following ABC order from the Edit -> QKD parameter page.
 * adq_time : update rate / adq time from histogram page.
 */
void DBControl::writeAttrToHDF5(QKD_param *param, char qubit_sequence[100000], float adq_time) {
    try {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH:mm:ss");
        H5::StrType str_type_t(H5::PredType::C_S1, timestamp.length() + 1);
        H5::DataSpace s_space(H5S_SCALAR);
        H5::Attribute *attr_t = new H5::Attribute(fileh5->createAttribute("timestamp", str_type_t, s_space));
        attr_t->write(str_type_t, timestamp.toStdString());

        H5::StrType str_type_seq(H5::PredType::C_S1, strlen(qubit_sequence) + 1);
        H5::Attribute *attr_seq = new H5::Attribute(fileh5->createAttribute("qubit_seq", str_type_seq, s_space));
        attr_seq->write(str_type_seq, qubit_sequence);

        // Types and spaces for QKD params metadata. Want to write arrays of 3 numbers.
        H5::FloatType float_type_p( H5::PredType::NATIVE_FLOAT );
        float_type_p.setOrder( H5T_ORDER_LE );
        H5::IntType int_type_p( H5::PredType::NATIVE_INT );
        int_type_p.setOrder( H5T_ORDER_LE );
        hsize_t dims[1] = {3};
        H5::DataSpace qkd_p_space(1, dims);

        // These hold the data from param. There are several sets of 3 int and double parameters taken from qkd_param.cpp.
        float qkd_floats[3] = {(float) param->ui->QKD_timeA->value(), (float) param->ui->QKD_timeB->value(), (float) param->ui->QKD_timeC->value()};
        int qkd_ints[3] = {(int) param->ui->QKD_numbA->value(), (int) param->ui->QKD_numbB->value(), (int) param->ui->QKD_numbC->value()};

        H5::Attribute *attr_qkd_times = new H5::Attribute(fileh5->createAttribute("qkd_param_qubit_times", float_type_p, qkd_p_space));
        attr_qkd_times->write(float_type_p, qkd_floats);

        H5::Attribute *attr_qkd_nums = new H5::Attribute(fileh5->createAttribute("qkd_param_num_qubits", int_type_p, qkd_p_space));
        attr_qkd_nums->write(int_type_p, qkd_ints);

        qkd_floats[0] = (float) param->ui->QKD_phA->value();
        qkd_floats[1] = (float) param->ui->QKD_phB->value();
        qkd_floats[2] = (float) param->ui->QKD_phC->value();
        H5::Attribute *attr_qkd_phases = new H5::Attribute(fileh5->createAttribute("qkd_param_phases", float_type_p, qkd_p_space));
        attr_qkd_phases->write(float_type_p, qkd_floats);

        qkd_ints[0] = (int) param->ui->QKD_iwA->value();
        qkd_ints[1] = (int) param->ui->QKD_iwB->value();
        qkd_ints[2] = (int) param->ui->QKD_iwC->value();
        H5::Attribute *attr_qkd_int_wins = new H5::Attribute(fileh5->createAttribute("qkd_param_integration_windows", int_type_p, qkd_p_space));
        attr_qkd_int_wins->write(int_type_p, qkd_ints);

        qkd_ints[0] = (int) param->ui->QKD_zeroA->value();
        qkd_ints[1] = (int) param->ui->QKD_zeroB->value();
        qkd_ints[2] = (int) param->ui->QKD_zeroC->value();
        H5::Attribute *attr_qkd_offsets = new H5::Attribute(fileh5->createAttribute("qkd_param_offsets", int_type_p, qkd_p_space));
        attr_qkd_offsets->write(int_type_p, qkd_ints);

        H5::Attribute *attr_adq_time = new H5::Attribute(fileh5->createAttribute("hist_adq_time", float_type_p, s_space));
        attr_adq_time->write(float_type_p, &adq_time);

        delete attr_t;
        delete attr_seq;
        delete attr_qkd_times;
        delete attr_qkd_nums;
        delete attr_qkd_phases;
        delete attr_qkd_int_wins;
        delete attr_qkd_offsets;
        delete attr_adq_time;
    }
    catch( H5::AttributeIException &error )
    {
        fprintf(stderr, "%s\n", error.getCDetailMsg());
        return;
    }
}
