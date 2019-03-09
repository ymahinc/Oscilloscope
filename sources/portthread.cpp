#include "portthread.h"

#include <QSerialPort>
#include <QTime>
#include <QDebug>

PortThread::PortThread(QObject *parent)
    : QThread(parent){
}

PortThread::~PortThread(){
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}

void PortThread::setPort(QString &portName){
    const QMutexLocker locker(&m_mutex);
    if ( m_portName != portName )
        m_portHasChanged = true;
    else
        m_portHasChanged = false;
    m_portName = portName;
}

void PortThread::transaction(int waitTimeout, const QQueue<Command> &commands, int type){
    const QMutexLocker locker(&m_mutex);
    Transaction trans;
    trans.timeout = waitTimeout;
    trans.commands = commands;
    trans.type = type;

    m_transactionsQueue.enqueue(trans);

    if (!isRunning())
        start();
    else
        m_cond.wakeOne();
}

void PortThread::run(){

    m_mutex.lock();
    Transaction currentTransaction = m_transactionsQueue.head();
    m_mutex.unlock();

    QSerialPort serial;

    if (m_portName.isEmpty()) {
        emit result(false,tr("No port name specified"),0);
        return;
    }

    while (!m_quit) {
        while ( m_transactionsQueue.count() ) {

            m_mutex.lock();
            currentTransaction = m_transactionsQueue.dequeue();
            m_mutex.unlock();

            if (m_portHasChanged) {
                serial.close();
                serial.setPortName(m_portName);
                serial.setBaudRate(2000000);
                serial.setReadBufferSize(2048);

                if (!serial.open(QIODevice::ReadWrite)) {
                    emit result(false, tr("Can't open %1, error code %2").arg(m_portHasChanged)
                                .arg(serial.error()),0);
                    return;
                }
            }

            while( currentTransaction.commands.count() ){
                Command cmd = currentTransaction.commands.dequeue();

                const QByteArray requestData = cmd.command.toUtf8();
                serial.write(requestData);
                if (serial.waitForBytesWritten(currentTransaction.timeout)) {
                    if (serial.waitForReadyRead(currentTransaction.timeout)) {
                        QByteArray responseData = serial.readAll();
                        while (serial.waitForReadyRead(20))
                            responseData += serial.readAll();

                        const QString response = QString::fromUtf8(responseData);

                        if ( cmd.expectedReplies.contains(response) ){
                            if ( currentTransaction.commands.isEmpty() ){
                                emit result(true, response,currentTransaction.type);
                                //qDebug() << response;
                            }
                        }else{
                            emit result(false,tr("Bad reply: %1").arg(response),currentTransaction.type);
                            break;
                        }

                    } else {
                        emit result(false, tr("Wait read response timeout %1")
                                    .arg(QTime::currentTime().toString()),currentTransaction.type);
                        break;
                    }

                } else {
                    emit result(false, tr("Wait write request timeout %1")
                                .arg(QTime::currentTime().toString()),currentTransaction.type);
                    break;
                }
            }
        }

        if ( ! currentTransaction.commands.count() ){
            serial.write(QString("GETTRIG\n").toUtf8());
            if (serial.waitForBytesWritten(currentTransaction.timeout)) {
                if (serial.waitForReadyRead(currentTransaction.timeout)) {
                    QByteArray responseData = serial.readLine();
                    emit newTrigStateAvailable((bool)responseData.at(0));
                }
            }

            serial.write(QString("GETDATA1\n").toUtf8());
            if (serial.waitForBytesWritten(currentTransaction.timeout)) {
                if (serial.waitForReadyRead(currentTransaction.timeout)) {
                    QByteArray responseData = serial.readAll();
                        while (serial.waitForReadyRead(15) )
                            responseData += serial.readAll();
                        //qDebug() << responseData;
                    if ( responseData.size() == 1024 )
                        emit newDataAvailable(responseData,1);
                }
            }

            serial.write(QString("GETDATA2\n").toUtf8());
            if (serial.waitForBytesWritten(currentTransaction.timeout)) {
                if (serial.waitForReadyRead(currentTransaction.timeout)) {
                    QByteArray responseData = serial.readAll();
                    while (serial.waitForReadyRead(15))
                        responseData += serial.readAll();
                    if ( responseData.size() == 1024 )
                        emit newDataAvailable(responseData,2);
                }
            }
        }

        m_mutex.lock();
        //m_cond.wait(&m_mutex);

        if ( m_transactionsQueue.count() )
            currentTransaction = m_transactionsQueue.head();

        m_mutex.unlock();
    }
}
