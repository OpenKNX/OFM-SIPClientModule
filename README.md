# OFM-SIPClientModule

Über diese Modul kann ein Anruf über SIP gestartet werden. 
Dabei wird das Telefon lediglich zum leuten gebracht, es wird keine Audio Verbindung hergestellt.
Somit können z.B. SIM-Karten Gargentoröffner betätigt werden die einen Anruf benötigen um das Tor zu öffnen.

## Abhängigkeiten

Das Modul setzt [OFM-Network](https://github.com/OpenKNX/OFM-Network) oder [OFM-WLAN](https://github.com/mgeramb/OFM-WLANModule) voraus.

## Features

- Je Kanal kann eine Rufnummer für den Anruf hinterlegt werden
- Ein KO pro Kanal um einen Anruf zu starten

## Hardware Unterstützung

|Prozessor | Status     | Anmerkung                  |
|----------|------------|----------------------------|
|ESP32     | Beta       |                            |

RP2040 wird von diesem Modul nicht unterstüzt.

Getestete Hardware:
- Adafruit ESP32 Feather V2

## Einbindung in die Anwendung

In das Anwendungs XML muss das OFM-SIPClientModule aufgenommen werden:

```xml
  <op:define prefix="SIP" ModuleType="22"
    share=   "../lib/OFM-SIPClientModule/src/SIPClientModule.share.xml"
    template="../lib/OFM-SIPClientModule/src/SIPClientModule.templ.xml"
    NumChannels="5"
    KoSingleOffset="985"
    KoOffset="980">
    <op:verify File="../lib/OFM-SIPClientModule/library.json" ModuleVersion="0" /> 
  </op:define>
```

**Hinweis:** Es wird ein Kanal für das Modul und je ein KO pro Kanal benötigt.

In main.cpp muss das SIPClientModule ebenfalls hinzugefügt werden:

```
[...]
#include "SIPClientModule.h"
[...]

void setup()
{
    [...]
    openknx.addModule(3, openknxSIPClientModule);
    [...]
}
```

## Bekannte Probleme

Das automatische beenden eines Anrufs funktioniert nur, solange die Gegenstelle den Anruf noch nicht entgegen genommen hat.

## Lizenz

[GNU GPL v3](LICENSE)