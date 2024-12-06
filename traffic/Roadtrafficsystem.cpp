#include "RoadTrafficSystem.h"
#include <QGraphicsLineItem>
#include <QVBoxLayout>

// TrafficLight Implementation
TrafficLight::TrafficLight(QGraphicsItem *parent)
    : QGraphicsRectItem(parent), currentState(State::Red), stateDuration(10), elapsedTime(0) {
    setRect(-10, -10, 20, 20); // Square traffic light
    setColor(Qt::red);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TrafficLight::updateLight);
    timer->start(1000); // Update every second
}

void TrafficLight::setColor(const QColor &color) {
    QBrush brush(color);
    setBrush(brush);
}

void TrafficLight::updateLight() {
    elapsedTime++;

    if (elapsedTime >= stateDuration) {
        elapsedTime = 0;

        switch (currentState) {
        case State::Red:
            currentState = State::Green;
            setColor(Qt::green);
            stateDuration = 10;
            break;
        case State::Green:
            currentState = State::Yellow;
            setColor(Qt::yellow);
            stateDuration = 3;
            break;
        case State::Yellow:
            currentState = State::Red;
            setColor(Qt::red);
            stateDuration = 10;
            break;
        }
    }
}

QString TrafficLight::getLightState() const {
    switch (currentState) {
    case State::Red: return "Red";
    case State::Yellow: return "Yellow";
    case State::Green: return "Green";
    }
    return "Unknown";
}

// RoadTrafficSystem Implementation
RoadTrafficSystem::RoadTrafficSystem(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    view = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    view->setScene(scene);

    layout->addWidget(view);

    createRoadsAndLights();
}

void RoadTrafficSystem::createRoadsAndLights() {
    // Set scene dimensions
    scene->setSceneRect(0, 0, 800, 800);

    // Draw horizontal roads
    for (int i = 200; i <= 600; i += 200) {
        QGraphicsLineItem *road = new QGraphicsLineItem(0, i, 800, i);
        road->setPen(QPen(Qt::gray, 40));
        scene->addItem(road);
    }

    // Draw vertical roads
    for (int i = 200; i <= 600; i += 200) {
        QGraphicsLineItem *road = new QGraphicsLineItem(i, 0, i, 800);
        road->setPen(QPen(Qt::gray, 40));
        scene->addItem(road);
    }

    // Place traffic lights at intersections
    for (int x = 200; x <= 600; x += 200) {
        for (int y = 200; y <= 600; y += 200) {
            TrafficLight *light = new TrafficLight();
            light->setPos(x, y);
            trafficLights.append(light);
            scene->addItem(light);
        }
    }
}
