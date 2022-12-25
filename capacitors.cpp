#include "capacitors.h"

Capacitors::Capacitors(QObject *parent) : QObject(parent)
    {

    }

void Capacitors::state_change(int new_state)
    {
    state = new_state;
    }

bool Capacitors::cap_focus_update(void)
    {
    bool ret = false;
    static int zero_values_nmb;
    s_coord update;

    if (zero_values_nmb > 15)
        {
        cap_focus.x = 0;
        cap_focus.y = 0;
        zero_values_nmb = 0;

        qDebug()<<"COORDS: x: " + QString::number(cap_focus.x) + "; y: " + QString::number(cap_focus.y);
        }

    if (cap_coord[i_cap_focus].x == 0)
        {
        zero_values_nmb++;
        return ret;
        }

    zero_values_nmb = 0;

    update.x = abs(cap_focus.x - cap_coord[i_cap_focus].x);
    update.y = abs(cap_focus.y - cap_coord[i_cap_focus].y);

    if ((update.x + update.y) > 7)
        {
        cap_focus.x = cap_coord[i_cap_focus].x;
        cap_focus.y = cap_coord[i_cap_focus].y;

        ret = true;

        qDebug()<<"COORDS: x: " + QString::number(cap_focus.x) + "; y: " + QString::number(cap_focus.y);
        }

    return ret;
    }

void Capacitors::parse_coords_from_string(QByteArray *input_array, QString *output_string)
    {
    QTextStream myteststream(input_array);

    myteststream >> caps_nmb;

    for (int i = 0; i < caps_nmb; i++)
        {
        myteststream >> cap_coord[i].x >> cap_coord[i].y;

        to_center[i].x = cap_coord[i].x - CENTER_X;
        to_center[i].y = cap_coord[i].y - CENTER_Y;

        output_string->append("x = " + QString::number(cap_coord[i].x) + "; y = " + QString::number(cap_coord[i].y) + "\n");
        }


    int cur_cap_distance = CENTER_X + CENTER_Y;

    for (int i = 0; i < caps_nmb; i++)
        {
        if (cur_cap_distance > to_center[i].x + to_center[i].y)
            {
            cur_cap_distance = to_center[i].x + to_center[i].y;
            i_cap_focus = i;
            }
        }

    if (caps_nmb == 0)
        {
        i_cap_focus = 0;
        cap_coord[i_cap_focus].x = 0;
        cap_coord[i_cap_focus].y = 0;
        }


    if (state == S_CENTRING)
        {
        if (cap_focus_update() && caps_nmb > 0)
            {
            if (to_center[i_cap_focus].x > 25) move.x = 2;
            else if (to_center[i_cap_focus].x > 5) move.x = 1;
            else if (to_center[i_cap_focus].x < -25) move.x = -2;
            else if (to_center[i_cap_focus].x < -5) move.x = -1;
            else move.x = 0;

            if (to_center[i_cap_focus].y > 25) move.y = 2;
            else if (to_center[i_cap_focus].y > 5) move.y = 1;
            else if (to_center[i_cap_focus].y < -25) move.y = -2;
            else if (to_center[i_cap_focus].y < -5) move.y = -1;
            else move.y = 0;

//            qDebug()<<"COORDS: x: " + QString::number(cap_coord[i_cap_focus].x) + "; y: " + QString::number(cap_coord[i_cap_focus].y);
//            qDebug()<<"DELTA:  x: " + QString::number(to_center[i_cap_focus].x) + "; y: " + QString::number(to_center[i_cap_focus].y);
            }
        else
            {
            move.x = 0;
            move.y = 0;
            }
        }
    }
