#include "bledevice.h"

BLEDevice::BLEDevice(QObject *parent) : QObject(parent),
    currentDevice(QBluetoothDeviceInfo()),
    controller(0),
    service(0),
    serviceBatt(0)
{
    DiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    DiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);

    connect(DiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BLEDevice::addDevice);
    connect(DiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this, &BLEDevice::deviceScanError);
    connect(DiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BLEDevice::scanFinished);
    connect(DiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BLEDevice::scanFinished);
}

BLEDevice::~BLEDevice()
{
    delete DiscoveryAgent;
    delete controller;
}

QStringList BLEDevice::deviceListModel()
{
    return m_deviceListModel;
}

void BLEDevice::setDeviceListModel(QStringList deviceListModel)
{
    if (m_deviceListModel == deviceListModel)
        return;

    m_deviceListModel = deviceListModel;
    emit deviceListModelChanged(m_deviceListModel);
}

void BLEDevice::resetDeviceListModel()
{
    m_deviceListModel.clear();
    emit deviceListModelChanged(m_deviceListModel);
}

void BLEDevice::addDevice(const QBluetoothDeviceInfo &device)
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        qDebug()<<"Discovered Device:"<<device.name()<<"Address: "<<device.address().toString()<<"RSSI:"<< device.rssi()<<"dBm";

        if(!m_foundDevices.contains(device.name(), Qt::CaseSensitive) && device.name().contains("HTS", Qt::CaseSensitive)) {
            m_foundDevices.append(device.name());

            DeviceInfo *dev = new DeviceInfo(device);
            qlDevices.append(dev);
        }
    }
}

void BLEDevice::scanFinished()
{
    setDeviceListModel(m_foundDevices);
    emit scanningFinished();
}

void BLEDevice::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        qDebug() << "The Bluetooth adaptor is powered off.";
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        qDebug() << "Writing or reading from the device resulted in an error.";
    else
        qDebug() << "An unknown error has occurred.";
}

void BLEDevice::startScan()
{

#if QT_CONFIG(permissions)
    //! [permissions]
    QBluetoothPermission permission{};
    permission.setCommunicationModes(QBluetoothPermission::Access);
    switch (qApp->checkPermission(permission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(permission, this, &BLEDevice::startScan);
        return;
    case Qt::PermissionStatus::Denied:
        qDebug()<< "Bluetooth permissions not granted!" ;
        return;
    case Qt::PermissionStatus::Granted:
        break; // proceed to search
    }
    //! [permissions]
#endif // QT_CONFIG(permissions)

    qDeleteAll(qlDevices);
    qlDevices.clear();
    m_foundDevices.clear();
    resetDeviceListModel();
    DiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    qDebug()<< "Searching for BLE devices..." ;
}

void BLEDevice::startConnect(int i)
{
    currentDevice.setDevice(((DeviceInfo*)qlDevices.at(i))->getDevice());
    if (controller) {
        controller->disconnectFromDevice();
        delete controller;
        controller = 0;

    }

    controller = QLowEnergyController::createCentral(currentDevice.getDevice());
    controller ->setRemoteAddressType(QLowEnergyController::RandomAddress);

    connect(controller, &QLowEnergyController::serviceDiscovered, this, &BLEDevice::serviceDiscovered);
    connect(controller, &QLowEnergyController::discoveryFinished, this, &BLEDevice::serviceScanDone);
    connect(controller, &QLowEnergyController::errorOccurred,  this, &BLEDevice::controllerError);
    connect(controller, &QLowEnergyController::connected, this, &BLEDevice::deviceConnected);
    connect(controller, &QLowEnergyController::disconnected, this, &BLEDevice::deviceDisconnected);

    controller->connectToDevice();
}

void BLEDevice::disconnectFromDevice()
{

    if (controller->state() != QLowEnergyController::UnconnectedState) {
        controller->disconnectFromDevice();
    } else {
        deviceDisconnected();
    }
}

void BLEDevice::serviceDiscovered(const QBluetoothUuid &gatt)
{
    if(gatt==QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::HealthThermometer)) {
        bFoundHTService =true;
        qDebug() << "HTS service found";
    }

    if(gatt==QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::BatteryService)) {
        bFoundBattService =true;
        qDebug() << "BATT service found";
    }
}

void BLEDevice::serviceScanDone()
{
    delete service;
    service=0;

    delete serviceBatt;
    serviceBatt=0;

    if(bFoundHTService) {
        qDebug() << "Connecting to HTS service...";
        service = controller->createServiceObject(QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::HealthThermometer),this);
    }

    if(!service) {
        qDebug() <<"HTS service not found";
        disconnectFromDevice();
        return;
    }

    connect(service, &QLowEnergyService::stateChanged,this, &BLEDevice::serviceStateChanged);
    connect(service, &QLowEnergyService::characteristicChanged,this, &BLEDevice::updateData);
    connect(service, &QLowEnergyService::descriptorWritten,this, &BLEDevice::confirmedDescriptorWrite);
    service->discoverDetails();

    if(bFoundBattService) {
        qDebug() << "Connecting to Battery service...";
        serviceBatt = controller->createServiceObject(QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::BatteryService),this);
    }

    if(!serviceBatt) {
        qDebug() <<"Batt service not found";
        return;
    }

    connect(serviceBatt, &QLowEnergyService::stateChanged,this, &BLEDevice::serviceBattStateChanged);
    connect(serviceBatt, &QLowEnergyService::characteristicChanged,this, &BLEDevice::updateData);
    connect(serviceBatt, &QLowEnergyService::descriptorWritten,this, &BLEDevice::confirmedDescriptorWrite);
    serviceBatt->discoverDetails();
}

void BLEDevice::deviceDisconnected()
{
    qDebug() << "Remote device disconnected";
    emit connectionEnd();
}

void BLEDevice::deviceConnected()
{
    qDebug() << "Device connected";
    controller->discoverServices();
}

void BLEDevice::controllerError(QLowEnergyController::Error error)
{
    qDebug() << "Controller Error:" << error;
}

void BLEDevice::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::RemoteServiceDiscovered:
    {

        //TempMeas characteristic
        const QLowEnergyCharacteristic  TempMeasChar = service->characteristic(QBluetoothUuid(QBluetoothUuid::CharacteristicType::TemperatureMeasurement));
        if (!TempMeasChar.isValid()) {
            qDebug() << "Temp Measurement characteristic not found";
            break;
        }
        // TempMeas indicate enabled
        const QLowEnergyDescriptor m_notificationDescTM = TempMeasChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (m_notificationDescTM.isValid()) {
            // Enable indication
            service->writeDescriptor(m_notificationDescTM, QByteArray::fromHex("0200")); // enable indication
            qDebug() << "TM indication enabled";
            //emit connectionStart();
        }

        //TempMeas characteristic
        const QLowEnergyCharacteristic  IntermediateTempChar = service->characteristic(QBluetoothUuid(QBluetoothUuid::CharacteristicType::IntermediateTemperature));
        if (!IntermediateTempChar.isValid()) {
            qDebug() << "Intermediate Temp characteristic not found";
            break;
        }
        // TempMeas notificate enabled
        const QLowEnergyDescriptor m_notificationDescIT = IntermediateTempChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (m_notificationDescIT.isValid()) {
            // Enable notification
            service->writeDescriptor(m_notificationDescIT, QByteArray::fromHex("0100")); // enable notification
            qDebug() << "IT notification enabled";
            emit connectionStart();
        }
        break;
    }
    default:

        break;
    }
}

void BLEDevice::serviceBattStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::RemoteServiceDiscovered:
    {
        const QLowEnergyCharacteristic  BattChar = serviceBatt->characteristic(QBluetoothUuid(QBluetoothUuid::CharacteristicType::BatteryLevel));
        if (!BattChar.isValid()) {
            qDebug() << "Batt characteristic not found";
            break;
        }

        // Enable notification
        const QLowEnergyDescriptor m_notificationDesc = BattChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (m_notificationDesc.isValid()) {
            serviceBatt->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100")); // enable notification
        }

        break;
    }
    default:

        break;
    }
}

void BLEDevice::confirmedDescriptorWrite(const QLowEnergyDescriptor &d, const QByteArray &value)
{
    if (d.isValid() && d == notificationDesc && value == QByteArray("0000")) {
        controller->disconnectFromDevice();
        delete service;
        service = nullptr;
    }
}

void BLEDevice::writeData(QByteArray v)
{
    const QLowEnergyCharacteristic  TempMeasChar = service->characteristic(QBluetoothUuid(QBluetoothUuid::CharacteristicType::TemperatureMeasurement));
    service->writeCharacteristic(TempMeasChar, v, QLowEnergyService::WriteWithoutResponse);
}

void BLEDevice::updateData(const QLowEnergyCharacteristic &c, const QByteArray &v)
{
    if (c.uuid() == QBluetoothUuid(QBluetoothUuid::CharacteristicType::TemperatureMeasurement)) {

        const HTS_t *hts = reinterpret_cast<const HTS_t*>(v.constData());
        // flags (0) - 0-C/1-F
        // flags (1) - TimeStamp present
        // flags (2) - TempType present
        // flags (7) - over 38 C

        // Encode ieee11073_32BitFloat_t
        int exponent = hts->temperature >> 24;
        int mantissa = hts->temperature & 0x00FFFFFF;
        qreal tempValue = (qreal)mantissa * pow(10, exponent);

        QString dateTime = QString().asprintf("%04d-%02d-%02d  %02d:%02d:%02d", hts->year, hts->month, hts->day, hts->hour, hts->min, hts->sec);
        QList<QVariant> data;
        data.clear();
        data.append(tempValue);
        data.append(dateTime);

        emit newTemperature(data);

        qDebug()<<"Temp meas";

    }

    if (c.uuid() == QBluetoothUuid(QBluetoothUuid::CharacteristicType::IntermediateTemperature)) {

        const HTS_t *hts = reinterpret_cast<const HTS_t*>(v.constData());
        // flags (0) - 0-C/1-F
        // flags (1) - TimeStamp present
        // flags (2) - TempType present
        // flags (7) - over 38 C

        // Encode ieee11073_32BitFloat_t
        int exponent = hts->temperature >> 24;
        int mantissa = hts->temperature & 0x00FFFFFF;
        qreal tempValue = (qreal)mantissa * pow(10, exponent);

        QString dateTime = QString().asprintf("%04d-%02d-%02d  %02d:%02d:%02d", hts->year, hts->month, hts->day, hts->hour, hts->min, hts->sec);
        QList<QVariant> data;
        data.clear();
        data.append(tempValue);
        data.append(dateTime);

        emit newIntermediateTemperature(data);

        qDebug()<<"Intermediate";

    }

    if (c.uuid() == QBluetoothUuid(QBluetoothUuid::CharacteristicType::BatteryLevel)) {

        quint8 batt=0;
        bool ok;
        batt = v.toHex().toInt(&ok, 16);

        if(batt>100)
            batt=100;

        emit batteryLevel(batt);
    }
}

