# Projektbericht: Memory Cache

## 1. Aufgabenstellung

In diesem Projekt wurde ein Cache-Modul implementiert, das in verschiedenen Konfigurationen (1‑, 2‑ und 3‑Level) mit selbstbestimmten Cachegrößen, Latenzen und Mapping-Strategien betrieben werden kann. Das Modul soll Lese- und Schreibzugriffe vom Prozessor annehmen, die Daten in Cachezeilen zwischenlagern und im Fall von Cache Miss auf einen Hauptspeicher zugreifen.

## 2. Rechercheergebnisse

### Typische Größen, Cachezeilen und Latenzen
- **Cache-Größen:**  
  - L1-Caches: typischerweise 16–64 KB  
  - L2-Caches: ca. 256 KB bis 1 MB  
  - L3-Caches: mehrere Megabyte (z. B. 2–16 MB)  
- **Cachezeilengröße:**  
  Moderne Prozessoren verwenden oft 64-Byte-Zeilen, um die räumliche Lokalität optimal zu nutzen.  
- **Latenzen:**  
  - L1-Zugriffe: 1–2 Taktzyklen  
  - L2-Zugriffe: 3–10 Taktzyklen  
  - L3-Zugriffe: 10–20 Taktzyklen  
  - Hauptspeicherzugriffe: ca. 100–200 Zyklen  

### Replacement-Strategien
- **Direct Mapped:**  
  Jeder Block wird in genau einer Cachezeile abgelegt. Vorteile sind die einfache Implementierung und schnelle Berechnung des Indexes. Der Nachteil ist die hohe Wahrscheinlichkeit von Konflikten, wenn mehrere Blöcke auf dieselbe Zeile abgebildet werden.  
- **Fully Associative:**  
  Jede Adresse kann auf jede Cache-Zeile abgebildet werden. Konflikte werden minimiert, jedoch mit höherer Komplexität, da alle Zeilen durchsucht werden müssen.  
- **Set Associative:**  
  Jede Adresse kann auf einen Teil der Cache-Zeilen abgebildet werden. Diese Methode bietet einen Kompromiss zwischen Hardware-Komplexität und Miss-Rate.  

### Speicherintensiver Algorithmus
Ein speicherintensiver Algorithmus benötigt große Datenmengen und führt viele Lese- und Schreiboperationen durch. Seine Effizienz wird durch die langsamen Hauptspeicherzugriffe beeinträchtigt, da der Prozessor oft auf Daten warten muss. Cache-Speicher hilft, indem er häufig verwendete Daten (zeitlich oder räumlich) zwischenspeichert, wodurch Zugriffszeiten reduziert und die Leistung verbessert werden.  
Um dieses Verhalten zu untersuchen, verwenden wir die Matrixmultiplikation, da sie viele Speicherzugriffe erfordert und sich gut in einer CSV-Datei dokumentieren lässt – jeder Zugriff auf eine Matrixzelle kann genau protokolliert werden.

## 3. Lösungsansatz

### Simulation des Caches
- **Cache-Zugriffe** über CPU-Signale (Lese- und Schreibzugriffe) verarbeitet.  
- **Cache Levels** separat simuliert, wobei jeder Level konfigurierbare Parameter wie Anzahl der Zeilen und Zugriffsverzögerung besitzt.  
- **Mapping-Strategien:**  
  Je nach Konfiguration (Direct Mapped, Fully Associative oder Set-Associative) erfolgt die Zuordnung der Adressen zu den Cachezeilen unterschiedlich.  
- **Replacement-Strategie:**  
  Ein einfacher FIFO-Ansatz wird verwendet, wobei jeder Cachezeile ein Zähler zugeordnet ist, um den am längsten vorhandenen Eintrag zu bestimmen.  

### Metriken und Berechnung
Für jeden Zugriff werden die folgenden Kennzahlen ermittelt:
- **Cache Miss und Hit:**  
  Jeder Zugriff, bei dem der benötigte Datenblock im Cache vorhanden ist (unabhängig von Lese- oder Schreiboperation), wird als Hit gewertet. Andernfalls erfolgt ein Miss, der den Hauptspeicherzugriff mit definierter Verzögerung triggert.  
- **Cycles:**  
  Die Simulationsdauer wird in Taktzyklen gemessen. Hierbei fließen die Verzögerungen der einzelnen Cache-Level (über Schleifen mit `wait()` im jeweiligen Modul) sowie der Hauptspeicherzugriff ein.  
- **Primitive Gate Count:**  
  Eine grobe Schätzung der benötigten Hardware wird anhand der Anzahl der Cachezeilen und -level berechnet.  

### Rahmenprogramm
Das Rahmenprogramm übernimmt folgende Aufgaben:
- **Einlesen der CLI-Optionen:**  
  Mithilfe von `getopt_long` werden Optionen eingelesen. Werden keine Werte angegeben, kommen sinnvolle Standardwerte zum Einsatz. Zudem werden die Eingabewerte geprüft, ob sie für die Simulation gültig sind.  
- **Eingabedatei verarbeiten:**  
  Das Rahmenprogramm liest die angegebene CSV-Datei ein. Jede Zeile (ohne Header) wird in einen Request übersetzt. Ein Lesezugriff wird als `R,<Address>,` und ein Schreibzugriff als `W,<Address>,<Data>` erwartet. Die extrahierten Requests werden an das Cache-Modul übergeben.
- **Simulation starten:**  
  Nachdem alle Konfigurationsparameter und Requests eingelesen wurden, wird das Cache-Modul (in Verbindung mit dem Hauptspeicher) simuliert. Die erfassten Kennzahlen (Cycles, Hit-/Miss-Raten, Primitive Gate Count) werden anschließend ausgegeben.

## 4. Bewertung und Fazit

Die Simulation ermöglicht es, über die Konfiguration der Mapping-Strategie** mithilfe von benötigten Zyklen, resultierenden Miss- und Hit-Raten zu vergleichen. Anhand dieser Kennzahlen kann z. B. ermittelt werden, welche Mapping-Strategie in Bezug auf Effizienz überlegen ist.  
Gleichzeitig ermöglicht die Variation der Cache-Level und -Größen eine intuitive Simulation des Einflusses von Lese- und Schreibzugriffen auf den Gesamtsystemdurchsatz.  
Diese Erkenntnisse liefern wichtige Hinweise für das Design moderner Cache-Systeme, insbesondere im Kontext speicherintensiver Algorithmen, bei denen ein optimales Cache-Design zu einer erheblichen Leistungssteigerung führen kann.

## 5. Verteilung der Arbeit

### Yuchen Yang
- ... 

### Zixuan Ye
- ...  

### Ka Ki Chen
- ...  