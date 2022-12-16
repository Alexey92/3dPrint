#ifndef __MOVE_PLATFORM_H_
#define __MOVE_PLATFORM_H_

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QString>

class Axis
{
public:
    int current_value;
    int max_value;
    int min_value;

    Axis(int value, int max, int min)
    {
        current_value = value;
        max_value = max;
        min_value = min;
    }

    void set_value(int value)
    {
        if (value > max_value || value < min_value)
            return;
        else
            current_value = value;
    }

};

class Platform_Control
{
private:
    Axis Axis_x{10, 1600, 0};
//    int x_max = 1600;
//    int y_max = 1600;
//    int z_max = 50;


};

//int GX_COORD, GY_COORD, GZ_COORD;
//int GX_COORD_MAX = 1600, GY_COORD_MAX = 1600, GZ_COORD_MAX = 50;
//int GX_COORD_MIN = 0, GY_COORD_MIN = 0, GZ_COORD_MIN = 2;

//int G_SMALL_STEP = 1, G_BIG_STEP = 10;

//QSerialPort serial;

void to_abs_coord(void);
void to_relative_coord(void);
void to_home(void);
void move_relative(int x_coord, int y_coord);
void move_abs(int x_coord, int y_coord);
void move_z(int z_coord);


#endif
