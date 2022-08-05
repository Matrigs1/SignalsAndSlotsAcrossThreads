#include <QCoreApplication>
#include "consumer.h"
#include "producer.h"

void cleanup()
{
    qInfo() << "Cleanup";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QThread::currentThread()->setObjectName("Main Thread");

    //criando novo thread para producer
    QThread pthread;
    pthread.setObjectName("Producer Thread");
    //ptr de producer
    Producer *p = new Producer();
    //move o obj para o thread de producer
    p->moveToThread(&pthread);
    //quando o thread estiver pronto para startar chama o slot start de producer
    QObject::connect(&pthread, &QThread::started, p, &Producer::start);

    //criando novo thread para consumer
    QThread cthread;
    cthread.setObjectName("Consumer Thread");
    //ptr de consumer
    Consumer *c = new Consumer();
    //move o obj para o thread de consumer
    p->moveToThread(&cthread);

    //quando o producer chamar a produce, o consumer chama a consume
    QObject::connect(p, &Producer::produce, c, &Consumer::consume);
    //quando o producer terminar, o thread de producer é parado.
    QObject::connect(p, &Producer::finished, &pthread, &QThread::quit);
    //quando o thread producer terminar, o thread de consumer é parado.
    QObject::connect(&pthread, &QThread::finished, &cthread, &QThread::quit);
    //quando o thread consumer terminar, o obj de consumer é agendado para deleção.
    QObject::connect(&cthread, &QThread::finished, c, &Consumer::deleteLater);
    //quando o thread producer terminar, o obj de producer é agendado para deleção.
    QObject::connect(&pthread, &QThread::finished, p, &Producer::deleteLater);
    //quando o thread consumer terminar, a aplicação será parada.
    QObject::connect(&cthread, &QThread::finished, &a, &QCoreApplication::quit);
    //quando a aplicação estiver prestes a ser parada, a func cleanup será chamada.
    QObject::connect(&a, &QCoreApplication::aboutToQuit, &cleanup);

    //starta os threads
    cthread.start();
    pthread.start();

    return a.exec();
}
