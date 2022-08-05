#include "producer.h"

Producer::Producer(QObject *parent)
    : QObject{parent}
{
    m_count = 0;
    m_max = 5;
    qInfo() << "Constructed" << this << "on" << QThread::currentThread();
}

Producer::~Producer()
{
    qInfo() << "Deconstructed" << this << "on" << QThread::currentThread();
}

void Producer::start()
{
    qInfo() << "Starting on" << QThread::currentThread();

    //Parent - child relationship | ptr de producer mandado como arg
    m_timer = new QTimer(this);
    //timeout de QTimer vai emitir para o slot de Producer.
    connect(m_timer, &QTimer::timeout, this, &Producer::timeout);
    //quando o producer parar, o timer para
    connect(this, &Producer::finished, m_timer, &QTimer::stop);
    //setando e começando o timer
    m_timer->setInterval(500);
    m_timer->start();
}

//a cada timeout de QTimer (500ms), essa func será chamada
void Producer::timeout()
{

    int value = QRandomGenerator::global()->bounded(1,100);
    qInfo() << "Produced" << value << "on" << QThread::currentThread();
    //emite o signal para produce com o valor
    emit produce(value);

    //para quando o count chegar em 5
    m_count++;
    if(m_count >= m_max)
    {
        qInfo() << "Finished" << value << "on" << QThread::currentThread();
        //emite o signal de finished - conectado a app
        emit finished();
    }
}
