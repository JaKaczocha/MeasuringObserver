#include "appcore.h"

AppCore::AppCore(QObject *parent)
    : QObject{parent}
{
    channel = 2569688;
    manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished, this, &AppCore::managerFinished);

    // Pobierz dane natychmiast po uruchomieniu
    onRequest();
}

void AppCore::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }
    answer = reply->readAll();
    qDebug() << answer;

    QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QJsonObject::iterator it;
        numdata.resize(0); // Resetowanie numdata przed dodaniem nowych danych
        timestamps.resize(0); // Resetowanie timestamps przed dodaniem nowych danych
        for (it = obj.begin(); it != obj.end(); ++it) {
            if (it.value().isArray()) {
                QJsonArray jArray = it.value().toArray();
                for (const auto &v : jArray) {
                    QVector<double> temp(1);
                    temp[0] = v.toObject().value("field1").toString().toDouble();

                    QString timestamp = v.toObject().value("created_at").toString();
                    timestamps.append(timestamp);

                    numdata.append(temp);
                }
            }
        }
    }

    // Wyświetl dane w konsoli
    for (int i = 0; i < numdata.size(); ++i) {
        qDebug() << "Timestamp:" << timestamps[i] << "Field1:" << numdata[i][0];
    }

    qDebug() << "Emitting newData signal";
    emit newData(numdata, timestamps);
}

void AppCore::onRequest()
{
    qDebug() << "Receive";
    datacount = 10; // Ustawienie liczby żądanych pomiarów na 10
    QString requeststring = "https://api.thingspeak.com/channels/" + QString::number(channel) + "/feeds.json?results=" + QString::number(datacount);
    request.setUrl(QUrl(requeststring));
    manager->get(request);
}

uint32_t AppCore::getChannel()
{
    return channel;
}
