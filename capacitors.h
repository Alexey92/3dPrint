#ifndef CAPACITORS_H
#define CAPACITORS_H

#include <QObject>
#include "ui_mainwindow.h"
#include "QUdpSocket"

struct s_coord
{
    int x;
    int y;
};

class Capacitors : public QObject
    {
    Q_OBJECT
    public:
    s_coord cap_coord[50];
    s_coord to_center;
    s_coord focus_cap;

    int parse_coords_from_string(QByteArray *input_array, QString *output_string);


    explicit Capacitors(QObject *parent = nullptr);

    signals:

    };

#endif // CAPACITORS_H
