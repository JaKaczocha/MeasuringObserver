#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHttpPart>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QTimer>
#include <QtDebug>
class AppCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint32_t channel READ getChannel WRITE setChannel NOTIFY channelChanged)


public:
    explicit AppCore(QObject *parent = nullptr);


private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QString answer;
    uint32_t datacount=10;
    uint32_t channel=0;
    QVector<QVector<double>> numdata;
private slots:
    void managerFinished(QNetworkReply *reply);
    void onRequest();

    double getChannel() const
    {
        return channel;
    }

    void setChannel(double _channel)
    {
        channel = _channel;
        emit channelChanged();
    }

signals:
    void channelChanged();
    void newData(QVector<QVector<double>>);

};
#endif // APPCORE_H
