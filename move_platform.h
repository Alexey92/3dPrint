#ifndef __MOVE_PLATFORM_H_
#define __MOVE_PLATFORM_H_

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QString>

#define X_MAX     1600
#define Y_MAX     1600
#define Z_MAX     100
#define X_MIN     0
#define Y_MIN     0
#define Z_MIN     0

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
    int step_small = 1;
    int step_big = 10;

    Axis Axis_x{500, X_MAX, X_MIN};
    Axis Axis_y{500, Y_MAX, Y_MIN};
    Axis Axis_z{6, Z_MAX, Z_MIN};

    void step_small_set(int value);
    void step_big_set(int value);
    int step_small_get(void);
    int step_big_get(void);


    void to_abs_coord(void);
    void to_relative_coord(void);
    void move_abs(int x, int y, int z);
    void move_relative(int x, int y, int z);
    void to_home(void);
};


#endif
