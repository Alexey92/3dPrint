#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QUdpSocket"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QString>
#include <QTimer>
#include <QTime>

#include "move_platform.h"
#include "capacitors.h"



QSerialPort serial;
QUdpSocket *socket;
Platform_Control current_platform;
Capacitors caps;

bool gf_centering = false;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Настройка comboBox с com портами
    ui->comboBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->comboBox->addItem(info.portName());

    // Настройка comboBox с выбором шагов перемещения платформы
    ui->comboBox_small_step->addItem("0.1 mm");
    ui->comboBox_small_step->addItem("0.5 mm");
    ui->comboBox_small_step->addItem("1.0 mm");

    ui->comboBox_big_step->addItem("1 mm");
    ui->comboBox_big_step->addItem("5 mm");
    ui->comboBox_big_step->addItem("10 mm");

    // Настройка таймера
    tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
    tmr->setInterval(500); // Задаем интервал таймера
    connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime())); // Подключаем сигнал таймера к нашему слоту
    tmr->start(); // Запускаем таймер

    // Отключение неактивных в начале программы кнопок
    ui->button_serial_disconnect->setDisabled(true);
    ui->button_up->setDisabled(true);
    ui->button_down->setDisabled(true);
    ui->button_left->setDisabled(true);
    ui->button_right->setDisabled(true);
    ui->button_double_up->setDisabled(true);
    ui->button_double_down->setDisabled(true);
    ui->button_double_left->setDisabled(true);
    ui->button_double_right->setDisabled(true);
    ui->button_z_up->setDisabled(true);
    ui->button_z_down->setDisabled(true);
    ui->button_center->setDisabled(true);
}


MainWindow::~MainWindow()
{
    delete ui;
}

// Обновление строки с текущими координатами
void MainWindow::updateTime()
{
    ui->label_coord->setText("X: " + QString::number(current_platform.Axis_x.get_value()) +
                             " Y: " + QString::number(current_platform.Axis_y.get_value()) +
                             " Z: " + QString::number(current_platform.Axis_z.get_value()));
}

// Обновление списка com портов
void MainWindow::on_button_serial_update_clicked()
{
    ui->comboBox->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->comboBox->addItem(info.portName());
}

int cx_coord[10], cy_coord[10];

// Чтение UDP пакета от LABVIEW
void MainWindow::udp_read_data(void)
    {
    QByteArray array;
    QString coords_string;
    static int get_cnt = 0;

    // pendingDatagramSize - размер первого сообщения, ожидающего чтения. Функция resize нормализует размер массива в соответствии с размером параметра.
    // Введите в array.data () данные, которые не превышают размер array.size (), а array.data () возвращает указатель на расположение данных, хранящихся в байтовом массиве
    while(socket->hasPendingDatagrams())
        {

        array.resize(socket->pendingDatagramSize());
        socket->readDatagram(array.data(), array.size());

        caps.parse_coords_from_string(&array, &coords_string);

        if (caps.state == S_CENTRING)
            current_platform.move_relative(caps.move.y, caps.move.x, 0);

        ui->label_c_coords->setText(coords_string);

        ui->label_stat->setText("Get: " + QString::number(++get_cnt));
        }
    }

// Открытие соккета
void MainWindow::on_button_open_socket_clicked()
{
    ui->button_open_socket->setDisabled(true);
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

// Подключение к 3D принтеру по com порту
void MainWindow::on_button_serial_clicked()
{
    bool res = true;

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
    res = serial.open(QSerialPort::WriteOnly);

    if (res)
    {
        ui->button_serial_disconnect->setEnabled(true);
        ui->button_serial->setDisabled(true);

        ui->button_up->setEnabled(true);
        ui->button_down->setEnabled(true);
        ui->button_left->setEnabled(true);
        ui->button_right->setEnabled(true);
        ui->button_double_up->setEnabled(true);
        ui->button_double_down->setEnabled(true);
        ui->button_double_left->setEnabled(true);
        ui->button_double_right->setEnabled(true);
        ui->button_z_up->setEnabled(true);
        ui->button_z_down->setEnabled(true);
        ui->button_center->setEnabled(true);

//        current_platform.to_home();
        current_platform.move_abs(1000, 1000, 26);
    }
    else ui->label_4->setText("Connect Error!");
}

// Отключение от 3D принтера
void MainWindow::on_button_serial_disconnect_clicked()
{
    ui->button_serial_disconnect->setDisabled(true);
    ui->button_serial->setEnabled(true);

    ui->button_up->setDisabled(true);
    ui->button_down->setDisabled(true);
    ui->button_left->setDisabled(true);
    ui->button_right->setDisabled(true);
    ui->button_double_up->setDisabled(true);
    ui->button_double_down->setDisabled(true);
    ui->button_double_left->setDisabled(true);
    ui->button_double_right->setDisabled(true);
    ui->button_z_up->setDisabled(true);
    ui->button_z_down->setDisabled(true);
    ui->button_center->setDisabled(true);

    serial.close();
}

// Функции управления положением платформы
void MainWindow::on_button_up_clicked()
{
    current_platform.move_relative(0, current_platform.step_small_get(), 0);
}

void MainWindow::on_button_left_clicked()
{
    current_platform.move_relative(-current_platform.step_small_get(), 0, 0);
}

void MainWindow::on_button_right_clicked()
{
    current_platform.move_relative(current_platform.step_small_get(), 0, 0);
}

void MainWindow::on_button_down_clicked()
{
    current_platform.move_relative(0, -current_platform.step_small_get(), 0);
}

void MainWindow::on_button_double_up_clicked()
{
    current_platform.move_relative(0, current_platform.step_big_get(), 0);
}

void MainWindow::on_button_double_left_clicked()
{
    current_platform.move_relative(-current_platform.step_big_get(), 0, 0);
}

void MainWindow::on_button_double_right_clicked()
{
    current_platform.move_relative(current_platform.step_big_get(), 0, 0);
}

void MainWindow::on_button_double_down_clicked()
{
    current_platform.move_relative(0, -current_platform.step_big_get(), 0);
}

void MainWindow::on_button_z_down_clicked()
{
    current_platform.move_relative(0, 0, -1);
}

void MainWindow::on_button_z_up_clicked()
{
    current_platform.move_relative(0, 0, 1);
}

// Выбор длины "маленького шага" из comboBox
void MainWindow::on_comboBox_small_step_currentIndexChanged(int index)
{
    switch (index) {
        case 0: current_platform.step_small_set(1);     break;
        case 1: current_platform.step_small_set(5);     break;
        case 2: current_platform.step_small_set(10);    break;
        default:current_platform.step_small_set(1);     break;
    }
}

// Выбор длины "большого шага" из comboBox
void MainWindow::on_comboBox_big_step_currentIndexChanged(int index)
{
    switch (index) {
        case 0: current_platform.step_big_set(10);  break;
        case 1: current_platform.step_big_set(50);  break;
        case 2: current_platform.step_big_set(100); break;
        default:current_platform.step_big_set(10);  break;
    }
}

// Включение/выключение режима центровки конденсатора по изображению
void MainWindow::on_button_center_clicked()
{
    if (caps.state == S_CENTRING)
    {
        caps.state = S_FINDING;
        ui->button_center->setText("Centering ON");
    }
    else if (caps.state == S_FINDING)
    {
        ui->button_center->setText("Centering OFF");
        caps.state = S_CENTRING;
    }
}

