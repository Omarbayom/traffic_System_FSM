#ifndef SMARTTRAFFICSYSTEM_H
#define SMARTTRAFFICSYSTEM_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsRectItem>
#include <QPushButton>
#include <QGraphicsTextItem>
#include <QComboBox>

class SmartTrafficLight : public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    explicit SmartTrafficLight(QGraphicsItem *parent = nullptr);

    void setTrafficDensity(int density);
    void triggerEmergency();
    void triggerPedestrianCrossing();
    QString getLightState() const;
    bool canCross() const; // Check if pedestrian can cross
    bool isPedestrianButtonPressed() const; // Track if the button has been pressed already

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void trafficLightClicked(SmartTrafficLight* clickedLight); // Signal for clicked traffic light
    void pedestrianButtonEnabled(bool enabled); // Emit signal to enable/disable pedestrian button

private slots:
    void updateLight();
    void updateTimer();

private:
    enum class State { Red, Yellow, Green };
    State currentState;

    QTimer *timer;
    QTimer *stateChangeTimer;
    int stateDuration;
    int elapsedTime;
    int remainingTime;

    bool pedestrianButtonPressed;  // Track if pedestrian button has been pressed for the current state

    QGraphicsTextItem *timeDisplay;

    void setColor(const QColor &color);
    void updateTimeDisplay();
    void resetPedestrianButton();  // Reset the pedestrian button state when the traffic light changes
};

class SmartTrafficSystem : public QWidget {
    Q_OBJECT

public:
    explicit SmartTrafficSystem(QWidget *parent = nullptr);

private slots:
    //void simulateEmergency(SmartTrafficLight* selectedLight);
    void simulatePedestrianCrossing(SmartTrafficLight* selectedLight);
    void synchronizeTraffic();

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    QVector<SmartTrafficLight *> trafficLights;

    QPushButton *emergencyButton;
    QPushButton *pedestrianButton;
    QComboBox *trafficSelector;

    void createRoadsAndLights();
    void updatePedestrianButton(bool enabled); // Enable/Disable pedestrian button
};

#endif // SMARTTRAFFICSYSTEM_H
