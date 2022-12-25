#include "move_platform.h"

// Конструктор для задания объекта одной оси
Axis::Axis(int value, int max, int min)
{
    current_value = value;
    max_value = max;
    min_value = min;
}

// Задание значения
bool Axis::set_value(int value)
{
    bool res = true;

    if (value > max_value || value < min_value)
        res = false;
    else
        current_value = value;

    return res;
}

// Прибавление к текущему значению
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

// Чтение значения
int Axis::get_value(void)
{
    return current_value;
}

// Задание значения
void Platform_Control::step_small_set(int value)
{
    step_small = value;
}

// Задание значения
void Platform_Control::step_big_set(int value)
{
    step_big = value;
}

// Чтение значения
int Platform_Control::step_small_get(void)
{
    return step_small;
}

// Чтение значения
int Platform_Control::step_big_get(void)
{
    return step_big;
}

// Перевод платформы в режим задания абсолютных координат
void Platform_Control::to_abs_coord(void)
{
    QString str = "\nG90\n";
    serial.write(str.toUtf8());

    return;
}

// Перевод платформы в режим задания относительных координат
void Platform_Control::to_relative_coord(void)
{
    QString str = "\nG91\n";
    serial.write(str.toUtf8());

    return;
}

// Перевод платформы по абсолютным координатам
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

// Перевод платформы по относительным координатам
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

// Перевод платформы "домой" по осям X и Y
void Platform_Control::to_home(void)
{
    QString str = "\nG28 X Y Z\n";
    serial.write(str.toUtf8());

    return;
}
