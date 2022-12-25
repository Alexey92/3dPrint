#ifndef CAPACITORS_H
#define CAPACITORS_H

#include <QObject>
#include "ui_mainwindow.h"
#include "QUdpSocket"

#define CENTER_X 320
#define CENTER_Y 240

// Состояния конечного автомата
enum CAP_STATE { S_FINDING, S_CENTRING, S_CHECKING };

struct s_coord
{
    int x;
    int y;
};

class Capacitors : public QObject
    {
    Q_OBJECT
    public:
    s_coord cap_coord[50];      // Координаты всех найденных конденсаторов
    s_coord to_center[50];      // Расстояние каждого конденсатора до центра
    s_coord cap_focus;          // Координаты выбранного конденсатора
    s_coord move;               // Необходимое движение платформы для перемещения выбранного конденсатора в центр объектива

    int state = S_FINDING;      // Конечный автомат управления платформой
    int caps_nmb;               // Количество обнаруженных на изображении конденсаторов
    int i_cap_focus;            // Номер конденсатора, который ближе всех к центру объектива

    bool cap_focus_update(void);
    // Изменение состояния конечного автомата
    void state_change(int new_state);
    // Анализ полученной строки с координатами конденсаторов
    void parse_coords_from_string(QByteArray *input_array, QString *output_string);

    explicit Capacitors(QObject *parent = nullptr);

    signals:

    };

#endif // CAPACITORS_H
