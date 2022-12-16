#ifndef __MOVE_PLATFORM_H_
#define __MOVE_PLATFORM_H_

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QString>

extern QSerialPort serial;


class Axis
{
public:
    int current_value;
    int max_value;
    int min_value;

    Axis(int value, int max, int min);
    bool set_value(int value);
    bool add_value(int value);
    int get_value(void);
};

class Platform_Control
{
public:
    Axis Axis_x{500, 1600, 0};
    Axis Axis_y{500, 1600, 0};
    Axis Axis_z{6, 100, 0};

    void to_abs_coord(void);
    void to_relative_coord(void);
    void move_abs(int x, int y, int z);
    void move_relative(int x, int y, int z);
    void to_home(void);
};



void to_home(void);
void move_z(int z_coord);


#endif
