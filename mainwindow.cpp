#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QUdpSocket"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QString>
#include <QTimer>
#include <QTime>

#include "move_platform.h"

#define ASCII_CR        13
#define ASCII_SPACE     32

int GX_COORD, GY_COORD, GZ_COORD;
int GX_COORD_MAX = 1600, GY_COORD_MAX = 1600, GZ_COORD_MAX = 50;
int GX_COORD_MIN = 0, GY_COORD_MIN = 0, GZ_COORD_MIN = 2;

int G_SMALL_STEP = 1, G_BIG_STEP = 10;

QSerialPort serial;

int g_snd_cnt = 0, g_get_cnt = 0;
QUdpSocket *socket;


bool gf_centering = false;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBox->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->comboBox->addItem(info.portName());

    tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
    tmr->setInterval(1000); // Задаем интервал таймера
    connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime())); // Подключаем сигнал таймера к нашему слоту
    tmr->start(); // Запускаем таймер

    ui->button_serial_disconnect->setDisabled(true);


    ui->comboBox_small_step->addItem("0.1 mm");
    ui->comboBox_small_step->addItem("0.5 mm");
    ui->comboBox_small_step->addItem("1.0 mm");

    ui->comboBox_big_step->addItem("1 mm");
    ui->comboBox_big_step->addItem("5 mm");
    ui->comboBox_big_step->addItem("10 mm");
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTime()
{
    ui->label_coord->setText("X: " + QString::number(GX_COORD) + " Y: " + QString::number(GY_COORD));
}

void MainWindow::on_button_serial_update_clicked()
{
    ui->comboBox->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->comboBox->addItem(info.portName());
}

// ------------ Считать данные, отправленные сервером ------------- //
void MainWindow::udp_read_data(void)
    {
    QByteArray array;

    int cnt_of_c;
    int cx_coord[10], cy_coord[10];
    int delta_x, delta_y;

    // pendingDatagramSize - размер первого сообщения, ожидающего чтения. Функция resize нормализует размер массива в соответствии с размером параметра.
    // Введите в array.data () данные, которые не превышают размер array.size (), а array.data () возвращает указатель на расположение данных, хранящихся в байтовом массиве
    while(socket->hasPendingDatagrams())
        {
//        qDebug()<<"get data";

        array.resize(socket->pendingDatagramSize());
        socket->readDatagram(array.data(), array.size());


        static int cnt_without_data = 0;
        if (array.at(0) != ASCII_CR)
        {
            cnt_without_data = 0;
//            ui->label_c_coords->setText(array.data());

            int symb_nbm = 2;

            if (array.at(1) == ASCII_SPACE)
            {
                cnt_of_c = array.at(0) - 48; // ASCII numeric

                for (int i = 0; i < cnt_of_c; i++)
                {
                    // X
                    if (array.at(symb_nbm + 1) == ASCII_SPACE)
                    {
                        cx_coord[i] = array.at(symb_nbm) - 48;
                        symb_nbm+= 2;
                    }
                    else if (array.at(symb_nbm + 2) == ASCII_SPACE)
                    {
                        cx_coord[i] = (array.at(symb_nbm) - 48) * 10 + (array.at(symb_nbm + 1) - 48);
                        symb_nbm+= 3;
                    }
                    else if (array.at(symb_nbm + 3) == ASCII_SPACE)
                    {
                        cx_coord[i] = (array.at(symb_nbm) - 48) * 100 + (array.at(symb_nbm + 1) - 48) * 10 + (array.at(symb_nbm + 2) - 48);
                        symb_nbm+= 4;
                    }
                    else qDebug()<<"get data ERROR";


                    // Y
                    if (array.at(symb_nbm + 1) == ASCII_SPACE)
                    {
                        cy_coord[i] = array.at(symb_nbm) - 48;
                        symb_nbm+= 2;
                    }
                    else if (array.at(symb_nbm + 2) == ASCII_SPACE)
                    {
                        cy_coord[i] = (array.at(symb_nbm) - 48) * 10 + (array.at(symb_nbm + 1) - 48);
                        symb_nbm+= 3;
                    }
                    else if (array.at(symb_nbm + 3) == ASCII_SPACE)
                    {
                        cy_coord[i] = (array.at(symb_nbm) - 48) * 100 + (array.at(symb_nbm + 1) - 48) * 10 + (array.at(symb_nbm + 2) - 48);
                        symb_nbm+= 4;
                    }
                    else qDebug()<<"get data ERROR";


                    ui->label_c_coords->setText("X: "+ QString::number(cx_coord[i]) + " Y: " + QString::number(cy_coord[i]) + "\n");
                }
            }
            else ui->label_c_coords->setText("Too much data");
        }
        else cnt_without_data++;

        if (cnt_without_data > 1000)
        {
            ui->label_c_coords->setText("No data");
            cnt_without_data = 1000;
            cnt_of_c = 0;
        }
//
        if (gf_centering && cnt_of_c > 0)
        {
            delta_x = cx_coord[0] - 310;
            delta_y = cy_coord[0] - 200;

            if (delta_x > 10) delta_x = 1;
            else if (delta_x < -10) delta_x = -1;
            else delta_x = 0;

            if (delta_y > 10) delta_y = 1;
            else if (delta_y < -10) delta_y = -1;
            else delta_y = 0;

            ui->label_4->setText("cx_coord: " + QString::number(cx_coord[0]) + "| delta_x: " + QString::number(delta_x));

            move_relative(delta_y, delta_x);
        }

//        qDebug()<<array.data();

        ui->label_stat->setText("Send: " + QString::number(g_snd_cnt) + "| Get: " + QString::number(++g_get_cnt));
        }
    }

void MainWindow::on_button_open_socket_clicked()
{
    socket = new QUdpSocket(this);

    // Отслеживаем, есть ли данные, поступающие в виде сигналов и слотов. Сигнал readyRead () срабатывает при появлении новых данных.
    QObject::connect(socket, &QUdpSocket::readyRead, this, &MainWindow::udp_read_data);

    // Поскольку клиент также получает данные, отправленные сервером, клиент также должен связать локальный IP-адрес и номер порта
    if(socket->bind(QHostAddress("127.0.0.1"), 4000) > 0)
    {
        ui->label->setText("Connect: ON");
        qDebug()<<"bind sucess";
    }
    else
    {
        ui->label->setText("Connect: ERROR");
        qDebug()<<"bind error";
    }
}

void MainWindow::on_button_send_udp_clicked()
{
    QByteArray msg = "Hello world";

    // Отправка на определенный IP, writeDatagram успешно возвращает количество байтов, иначе -1
    QHostAddress serverAddress = QHostAddress("127.0.0.1");

    if(socket->writeDatagram(msg.data(), msg.size(), serverAddress, 4000) > 0)
        {
        qDebug()<<"send ok";
        ui->label_stat->setText("Send: " + QString::number(++g_snd_cnt) + "| Get: " + QString::number(g_get_cnt));
        }
    else    qDebug()<<socket->error();
}

void MainWindow::on_button_serial_clicked()
{
    ui->button_serial_disconnect->setEnabled(true);
    ui->button_serial->setDisabled(true);

    //select name for our serial port from combobox
    if (serial.portName() != ui->comboBox->currentText())
    {
        serial.close();
        serial.setPortName(ui->comboBox->currentText());
    }

    //setup COM port
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.open(QSerialPort::WriteOnly);

//    to_home();
    move_z(6);
    move_abs(1000, 1000);
}

void MainWindow::on_button_serial_disconnect_clicked()
{
    ui->button_serial_disconnect->setDisabled(true);
    ui->button_serial->setEnabled(true);

    serial.close();
}

void MainWindow::on_button_serial_write_clicked()
{
    QString str = "\nG0 X20 Y30\n";

    serial.write(str.toUtf8());

    str = "\nG0 X30 Y20\n";

    serial.write(str.toUtf8());
}





void MainWindow::on_button_up_clicked()
{
    move_relative(0, G_SMALL_STEP);
}


void MainWindow::on_button_left_clicked()
{
    move_relative(-G_SMALL_STEP, 0);
}

void MainWindow::on_button_right_clicked()
{
    move_relative(G_SMALL_STEP, 0);
}

void MainWindow::on_button_down_clicked()
{
    move_relative(0, -G_SMALL_STEP);
}

void MainWindow::on_button_double_up_clicked()
{
    move_relative(0, G_BIG_STEP);
}

void MainWindow::on_button_double_left_clicked()
{
    move_relative(-G_BIG_STEP, 0);
}

void MainWindow::on_button_double_right_clicked()
{
    move_relative(G_BIG_STEP, 0);
}

void MainWindow::on_button_double_down_clicked()
{
    move_relative(0, -G_BIG_STEP);
}



void MainWindow::on_comboBox_small_step_currentIndexChanged(int index)
{
    switch (index) {
        case 0: G_SMALL_STEP = 1; break;
        case 1: G_SMALL_STEP = 5; break;
        case 2: G_SMALL_STEP = 10; break;
        default:G_SMALL_STEP = 1; break;
    }
}

void MainWindow::on_comboBox_big_step_currentIndexChanged(int index)
{
    switch (index) {
        case 0: G_BIG_STEP = 10; break;
        case 1: G_BIG_STEP = 50; break;
        case 2: G_BIG_STEP = 100; break;
        default:G_BIG_STEP = 10; break;
    }
}



void MainWindow::on_button_center_clicked()
{
    if (gf_centering)
    {
        gf_centering = false;
        ui->button_center->setText("Centering ON");
    }
    else
    {
        ui->button_center->setText("Centering OFF");
        gf_centering = true;
    }
}

void MainWindow::on_button_z_down_clicked()
{
    to_relative_coord();

    QString cmd;

    cmd = QString("\nG0 Z-1\n");

    serial.write(cmd.toUtf8());
}

void MainWindow::on_button_z_up_clicked()
{
    to_relative_coord();

    QString cmd;

    cmd = QString("\nG0 Z1\n");

    serial.write(cmd.toUtf8());
}
