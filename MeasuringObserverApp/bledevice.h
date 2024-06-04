#ifndef BLEDEVICE_H
#define BLEDEVICE_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>

#if QT_CONFIG(permissions)
#include <QtCore/qcoreapplication.h>
#include <QtCore/qpermissions.h>
#endif

#include "deviceinfo.h"

#pragma pack (1)
typedef struct{
    const uint8_t flags;
    const int32_t temperature;
    const uint16_t year;
    const uint8_t month;
    const uint8_t day;
    const uint8_t hour;
    const uint8_t min;
    const uint8_t sec;
    const uint8_t place;
}HTS_t;
#pragma pack (0)

class BLEDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList deviceListModel READ deviceListModel WRITE setDeviceListModel RESET resetDeviceListModel NOTIFY deviceListModelChanged)

public:
    explicit BLEDevice(QObject *parent = nullptr);
    ~BLEDevice();

    QStringList deviceListModel();

private:
    DeviceInfo currentDevice;
    QBluetoothDeviceDiscoveryAgent *DiscoveryAgent;
    QList<QObject*> qlDevices;
    QLowEnergyController *controller;
    QLowEnergyService *service, *serviceBatt;
    QLowEnergyDescriptor notificationDesc;
    bool bFoundHTService;
    bool bFoundBattService;
    QStringList m_foundDevices;
    QStringList m_deviceListModel;

private slots:
    /* Slots for QBluetothDeviceDiscoveryAgent */
    void addDevice(const QBluetoothDeviceInfo &);
    void scanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);

    /* Slots for QLowEnergyController */
    void serviceDiscovered(const QBluetoothUuid &);
    void serviceScanDone();
    void controllerError(QLowEnergyController::Error);
    void deviceConnected();
    void deviceDisconnected();

    /* Slotes for QLowEnergyService */
    void serviceStateChanged(QLowEnergyService::ServiceState);
    void serviceBattStateChanged(QLowEnergyService::ServiceState);
    void updateData(const QLowEnergyCharacteristic &, const QByteArray &);
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &, const QByteArray &);

public slots:
    /* Slots for user */
    void startScan();
    void startConnect(int);
    void disconnectFromDevice();
    void writeData(QByteArray);
    void setDeviceListModel(QStringList);
    void resetDeviceListModel();

signals:
    /* Signals for user */
    void newTemperature(QList<QVariant>);
    void newIntermediateTemperature(QList<QVariant>);
    void scanningFinished();
    void connectionStart();
    void connectionEnd();
    void deviceListModelChanged(QStringList);
    void batteryLevel(quint8);

};

#endif // BLEDEVICE_H
