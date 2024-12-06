#ifndef ROADTRAFFICSYSTEM_H
#define ROADTRAFFICSYSTEM_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsRectItem>
#include <QLabel>

class TrafficLight : public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    explicit TrafficLight(QGraphicsItem *parent = nullptr);

    void setLightState(const QString &state);
    QString getLightState() const;

private slots:
    void updateLight();

private:
    enum class State { Red, Yellow, Green };
    State currentState;

    QTimer *timer;
    QLabel *stateLabel;
    int stateDuration;
    int elapsedTime;

    void setColor(const QColor &color);
};

class RoadTrafficSystem : public QWidget {
    Q_OBJECT

public:
    explicit RoadTrafficSystem(QWidget *parent = nullptr);

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    QVector<TrafficLight *> trafficLights;

    void createRoadsAndLights();
};

#endif // ROADTRAFFICSYSTEM_H
