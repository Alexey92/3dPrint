#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_button_open_socket_clicked();
    void udp_read_data(void);
    void on_button_serial_clicked();
    void on_button_serial_disconnect_clicked();
    void on_button_serial_write_clicked();
    void updateTime();
    void on_button_double_up_clicked();
    void on_button_up_clicked();
    void on_button_left_clicked();
    void on_button_right_clicked();
    void on_button_down_clicked();
    void on_button_double_left_clicked();
    void on_button_double_right_clicked();
    void on_button_double_down_clicked();
    void on_button_serial_update_clicked();
    void on_comboBox_small_step_currentIndexChanged(int index);
    void on_comboBox_big_step_currentIndexChanged(int index);
    void on_button_center_clicked();
    void on_button_z_down_clicked();
    void on_button_z_up_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *tmr; //Адресная переменная таймера
};
#endif // MAINWINDOW_H
