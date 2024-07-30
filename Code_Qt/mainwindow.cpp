#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(int updateRate, QWidget *parent):
    QMainWindow(parent)
{
    // Constructeur de la classe
    // Initialisation du UI
    ui = new Ui::MainWindow;
    ui->setupUi(this);

    // Ajoute couleur au LCDNUMBER
        auto powerPalette = ui->Power_Display->palette();
        powerPalette.setColor(powerPalette.Light, QColor(0, 0, 0));
        ui->Power_Display->setPalette(powerPalette);

        auto energyPalette = ui->Energy_Display->palette();
        energyPalette.setColor(energyPalette.Light, QColor(0, 0, 0));
        ui->Energy_Display->setPalette(energyPalette);

        auto pendulumPalette = ui->display_pendulum->palette();
        pendulumPalette.setColor(pendulumPalette.Light, QColor(0, 0, 0));
        ui->display_pendulum->setPalette(pendulumPalette);

        auto distancePalette = ui->distance_display->palette();
        distancePalette.setColor(distancePalette.Light, QColor(0, 0, 0));
        ui->distance_display->setPalette(distancePalette);

    // Initialisation du graphique
    ui->graph->setChart(&chart_);
    chart_.setTitle("Donnees brutes");
    chart_.legend()->hide();
    chart_.addSeries(&series_);

    // Fonctions de connections events/slots
    connectTimers(updateRate);
    connectButtons();
    connectSpinBoxes();
    connectTextInputs();
    connectComboBox();

    // Recensement des ports
    portCensus();

    // initialisation du timer
    updateTimer_.start();
}

MainWindow::~MainWindow(){
    // Destructeur de la classe
    updateTimer_.stop();
    if(serialCom_!=nullptr){
      delete serialCom_;
    }
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event){
    // Fonction appelee lorsque la fenetre est detruite
    event->accept();
}

void MainWindow::receiveFromSerial(QString msg){
    // Fonction appelee lors de reception sur port serie
    // Accumulation des morceaux de message
    msgBuffer_ += msg;

    //Si un message est termine
    if(msgBuffer_.endsWith('\n')){
        // Passage ASCII vers structure Json
        QJsonDocument jsonResponse = QJsonDocument::fromJson(msgBuffer_.toUtf8());

        // Analyse du message Json
        if(~jsonResponse.isEmpty())
        {
            QJsonObject jsonObj = jsonResponse.object();
            QString buff = jsonResponse.toJson(QJsonDocument::Indented);

            // Affichage des messages Json
            ui->textBrowser->setText(buff.mid(2,buff.length()-4));

            // Affichage des donnees dans le graph
            if(jsonObj.contains(JsonKey_)){
                double time = jsonObj["time"].toDouble();
                series_.append(time, jsonObj[JsonKey_].toDouble());
                // Mise en forme du graphique (non optimal)
                chart_.removeSeries(&series_);
                chart_.addSeries(&series_);
                chart_.createDefaultAxes();
            }

            if (jsonObj.contains("current") && jsonObj.contains("voltage") && jsonObj.contains("time")) //Displaying Power and Energy
            {
                double current = jsonObj["current"].toDouble();
                double voltage = jsonObj["voltage"].toDouble();
                double time = jsonObj["time"].toDouble();

                power_ = current * voltage;


                double timeSeconds = time / 1000.0;

                double DeltaTime = timeSeconds - previousTime_;


                if (previousTime_ != 0.0)
                {
                    energy_ = power_ * DeltaTime + energy_;



                }
                    previousTime_ = timeSeconds;


                     ui->Power_Display->display(power_);
                     ui->Energy_Display->display(energy_);

            }

            if (jsonObj.contains("distanceTravelled")) //Displaying Distance travelled
            {
                double distanceTravelled = jsonObj["encVex"].toDouble();
                displayDistance(distanceTravelled);
            }

            if (jsonObj.contains("potVex"))
            {
                double potVexValue = jsonObj["potVex"].toDouble();
                displayPendulum(potVexValue);
             }
            // Fonction de reception de message (vide pour l'instant)
            msgReceived_ = msgBuffer_;
            onMessageReceived(msgReceived_);

            // Si les donnees doivent etre enregistrees
            if(record){
                writer_->write(jsonObj);
            }
        }
        // Reinitialisation du message tampon
        msgBuffer_ = "";
    }
}

void MainWindow::connectTimers(int updateRate){
    // Fonction de connection de timers
    connect(&updateTimer_, &QTimer::timeout, this, [this]{onPeriodicUpdate();});
    updateTimer_.start(updateRate);
}

void MainWindow::connectSerialPortRead(){
    // Fonction de connection au message de la classe (serialProtocol)
    connect(serialCom_, SIGNAL(newMessage(QString)), this, SLOT(receiveFromSerial(QString)));
}

void MainWindow::connectButtons(){
    // Fonction de connection du boutton Send
    connect(ui->pulseButton, SIGNAL(clicked()), this, SLOT(sendPulseStart()));
    connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(manageRecording(int)));
    connect(ui->pushButton_Params, SIGNAL(clicked()), this, SLOT(sendPID()));
    connect(ui->pb_mode_man, SIGNAL(clicked()), this, SLOT(manuelMode()));
    connect(ui->pb_start_auto, SIGNAL(clicked()), this, SLOT(startAuto()));
    connect(ui->pb_stop_auto, SIGNAL(clicked()), this, SLOT(stopAuto()));
    connect(ui->pb_ElectroAimantON, SIGNAL(clicked()), this, SLOT(electroAimantStart()));
    connect(ui->pb_ElectroAimantOFF, SIGNAL(clicked()), this, SLOT(electroAimantStop()));

}

void MainWindow::connectSpinBoxes(){
    // Fonction de connection des spin boxes
    connect(ui->DurationBox, SIGNAL(valueChanged(int)), this, SLOT(sendPulseSetting()));
    connect(ui->PWMBox, SIGNAL(valueChanged(double)), this, SLOT(sendPulseSetting()));
}

void MainWindow::connectTextInputs(){
    // Fonction de connection des entrees de texte
    connect(ui->JsonKey, SIGNAL(returnPressed()), this, SLOT(changeJsonKeyValue()));
    JsonKey_ = ui->JsonKey->text();
}

void MainWindow::connectComboBox(){
    // Fonction de connection des entrees deroulantes
    connect(ui->comboBoxPort, SIGNAL(activated(QString)), this, SLOT(startSerialCom(QString)));
}

void MainWindow::portCensus(){
    // Fonction pour recenser les ports disponibles
    ui->comboBoxPort->clear();
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
        ui->comboBoxPort->addItem(port.portName());
    }
}

void MainWindow::startSerialCom(QString portName){
    // Fonction SLOT pour demarrer la communication serielle
    qDebug().noquote() << "Connection au port" << portName;
    if(serialCom_!=nullptr){
        delete serialCom_;
    }
    serialCom_ = new SerialProtocol(portName, BAUD_RATE);
    connectSerialPortRead();
}

void MainWindow::changeJsonKeyValue(){
    // Fonction SLOT pour changer la valeur de la cle Json
    series_.clear();
    JsonKey_ = ui->JsonKey->text();
}

void MainWindow::sendPID(){
    // Fonction SLOT pour envoyer les paramettres de pulse
    float goal = ui->lineEdit_DesVal->text().toFloat();
    float kp = ui->lineEdit_Kp->text().toFloat();
    float ki = ui->lineEdit_Ki->text().toFloat();
    float kd = ui->lineEdit_Kd->text().toFloat();
    float thresh = ui->lineEdit_Thresh->text().toFloat();
    float slow_speed = ui->lineEdit_slowSpeed->text().toFloat();
    float fast_speed = ui->lineEdit_fastSpeed->text().toFloat();
    float drop_position = ui->lineEdit_dropPosition->text().toFloat();
    float end_position = ui->lineEdit_endPosition->text().toFloat();
    float init_reverse_position = ui->lineEdit_initReversePosition->text().toFloat();
    float time_stop_pendulum = ui->lineEdit_timeStopPendulum->text().toFloat();
    // pour minimiser le nombre de decimales( QString::number)

    QJsonArray array = { QString::number(kp, 'f', 2),
                         QString::number(ki, 'f', 2),
                         QString::number(kd, 'f', 2),
                         QString::number(thresh, 'f', 2),
                         QString::number(goal, 'f', 2),
                         QString::number(slow_speed, 'f', 2),
                         QString::number(fast_speed, 'f', 2),
                         QString::number(drop_position, 'f', 2),
                         QString::number(end_position, 'f', 2),
                         QString::number(init_reverse_position, 'f', 2),
                         QString::number(time_stop_pendulum, 'f', 2),

                       };
    QJsonObject jsonObject
    {
        {"setGoal", array}
    };
    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    sendMessage(strJson);
}
void MainWindow::sendPulseSetting(){
    // Fonction SLOT pour envoyer les paramettres de pulse
    double PWM_val = ui->PWMBox->value();
    int duration_val = ui->DurationBox->value();
    QJsonObject jsonObject
    {// pour minimiser le nombre de decimales( QString::number)
        {"pulsePWM", QString::number(PWM_val)},
        {"pulseTime", duration_val}
    };
    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    sendMessage(strJson);
}

void MainWindow::sendPulseStart(){
    // Fonction SLOT pour envoyer la commande de pulse
    QJsonObject jsonObject
    {
        {"pulse", 1}
    };
    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    sendMessage(strJson);
}

void MainWindow::sendMessage(QString msg){
    // Fonction SLOT d'ecriture sur le port serie
    if(serialCom_==nullptr){
        qDebug().noquote() <<"Erreur aucun port serie !!!";
        return;
    }
    serialCom_->sendMessage(msg);
    qDebug().noquote() <<"Message du RPI: "  <<msg;
}

void MainWindow::setUpdateRate(int rateMs){
    // Fonction d'initialisation du chronometre
    updateTimer_.start(rateMs);
}

void MainWindow::manageRecording(int stateButton){
    // Fonction SLOT pour determiner l'etat du bouton d'enregistrement
    if(stateButton == 2){
        startRecording();
    }
    if(stateButton == 0){
        stopRecording();
    }
}

void MainWindow::startRecording(){
    // Fonction SLOT pour creation d'un nouveau fichier csv
    record = true;
    writer_ = new CsvWriter("/home/pi/Desktop/");
    ui->label_pathCSV->setText(writer_->folder+writer_->filename);
}

void MainWindow::stopRecording(){
    // Fonction permettant d'arreter l'ecriture du CSV
    record = false;
    delete writer_;
}

void MainWindow::onMessageReceived(QString msg){
    // Fonction appelee lors de reception de message
    // Decommenter la ligne suivante pour deverminage
    // qDebug().noquote() << "Message du Arduino: " << msg;
}

void MainWindow::onPeriodicUpdate(){
    // Fonction SLOT appelee a intervalle definie dans le constructeur
    qDebug().noquote() << "*";
}


void MainWindow::electroAimantStart()
{
    magnetOn = true;
    QJsonObject jsonObject {
        {"magnet", magnetOn}
    };
    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    sendMessage(strJson);
}

void MainWindow::electroAimantStop()
{
    magnetOn = false;
    QJsonObject jsonObject {
        {"magnet", magnetOn}
    };
    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    sendMessage(strJson);
}


void MainWindow::startAuto()
{
    bool startAuto = true;
    QJsonObject jsonObject {
        {"startAuto", startAuto} //Vérifier variables du code avec nath
    };
    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    sendMessage(strJson);

}

void MainWindow::stopAuto()
{
    bool startAuto = false;
    QJsonObject jsonObject {
        {"startAuto", startAuto} //Vérifier variable avec nath
    };
    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    sendMessage(strJson);

}

void MainWindow::manuelMode()
{
    bool startMan = true;
    QJsonObject jsonObject {
        {"startMan", startMan} //Vérifier variable avec nath
    };
    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    sendMessage(strJson);

}



void MainWindow::restMode()
{

    bool restMode = true;
    QJsonObject jsonObject {
        {"restMode", restMode} //Vérifier variable avec nath
    };
    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    sendMessage(strJson);

}




void MainWindow::displayDistance(double distanceTravelled) //Vérifier si cette technique marche pis fait pas juste afficher des gros jump de valeurs
{

  //const double encoderCountsPerRevolution = 3200;
  //const double wheelCircumference = 0.628; // s'assurer que c'est good


  //double revolutions = encoderValue / encoderCountsPerRevolution;


  //double distance = revolutions * wheelCircumference;


    ui->distance_display->display(distanceTravelled);
}

void MainWindow::displayPendulum(double potVex)
{
    ui->display_pendulum->display(potVex);
}


