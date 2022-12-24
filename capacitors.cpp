#include "capacitors.h"

Capacitors::Capacitors(QObject *parent) : QObject(parent)
    {

    }



int Capacitors::parse_coords_from_string(QByteArray *input_array, QString *output_string)
    {
    QTextStream myteststream(input_array);
    int nmb_of_caps;

    myteststream >> nmb_of_caps;

    for (int i = 0; i < nmb_of_caps; i++)
        {
        myteststream >> cap_coord[i].x >> cap_coord[i].y;

        output_string->append("x = " + QString::number(cap_coord[i].x) + "; y = " + QString::number(cap_coord[i].y) + "\n");
        }

    focus_cap.x = cap_coord[0].x;
    focus_cap.y = cap_coord[0].y;

    return nmb_of_caps;
    }
