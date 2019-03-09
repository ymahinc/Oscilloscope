#ifndef PORTTHREAD_H
#define PORTTHREAD_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#include "global.h"

typedef struct{
    int timeout;
    QQueue<Command> commands;
    int type;
}Transaction;

class PortThread : public QThread
{
    Q_OBJECT

public:
    explicit PortThread(QObject *parent = nullptr);
    ~PortThread();

    void transaction(int waitTimeout, const QQueue<Command> &commands, int type);
    void setPort(QString &portName);

signals:
    void result(bool success, const QString &error, int type);
    void newDataAvailable(QByteArray data, int canal);
    void newTrigStateAvailable(bool triggered);

private:
    void run() override;

    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit = false;
    QQueue<Transaction> m_transactionsQueue;
    QString m_portName;
    bool m_portHasChanged;
};

#endif // PORTTHREAD_H
