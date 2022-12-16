#include "move_platform.h"


//extern int G_SMALL_STEP, G_BIG_STEP;


Axis::Axis(int value, int max, int min)
{
    current_value = value;
    max_value = max;
    min_value = min;
}

bool Axis::set_value(int value)
{
    bool res = true;

    if (value > max_value || value < min_value)
        res = false;
    else
        current_value = value;

    return res;
}

bool Axis::add_value(int value)
{
    bool res = true;
    int old_value = current_value;
    int new_value = current_value + value;

    if (new_value > max_value || new_value < min_value)
    {
        res = false;
        current_value = old_value;
//        qDebug()<<"move over x axis: " + QString::number(GX_COORD) + " + " + QString::number(x_coord);
    }
    else
        current_value = new_value;

    return res;
}

int Axis::get_value(void)
{
    return current_value;
}

void Platform_Control::to_abs_coord(void)
{
    QString str = "\nG90\n";
    serial.write(str.toUtf8());

    return;
}

void Platform_Control::move_abs(int x, int y, int z)
{
    if (Axis_x.set_value(x) == false) return;
    if (Axis_y.set_value(y) == false) return;
    if (Axis_z.set_value(z) == false) return;

    to_abs_coord();

    QString cmd;

    cmd = QString("\nG0 X%1.%2 Y%3.%4 Z%5\n").arg(x/10).arg(x%10).arg(y/10).arg(y%10).arg(z);

    serial.write(cmd.toUtf8());

    return;
}

void Platform_Control::move_relative(int x, int y, int z)
{
    if (Axis_x.add_value(x) == false) return;
    if (Axis_y.add_value(y) == false) return;
    if (Axis_z.add_value(z) == false) return;

    to_relative_coord();

    QString cmd = "\nG0 ";

    if (x != 0)
    {
        cmd.append("X");
        if (x < 0) cmd.append("-");
        x = abs(x);

        cmd.append(QString::number(x/10));

        if (x%10 != 0)
        {
            cmd.append(".");
            cmd.append(QString::number(x%10));
        }

    }

    if (y != 0)
    {
        cmd.append(" Y");
        if (y < 0) cmd.append("-");
        y = abs(y);

        cmd.append(QString::number(y/10));

        if (y%10 != 0)
        {
            cmd.append(".");
            cmd.append(QString::number(y%10));
        }
    }

    if (z != 0)
    {
        cmd.append(" Z");
        cmd.append(QString::number(z));
    }

    cmd.append("\n");

//    qDebug()<<cmd;

    serial.write(cmd.toUtf8());

    to_abs_coord();


    return;
}


void Platform_Control::to_relative_coord(void)
{
    QString str = "\nG91\n";
    serial.write(str.toUtf8());

    return;
}


void Platform_Control::to_home(void)
{
    QString str = "\nG28 X Y\n";
    serial.write(str.toUtf8());

    return;
}
