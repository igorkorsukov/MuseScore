#ifndef SAMPLEOBJECT_H
#define SAMPLEOBJECT_H

#include <QObject>

namespace msf {

class SampleObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    Q_ENUMS(State)

public:
    explicit SampleObject(QObject *parent = nullptr);

    enum State {
        First = 0,
        Second,
        Third
    };

    State state() const;

    Q_INVOKABLE void next();

signals:
    void stateChanged(State state);

private:

    State _state = First;
};

}

#endif // SAMPLEOBJECT_H
