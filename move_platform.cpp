#include "move_platform.h"

extern int GX_COORD, GY_COORD, GZ_COORD;
extern int GX_COORD_MAX, GY_COORD_MAX, GZ_COORD_MAX;
extern int GX_COORD_MIN, GY_COORD_MIN, GZ_COORD_MIN;

extern int G_SMALL_STEP, G_BIG_STEP;

extern QSerialPort serial;

void move_abs(int x_coord, int y_coord)
{
    if (x_coord > GX_COORD_MAX || x_coord < GX_COORD_MIN) return;
    if (y_coord > GY_COORD_MAX || y_coord < GY_COORD_MIN) return;

    GX_COORD = x_coord;
    GY_COORD = y_coord;

    to_abs_coord();

    QString cmd;

    cmd = QString("\nG0 X%1.%2 Y%3.%4\n").arg(x_coord/10).arg(x_coord%10).arg(y_coord/10).arg(y_coord/10);

    serial.write(cmd.toUtf8());

    return;
}

void move_relative(int x_coord, int y_coord)
{
    GX_COORD += x_coord;
    if (GX_COORD > GX_COORD_MAX || GX_COORD < GX_COORD_MIN)
    {
//        qDebug()<<"move over x axis: " + QString::number(GX_COORD) + " + " + QString::number(x_coord);
        GX_COORD -= x_coord;
        return;
    }

    GY_COORD += y_coord;
    if (GY_COORD > GY_COORD_MAX || GY_COORD < GY_COORD_MIN)
    {
//        qDebug()<<"move over y axis: " + QString::number(GY_COORD) + " + " + QString::number(y_coord);
        GY_COORD_MAX -= y_coord;
        return;
    }


    to_relative_coord();

    QString cmd = "\nG0 ";

    if (x_coord != 0)
    {
        cmd.append("X");
        if (x_coord < 0) cmd.append("-");
        x_coord = abs(x_coord);

        cmd.append(QString::number(x_coord/10));

        if (x_coord%10 != 0)
        {
            cmd.append(".");
            cmd.append(QString::number(x_coord%10));
        }

    }

    if (y_coord != 0)
    {
        cmd.append(" Y");
        if (y_coord < 0) cmd.append("-");
        y_coord = abs(y_coord);

        cmd.append(QString::number(y_coord/10));

        if (y_coord%10 != 0)
        {
            cmd.append(".");
            cmd.append(QString::number(y_coord%10));
        }
    }
    cmd.append("\n");

//    qDebug()<<cmd;

    serial.write(cmd.toUtf8());

    to_abs_coord();


    return;
}

void move_z(int z_coord)
{
    if (z_coord > GZ_COORD_MAX || z_coord < GZ_COORD_MIN)
    {
        return;
    }

    GZ_COORD = z_coord;

    to_abs_coord();

    QString cmd;

    cmd = QString("\nG0 Z%1\n").arg(z_coord);

    serial.write(cmd.toUtf8());

    return;
}

void to_abs_coord(void)
{
    QString str = "\nG90\n";
    serial.write(str.toUtf8());

    return;
}

void to_relative_coord(void)
{
    QString str = "\nG91\n";
    serial.write(str.toUtf8());

    return;
}


void to_home(void)
{
    QString str = "\nG28 X Y\n";
    serial.write(str.toUtf8());

    return;
}
