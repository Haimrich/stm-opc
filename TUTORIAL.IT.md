# OPC UA Client on STM32
Tutorial Completo per ***STM32F769I-DISCO***

## Indice
- [Preparazione](#Preparazione)
- [Creazione del progetto](#segnalazione-bug-e-richieste-di-aiuto)
- [Creazione interfaccia grafica](#Creazione-interfaccia-grafica)
- [Importare progetto su STM32CubeIDE](#Importare-progetto-su-STM32CubeIDE)
- [Preparare la board per OPC UA](#Preparare-la-board-per-OPC-UA)
  - [Interfaccia Ethernet](#Interfaccia-Ethernet)
  - [lwIP](#lwIP)
  - [FreeRTOS](#FreeRTOS)
  - [Ethernet DMA Buffers](#Ethernet-DMA-Buffers)
- [Aggiungere la libreria open62541](#Aggiungere-la-libreria-open62541)
  - [Generazione dei file .c e .h](#Generazione-dei-file-.c-e-.h)
  - [Modifica dei file](#Modifica-dei-file)
  - [Aggiunta dei file al progetto](#Aggiunta-dei-file-al-progetto)
- [(Opzionale) Printf Redirect](#opzionale-printf-redirect)
- [Implementazione del Client OPC UA](#Implementazione-del-Client-OPC-UA)
  - [opcua.h](#opcua.h)
  - [opcua.c](#opcua.c)
- [Interazione dell’interfaccia grafica con il client OPC UA](#Interazione-dell’interfaccia-grafica-con-il-client-OPC-UA)
  - [Model.cpp](#Model.cpp)
  - [ModelListener.hpp](#ModelListener.hpp)
  - [MainScreenPresenter.cpp](#MainScreenPresenter.cpp)
  - [MainScreenPresenter.hpp](#MainScreenPresenter.hpp)
  - [MainScreenView.cpp](#MainScreenView.cpp)
  - [MainScreenView.hpp](#MainScreenView.hpp)
- [OPC UA Server](#OPC-UA-Server)
- [Esecuzione](#Esecuzione)

## Preparazione

Per prima cosa, è necessario installare gli strumenti di sviluppo forniti da ST Microelectronics.

- **STM32CubeIDE**: [https://www.st.com/en/development-tools/stm32cubeide.html](https://www.st.com/en/development-tools/stm32cubeide.html#get-software)
- **STM32CubeMX**: [https://www.st.com/en/development-tools/stm32cubemx.html](https://www.st.com/en/development-tools/stm32cubemx.html)
- **TouchGFX:** [https://www.st.com/en/development-tools/touchgfxdesigner.html](https://www.st.com/en/development-tools/touchgfxdesigner.html)

In particolare, per quest’ultimo si consiglia di seguire tutti i passaggi riportati nella documentazione: alla seguente pagina: [https://support.touchgfx.com/4.19/docs/introduction/installation](https://support.touchgfx.com/4.19/docs/introduction/installation).

> ℹ️ Questa guida è stata scritta con le seguenti versioni dei suddetti software:
> | Software     | Version |
> | ---      | ---       |
> | STM32CubeIDE | 1.9.0         |
> | STM32CubeMX     | 6.5.0        |
> | TouchGFX | 4.19.1

## Creazione del progetto

La creazione del progetto avviene attraverso TouchGFX Designer, che permette di generare il codice di partenza per la board. Per fare ciò, **aprire TouchGFX Designer**, nel menù a sinistra ci sono le voci *Examples* e *Demos* che contengono vari esempi funzionanti per la gestione dell’interfaccia grafica. Noi procederemo però con un progetto vuoto, quindi andiamo in ***Create***. Adesso è importante selezionare il modello della nostra board, nel caso di questa guida **STM32F769I Discovery Kit.** Una volta selezionato il modello, sulla parte destra della schermata si può decidere l’***Application Name*** e la directory in cui salvare il progetto. Infine cliccare su ***Create***

![Screen Creazione Progetto](https://user-images.githubusercontent.com/7826610/174487425-7a8c2188-d495-4166-a908-6da6f371710f.png)

## Creazione interfaccia grafica

Una volta creato il progetto, verrà visualizzata l’anteprima dell’interfaccia grafica della board che risulterà vuota. Possiamo procedere ad inserire i vari componenti che ci interessano. Per esempio, inseriamo una *Box* per colorare lo sfondo, scegliendola dal menu in alto. Sulla destra potremo modificarne il colore e il nome.

Poi inseriamo un *Button* con o senza label e un *Digital Clock*, modificandone l’aspetto e assegnando un nome significativo a ciascun elemento. Inseriamo anche un *Gauge* che si trova nella voce *Miscellaneous* del menù in alto.

Infine modifichiamo il nome della schermata da *Screen1* a *MainScreen*, cliccando sull’elemento del menù a sinistra e modificando il nome a destra. Il risultato finale sarà simile a questo:

![Screen Creazione interfaccia grafica 1](https://user-images.githubusercontent.com/7826610/174486706-3d63a53a-4023-4dbd-ae8a-50dbab944417.png)


Prima di passare al codice dobbiamo associare al pulsante azzurro, una funzione da implementare poi nel codice. Per farlo, selezioniamo il *Button* e clicchiamo su **Interactions** in alto a destra. Clicchiamo sul **+** e impostiamo come trigger ***A button is clicked,*** come source il nome che abbiamo assegnato al pulsante, come action ***Call new virtual function*** e infine inseriamo un nome significativo come function name, per esempio **getTime**. Di questa funzione dovrà essere effettuato poi l’overriding nel codice.

![Screen Creazione interfaccia grafica 2](https://user-images.githubusercontent.com/7826610/174487359-26567280-9716-453d-b17b-3a95ab1bf793.png)

A questo punto possiamo generare il codice di partenza premendo sulla prima icona </> rosa in basso a destra.

## Importare progetto su STM32CubeIDE

Per modificare il codice, procediamo ad importare il progetto generato con TouchGFX su STM32CubeIDE. Apriamo dunque STM32CubeIDE e andiamo su **File** > **Open projects from File System…** poi clicchiamo su **Directory…** e selezioniamo la directory del progetto creato precedentemente con TouchGFX. Assicurarsi che ci sia la spunta **Search for nested projects.** A questo punto dovrebbe essere rilevato almeno un progetto e quindi clicchiamo **Finish**.

Chiudere l’Information Center se non lo si è già fatto. Nel project explorer a sinistra dovrebbero esserci tutti i file di progetto generati da TouchGFX. 

![Screen Importare progetto su STM32CubeIDE](https://user-images.githubusercontent.com/7826610/174487375-ff59014c-239f-4d7f-a50e-9f286fd66eb3.png)

## Preparare la board per OPC UA

Affinché la board possa comunicare secondo il protocollo OPC UA, è necessario che sia in grado di comunicare con IP. In particolare, per questa Proof of Concept, si è utilizzata la libreria [open62541](https://www.open62541.org/), che può essere utilizzata in configurazioni che, come la nostra board, utilizzano FreeRTOS e lwIP, come spiegato nella [documentazione](https://www.open62541.org/doc/1.3/building.html#freertos-lwip). Procediamo quindi a configurare i moduli FreeRTOS, lwIP ed Ethernet opportunamente. 

Per la configurazione di Ethernet ed lwIP, i riferimenti principali sono la [documentazione](https://www.open62541.org/doc/1.3/building.html#freertos-lwip) di open62541 e [questo video](https://www.youtube.com/watch?v=Wg3edgNUsTk).

Clicchiamo due volte sul file **STM32F769I_DISCO.ioc**. Nella figura precedente si può notare che ce ne sono due. Quello da aprire è il primo a partire dall’alto che sta subito sopra *STM32F769I_DISCO.launch.* Aprendo l’altro si riceverà un errore.

### Interfaccia Ethernet
    
Alla richiesta di cambia visualizzazione cliccare Yes. Nella nuova schermata procediamo alla configurazione. Per prima cosa, attiviamo il modulo **Connectivity** → **ETH.** Per la board che stiamo considerando la modalità corretta è **RMII.**

In **Parameter Settings** impostiamo **PHY Address = 0** per stabilire che utilizziamo l’interfaccia Ethernet integrata nella board.

![Screen Interfaccia Ethernet](https://user-images.githubusercontent.com/7826610/174487505-50f10113-4743-4e24-bc7b-c924aa21933e.png)

    
### lwIP

Passiamo quindi al componente lightweight IP, necessario per la libreria open62541. Nella stesso file .ioc, andiamo in **Middleware → lwIP.** Dopo averlo abilitato con la spunta, dobbiamo configurare i parametri seguenti:

In **Key Options,** dopo aver spuntato **Show Advanced Parameters**

MEM_SIZE = 16*1024

LWIP_COMPAT_SOCKETS = 0

SO_REUSE = 1

In **General Settings:**

LWIP_DNS = Enabled

Opzionalmente, si può anche disattivare il DHCP e impostare un IP fisso.
    
### Porta seriale USART1 per debug
    
Per utilizzare la printf e visualizzare i log della libreria open62541, è consigliabile abilitare l’interfaccia seriale USART1, che nel caso della board in considerazione è quella collegata alla porta USB. Per farlo basta andare in **Connectivity** → **USART1** e impostare **Mode** = **Asynchronous.**
    
### FreeRTOS

Andiamo in **Middleware** → **FreeRTOS**

In **Config Parameters** impostiamo **TOTAL_HEAP_SIZE** = **300000**

In **Advanced Settings** impostiamo **USE_NEWLIB_REENTRANT = Enabled**

Adesso creiamo un Task (thread) che eseguirà il nostro client OPC UA. Andiamo in **Task and Queues** ai tre Tasks già presenti ne aggiungiamo un altro e lo configuriamo come mostrato nella figura qui sotto.

![FreeRTOS Tasks](https://user-images.githubusercontent.com/7826610/174487646-cc91e33c-618c-4097-a572-995f1358e32f.png)

Questo task deve comunicare con il task che gestisce l’interfaccia grafica. Per consentirlo aggiungiamo due Queue nel riquadro subito sotto. 

Configuriamo la prima ***opcuaRequestQueue*** come raffigurato di seguito:

![opcuaRequestQueue](https://user-images.githubusercontent.com/7826610/174487835-9a43957f-7e10-4dc8-82b2-8a16757c10c6.png)

In questa prima coda il task dell’interfaccia grafica scriverà un intero per dire al task OPC UA di fare una lettura dal server. 

Configuriamo la seconda ***opcuaResultQueue*** come raffigurato di seguito:

![opcuaResultQueue](https://user-images.githubusercontent.com/7826610/174487855-fe8796f1-c87d-4fb0-bdbc-295772e90073.png)

In questa seconda coda, il task OPC UA scriverà un intero per comunicare al task della grafica che un nuovo dato è disponibile. La situazione finale dovrebbe essere questa:

![FreeRTOS Tasks and Queues](https://user-images.githubusercontent.com/7826610/174487863-76f62b44-48b0-460c-8ae3-5406e58b962e.png)
    
### Ethernet DMA Buffers

Prima di chiudere il file **.ioc** restano alcune importanti configurazioni da fare. Di default i buffer dell’interfaccia Ethernet vengono allocati in un’area di memoria soggetta al caching e ciò causa problemi di coerenza. Dobbiamo quindi configurare una zona della memoria RAM *non-cacheable* e stabilire che vi vengano allogati questi buffer*.* Per maggiori informazioni fare riferimento al video citato sopra.

Per fare ciò ci spostiamo in **System Core → CORTEX_M7.** In **Parameter Settings** troviamo la definizione di alcune **Memory Protection Unit Regions.** Andiamo in fondo fino alla prima *Disabled.* Configurare altre due MPU Regions (**Region 5** e **Region 6**) esattamente come in figura:

![MCU Regions](https://user-images.githubusercontent.com/7826610/174487904-adc9debc-919f-4826-a79c-64e79436adf8.png)

Per entrambe le regions il Base Address è `0x2007C000`

> ℹ️ Questo è l’indirizzo su cui è mappato l’inizio della memoria SRAM1, come illustrato a pagina 76 del [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00224583-stm32f76xxx-and-stm32f77xxx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf) della board.

A questo punto possiamo salvare il file **.ioc** (CTRL+S), verrà chiesto di generare il codice e di passare alla visualizzazione C/C++. Cliccare **Yes**.

Adesso dobbiamo allocare i buffer in questa regione di memoria appena definita.

Aprire il file ****STM32F769NIHX_FLASH.ld.** Sostituire il seguente codice:

```c
/* Memories definition */
MEMORY
{
  RAM    (xrw)    : ORIGIN = 0x20000000,   LENGTH = 512K
  FLASH    (rx)    : ORIGIN = 0x8000000,   LENGTH = 2048K
  QUADSPI (r)    : ORIGIN = 0x90000000, LENGTH = 64M
  SDRAM   (xrw)  : ORIGIN = 0xC0000000,  LENGTH = 16M
}
```

con

```c
/* Memories definition */
MEMORY
{
  RAM    	(xrw)   : ORIGIN = 0x20000000,  LENGTH = 496K
  FLASH    	(rx)    : ORIGIN = 0x8000000, 	LENGTH = 2048K
  QUADSPI 	(r)    	: ORIGIN = 0x90000000, 	LENGTH = 64M
  SDRAM   	(xrw)  	: ORIGIN = 0xC0000000,  LENGTH = 16M

  /* Non-cachable ethernet buffers in SRAM1 */
  ETH1(xrw)   : ORIGIN = 0x2007C000, LENGTH = 0x80
  ETH2(xrw)   : ORIGIN = 0x2007C080, LENGTH = 0x80
  ETH3(xrw)   : ORIGIN = 0x2007C100, LENGTH = 0x17d0 
  ETH4(xrw)   : ORIGIN = 0x2007D8D0, LENGTH = 0x17d0
}
```

e aggiungere, alla fine del file subito prima della parentesi graffa chiusa **}**, il seguente codice

```c
/* Ethernet Sections */
.RxDescripSection (NOLOAD) : { *(.RxDescripSection) } >ETH1
.TxDescripSection (NOLOAD) : { *(.TxDescripSection) } >ETH2
.RxArraySection (NOLOAD) : { *(.RxArraySection) } >ETH3
.TxArraySection (NOLOAD) : { *(.TxArraySection) } >ETH4
```

Il file completo e modificato è disponibile [qui](stm-opc/STM32CubeIDE/STM32F769NIHX_FLASH.ld).

Salvare e chiudere il file. 

Aprire il file **ethernetif.c** che si può trovare come in figura sotto in 🟦 **STM32F769I_DISCO → Application → User → LWIP → Target**

![ethernetif.c where](https://user-images.githubusercontent.com/7826610/174488132-ec4fb46e-a051-4766-a11c-529b53f97a08.png)

Modificare l’***USER CODE 1*** come di seguito:

```c
/* USER CODE BEGIN 1 */
#if false
/* USER CODE END 1 */
```

E l’***USER CODE 2*** come di seguito:

```c
/* USER CODE BEGIN 2 */
#endif

__attribute__ ((section(".RxDescripSection"), used)) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB];
__attribute__ ((section(".TxDescripSection"), used)) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB];
__attribute__ ((section(".RxArraySection"), used)) uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];
__attribute__ ((section(".TxArraySection"), used)) uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];

/* USER CODE END 2 */
```

Salvare e chiudere il file.

## Aggiungere la libreria open62541

A questo punto è tutto pronto per aggiungere al progetto la libreria open62541. Per farlo bisogna generare due file **open62541.c** e **open62541.h** a partire dal codice sorgente della libreria, lanciando la compilazione con opportune definizioni di simboli. Entrambi i file sono già disponibili rispettivamente [qui](https://github.com/Haimrich/stm-opc/blob/main/stm-opc/Core/Src/open62541.c) e [qui](https://github.com/Haimrich/stm-opc/blob/main/stm-opc/Core/Inc/open62541.h), sono relativi alla versione 1.3.1 di open62541 e possono essere scaricati e inseriti nel progetto come vedremo più avanti.

Di seguito è comunque riportata la spiegazione su come ottenere e modificare tali file. Se però si è scelto di scaricarli, si può saltare alla parte [Aggiunta dei file al progetto](#Aggiunta-dei-file-al-progetto) di questa sezione.

### Generazione dei file .c e .h

**Linux/Unix**

Assicurarsi di avere installato i pacchetti **git** e **cmake**. Per esempio su Ubuntu con:

```bash
sudo apt-get update
sudo apt-get install git cmake
```

Procedere con la clonazione del repository e la compilazione parziale del codice

```bash
git clone https://github.com/open62541/open62541.git
cd open62541
git checkout tags/v1.3.1
mkdir build
cd build
cmake -DUA_ARCHITECTURE=freertosLWIP -DUA_ENABLE_AMALGAMATION=ON \
-DUA_ARCH_FREERTOS_USE_OWN_MEMORY_FUNCTIONS=ON ../
make
```

La compilazione dovrebbe terminare con un errore, però nella cartella build/ saranno stati generati i file open62541.h e open62541.c

**Windows**

Per la compilazione su Windows è richiesto Visual Studio e [CMake](https://cmake.org/download/). 

Scaricare il Source Code .zip di open62541, per esempio della versione 1.3.1 da [qui](https://github.com/open62541/open62541/releases/tag/v1.3.1) e decomprimerlo. Aprire CMake e dopo aver cliccato **Browse Source…** selezionare la cartella contenente il codice sorgente di open62541. Cliccare su **Browse Build…** e selezionare la cartella dove generare i file  e cliccare **Finish**.

Cliccare **Configure** Poi impostare:

UA_ARCHITECTURE=freertosLWIP

UA_ENABLE_AMALGAMATION=ON

Cliccare **Configure** Poi impostare:

UA_ARCH_FREERTOS_USE_OWN_MEMORY_FUNCTIONS=ON

Cliccare **Configure** poi **Generate** e infine **Open Project. Dovrebbe aprirsi una soluzione di Visual Studio.** Andare dal menu in alto in **Compilazione → Compilazione → ALL BUILD.**

La compilazione dovrebbe terminare con un errore, ma i file open62541.h e open62541.c verranno generati nella cartella scelta precedentemente.

### Modifica dei file

Aprire open62541.h con un qualsiasi editor di testo. Cercare `OPEN62541_FEERTOS_USE_OWN_MEM`. Sostituire questa parte di codice:

```c
#ifdef OPEN62541_FEERTOS_USE_OWN_MEM
# define UA_free vPortFree
# define UA_malloc pvPortMalloc
# define UA_calloc pvPortCalloc
# define UA_realloc pvPortRealloc
#else
```

con

```c
#ifdef OPEN62541_FREERTOS_USE_OWN_MEM
# define UA_free vPortFree
# define UA_malloc pvPortMalloc
// # define UA_calloc pvPortCalloc
// # define UA_realloc pvPortRealloc
#else
```

Notare che è stata corretta la mancanza di R in `OPEN62541_FEERTOS_USE_OWN_MEM` e sono state commentate le due righe. Salvare e chiudere il file.

Aprire **open62541.c** con un qualsiasi editor di testo. Dobbiamo implementare le due funzioni che prima abbiamo commentato `UA_calloc` e `UA_realloc`, perché non sono implementate in FreeRTOS per questa board. Per farlo, aggiungere al file, dopo l’include dell’header, il seguente codice:

```c
// calloc e realloc definition
#ifdef OPEN62541_FREERTOS_USE_OWN_MEM

void *UA_calloc(size_t count, size_t size)
{
    void *p;

    /* allocate 'count' objects of size 'size' */
    p = UA_malloc(count * size);
    if (p) {
        /* zero the memory */
        memset(p, 0, count * size);
    }
    return p;
}

void *UA_realloc(void *mem, size_t newsize)
{
    if (newsize == 0) {
        UA_free(mem);
        return NULL;
    }

    void *p;
    p = UA_malloc(newsize);
    if (p) {
        /* zero the memory */
        if (mem != NULL) {
            memcpy(p, mem, newsize);
            UA_free(mem);
        }
    }
    return p;
}

#endif
```

Salvare e chiudere il file.

### Aggiunta dei file al progetto 

Una volta ottenuti ed eventualmente modificati i due file **open62541.h** e **open62541.c**, possiamo includerli nel progetto. Andiamo quindi nella cartella che ospita il progetto. Possiamo arrivarci anche da STM32CubeIDE facendo tasto destro nel Project Explorer su 🟦 STM32F769I_DISCO con l’icona IDE azzurra a sinistra, poi **Show In** → **System Explorer.** Tra le varie cartelle dovrebbe esserci la cartella **Core** che contiene le cartelle **Inc** e **Src.** Nella prima mettiamo il file **open62541.h** e nella seconda il file **open62541.c**

Mentre il primo verrà incluso automaticamente nella compilazione, il file delle implementazioni **open62541.c** deve essere incluso manualmente nell’**IDE.** Per fare ciò, andiamo nel Project Explorer, poi 🟦 **STM32F769I_DISCO** (con l’icona IDE accanto) → **Application** → **User**, facciamo tasto destro su **User** poi **New** → **File.** Clicchiamo **Advanced,** spuntiamo **Link to file in the filesystem** e in **Browse** selezioniamo il file **open62541.c** che avevamo messo in **Src**, infine clicchiamo **Finish.** Dovremmo ritrovarcelo vicino al main.c

Prima di procedere con la compilazione del progetto dobbiamo definire alcuni simboli per la compilazione. Facciamo tasto destro su **STM32F769I_DISCO** (con l’icona IDE accanto) e poi **Properties.**

In **C/C++ Build → Settings → MCU GCC Compiler → Miscellaneous**, clicchiamo sul foglio con il + verde a destra e aggiungiamo il flag `-fcommon`, per evitare un errore di ridefinizione durante la compilazione.

In **C/C++ General → Paths and Symbols → Symbols,** clicchiamo su **Add…** e definiamo il simbolo `OPEN62541_FREERTOS_USE_OWN_MEM` e spuntiamo le due checkbox sotto, value possiamo lasciarlo vuoto. Facciamo la stessa cosa per il simbolo `UA_ARCHITECTURE_FREERTOSLWIP`.

Infine, in 🟦 **STM32F769I_DISCO** → **Application** → **User** facciamo tasto destro sul file **sysmem.c** poi **Resource Configurations → Exclude from build → Select all → Ok.** 


> ❗ Il file **sysmem.c** viene rigenerato ogni volta che si effettua una modifica al file **.ioc** e deve quindi essere escluso nuovamente dalla compilazione altrimenti genererà errori.

A questo punto possiamo fare una compilazione di prova per vedere se la libreria viene compilata con successo: tasto destro su 🟦 **STM32F769I_DISCO** → **Build Project.**

In caso di successo abbiamo quindi a disposizione tutte le API di open62541 per creare un server OPC UA o, come nel caso di questo tutorial, creare un client OPC UA.

## (Opzionale) Printf Redirect 

Se precedentemente abbiamo attivato l’USART1, per far sì che possiamo visualizzare messaggi stampanti con la printf o i messaggi di log della libreria open62541, dobbiamo reimplementare la funzione `_write()` in **main.c**. 

Nell’IDE apriamo il file 🟦 **STM32F769I_DISCO** → **Application** → **User → main.c** e modifichiamo l’**USER CODE 0** come segue:

```c
/* USER CODE BEGIN 0 */
// Redirect printf to USART1
int _write(int file, char *ptr, int len)
{
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, 100);
    return (status == HAL_OK ? len : 0);
}
/* USER CODE END 0 */
```

Per visualizzare i messaggi ricevuti sulla porta seriale sul pc a cui la board è collegata attraverso USB, su Windows possiamo usare [TeraTerm](https://ttssh2.osdn.jp/index.html.en). All’apertura del terminale, mentre la board è collegata selezioniamo la porta opportuna come in figura:

![TeraTerm screen](https://user-images.githubusercontent.com/7826610/174488244-ea017cbb-cc6b-488a-977e-c7a2cd21e89b.png)

Una volta premuto Ok, bisogna anche impostare il baud-rate corretto andando in **Setup → Serial Port** e impostando `115200` come **speed**. Per una visualizzazione corretta impostare anche **AUTO** in **Setup → Terminal → New line → Receive**.

## Implementazione del Client OPC UA 

### opcua.h 

Passiamo adesso ad implementare il Client OPC UA. Creiamo il file **opcua.h** nella cartella **Core → Inc**, in cui abbiamo messo anche **open62541.h**, e definiamo alcuni tipi di dato e variabili:

```c
#ifndef OPCUA_H_
#define OPCUA_H_

#include "stdint.h"
#include "cmsis_os.h"

typedef struct Tempo {
    uint16_t hour;
    uint16_t min;
    uint16_t sec;
} Tempo;

typedef struct GlobalData {
    double availableMemory;
    Tempo opcuaTime;
} GlobalData;

// Global variable shared between tasks declaration
extern GlobalData gData;

// Types of result message notification in opcuaResultQueue

extern osMessageQueueId_t opcuaResultQueueHandle;

#define OPCUA_NEW_AVAILABLE_MEMORY_VALUE  0
#define OPCUA_NEW_TIME_VALUE              1

// Types of request message notification in opcuaRequestQueue

extern osMessageQueueId_t opcuaRequestQueueHandle;

#define OPCUA_REQUEST_TIME                0

#endif
```

### opcua.c 

Creiamo il file **opcua.c** in **Core → Src** e poi lo includiamo nel progetto esattamente come abbiamo fatto prima per **open62541.c**

La struttura è simile a questo [esempio](https://github.com/open62541/open62541/blob/master/examples/client_subscription_loop.c) ed è stato adattata ai Task di FreeRTOS. Creeremo una subscription per aggiornare in tempo reale il valore di RAM disponibile sul server e leggeremo dalla coda `opcuaRequestQueue` se il task della grafica ha chiesto di leggere l’orario dal server.

```c
#include "opcua.h"

#include "open62541.h"
#include "cmsis_os.h"

#define OPCUA_SERVER_ENDPOINT "opc.tcp://192.168.1.33:4840"

// Shared Global Variables between tasks implementation
GlobalData gData;

// New availableMemory variable received from subscription handler
static void
handler_availMemoryChanged(UA_Client * client, UA_UInt32 subId, void * subContext,
    UA_UInt32 monId, void * monContext, UA_DataValue * value) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Ricevuto nuovo valore di RAM disponibile!");

    if (UA_Variant_hasScalarType( & value -> value, & UA_TYPES[UA_TYPES_DOUBLE])) 
    {
        UA_Double double_data = * ((UA_Double * ) value -> value.data);
        gData.availableMemory = double_data;

        // Comunico all'altro thread che c'è un nuovo valore di RAM disponibile
        uint8_t status = OPCUA_NEW_AVAILABLE_MEMORY_VALUE;
        osMessageQueuePut(opcuaResultQueueHandle, & status, 0, osWaitForever);
    }
}

static void
deleteSubscriptionCallback(UA_Client * client, UA_UInt32 subscriptionId, void * subscriptionContext) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Subscription Id %lu was deleted", subscriptionId);
}

static void
subscriptionInactivityCallback(UA_Client * client, UA_UInt32 subId, void * subContext) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Inactivity for subscription %lu", subId);
}

// Funzione chiamata al cambio dello stato del client, 
// per esempio appena viene attivata una sessione
static void
stateCallback(UA_Client * client, UA_SecureChannelState channelState,
    UA_SessionState sessionState, UA_StatusCode recoveryStatus) {
    switch (sessionState) 
    {
    case UA_SESSIONSTATE_ACTIVATED: {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "A session with the server is activated");

        // Appena la sessione è attivata procediamo con la creazione della subscription

        UA_CreateSubscriptionRequest request = UA_CreateSubscriptionRequest_default();
        UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(client, request, NULL, NULL, deleteSubscriptionCallback);

        if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Create subscription succeeded, id %lu", response.subscriptionId);
        else
            return;

        // Aggiungiamo il nodo n=1,s=AvailableMemory ai monitored items della subscription
        // Questo nodo deve esistere nel server opc ua
        UA_NodeId nodeId = UA_NODEID_STRING(1, "AvailableMemory");
        UA_MonitoredItemCreateRequest monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
        UA_MonitoredItemCreateResult monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_BOTH, monRequest, NULL, handler_availMemoryChanged, NULL);

        if (monResponse.statusCode == UA_STATUSCODE_GOOD)
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring n=1,s=AvailableMemory', id %lu", monResponse.monitoredItemId);
    }
    break;
    case UA_SESSIONSTATE_CLOSED:
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Session disconnected");
        break;
    default:
        break;
    }
}

// Questa è l'entry function dell'opcuaTask che avevamo configurato nel file .ioc
// Qui la implementiamo
void StartOpcuaTask(void * argument) 
{
    // Creazione del client
    UA_Client * client = UA_Client_new();
    UA_ClientConfig * client_config = UA_Client_getConfig(client);
    UA_ClientConfig_setDefault(client_config);

    client_config -> stateCallback = stateCallback;
    client_config -> subscriptionInactivityCallback = subscriptionInactivityCallback;

    // Task Main Loop
    for (;;) {
        // Prova a connetersi ogni secondo, se è già connesso ignora
        UA_StatusCode retval = UA_Client_connect(client, OPCUA_SERVER_ENDPOINT);
        if (retval != UA_STATUSCODE_GOOD) {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Not connected. Retrying to connect in 1 second");
            osDelay(1000);
            continue;
        }

        uint8_t message = 0;

        // Legge la coda delle richieste inviate dal task della GUI
        if (osMessageQueueGet(opcuaRequestQueueHandle, & message, NULL, 0) == osOK) {
            switch (message) {
            case OPCUA_REQUEST_TIME: {
                printf("Bottone Premuto.\n");
                Tempo tempo = (Tempo) {0, 0, 0};

                UA_Variant value;
                UA_Variant_init( & value);

                // Lettura del nodo contenente l'ora attuale dal server
                // Questo nodo è già presente nel namespace di default 0
                const UA_NodeId nodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME);
                retval = UA_Client_readValueAttribute(client, nodeId, & value);

                if (retval == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType( & value, & UA_TYPES[UA_TYPES_DATETIME])) {
                    UA_DateTime raw_date = * (UA_DateTime * ) value.data;
                    UA_DateTimeStruct dts = UA_DateTime_toStruct(raw_date);
                    printf("date is: %u-%u-%u %u:%u:%u\n", dts.day, dts.month, dts.year, dts.hour, dts.min, dts.sec);
                    tempo = (Tempo) {
                        dts.hour, dts.min, dts.sec
                    };
                }

                UA_Variant_clear( & value);

                gData.opcuaTime = tempo;

                // Comunico all'altro task che c'è un nuovo valore disponibile
                message = OPCUA_NEW_TIME_VALUE;
                osMessageQueuePut(opcuaResultQueueHandle, & message, 0, osWaitForever);
            }
            break;
            // gestire eventuali altri tipi di richieste con altri cases
            default:
                break;
            }
        }

        UA_Client_run_iterate(client, 1000);
    }

    // Questa parte in realtà non verrà mai eseguita
    UA_Client_delete(client);
}
```

## Interazione dell’interfaccia grafica con il client OPC UA 

Una volta implementato il client OPC UA, dobbiamo far sì che quando nella coda `opcuaResultQueue` c’è una notifica di nuovo valore, venga aggiornato l’elemento grafico opportuno. Inoltre quando premiamo il bottone azzurro dobbiamo comunicare attraverso la coda `opcuaRequestQueue` al client opc di leggere l’orario aggiornato dal server. La prima parte è quella che richiede più passaggi. TouchGFX si basa sul pattern [Model-View-Presenter](https://support.touchgfx.com/4.14/docs/development/ui-development/software-architecture/model-view-presenter-design-pattern). 

### Model.cpp 

Il controllo periodico della nostra coda dei risultati verrà effettuato dal Model che lo comunicherà al Presenter. Il Presenter fa da ponte tra il Model alla View, che è l’oggetto che contiene i vari elementi grafici della schermata.

Procediamo dunque con la prima parte e apriamo il file 
**TouchGFX → gui → src → model → Model.cpp**

e lo modifichiamo come segue

```cpp
#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

extern "C" {
    // gData declaration, queues declaration, etc.
    #include "opcua.h"
    #include "cmsis_os.h"
}

Model::Model(): modelListener(0) {

}

void Model::tick() 
{
    uint8_t message;
    if (osMessageQueueGet(opcuaResultQueueHandle, & message, NULL, 0) == osOK) 
    {
        switch (status) 
        {
        case OPCUA_NEW_TIME_VALUE:
            modelListener -> updateTime(gData.opcuaTime);
            break;
        case OPCUA_NEW_AVAILABLE_MEMORY_VALUE:
            modelListener -> updateMemoryGauge(gData.availableMemory);
            break;
        default:
            break;
        }
    }
}
```

### ModelListener.hpp 

Nel codice abbiamo chiamato le funzioni `updateTime()` e `updateMemoryGauge()` ma dobbiamo dichiararle nell’header **ModelListener.hpp**, per farlo possiamo fare CTRL + click sull’include corrispondente del **model.c** oppure andare in **TouchGFX → gui → include → gui → model → ModelListener.hpp**

Qui aggiungiamo le dichiarazioni come funzioni virtual e otteniamo:

```cpp
#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

extern "C" {
#include "opcua.h"
}

class ModelListener {
   public:
    ModelListener() : model(0) {}

    virtual ~ModelListener() {}

    void bind(Model* m) { model = m; }

    virtual void updateTime(Tempo tempo) {}
    virtual void updateMemoryGauge(double availMemory) {}

   protected:
    Model* model;
};

#endif  // MODELLISTENER_HPP
```

### MainScreenPresenter.cpp 

Di queste due funzioni virtual faremo l’overriding nel presenter della nostra MainScreen che è una classe figlia del ModelListener. Per fare ciò andiamo in 
**TouchGFX → gui → src → mainscreen_screen → MainScreenPresenter.cpp** e implementiamo l’overriding delle due funzioni, che consisterà nel chiamare due funzioni (che ancora dobbiamo implementare) della view di cui il presenter ha un riferimento, che saranno quelle che effettivamente aggiorneranno l’interfaccia:

```cpp
#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{
}

void MainScreenPresenter::activate()
{
}

void MainScreenPresenter::deactivate()
{
}

void MainScreenPresenter::updateTime(Tempo tempo)
{
    view.updateTime(tempo);
}

void MainScreenPresenter::updateMemoryGauge(double availMemory)
{
    view.updateGauge(availMemory);
}
```

### MainScreenPresenter.hpp  

Anche questi due metodi devono essere dichiarati nell’header, quindi dal .cpp facciamo CTRL + click sull’include corrispondente oppure andiamo in **TouchGFX → gui → include → gui → mainscreen_screen → MainScreenPresenter.hpp**

```cpp
#ifndef MAINSCREENPRESENTER_HPP
#define MAINSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

extern "C" 
{
#include "opcua.h"
}

using namespace touchgfx;

class MainScreenView;

class MainScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    MainScreenPresenter(MainScreenView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~MainScreenPresenter() {};

    void updateTime(Tempo tempo);
    void updateMemoryGauge(double availMemory);

private:
    MainScreenPresenter();

    MainScreenView& view;
};

#endif // MAINSCREENPRESENTER_HPP
```

### MainScreenView.cpp  

Finalmente possiamo adesso implementare le funzioni `updateTime()` e `updateMemoryGauge()` nella view che a partire dal valore passato come argomento aggiornerano gli elementi grafici.

Andiamo in **ouchGFX → gui → src → mainscreen_screen → MainScreenView.cpp**

E aggiungiamo le implementazioni delle funzioni due funzioni. Visto che siamo qui, implementiamo anche la funzione da chiamare alla pressione del pulsante azzurro, che deve avere lo stesso nome di quello specificato nel TouchGFX Designer, in questo caso `getTime()`.

La funzione `getTime()` invia un messaggio nella coda delle richieste che verrà poi letto dal task del client OPC UA.

```cpp
#include <gui/mainscreen_screen/MainScreenView.hpp>

extern "C"
{
#include "opcua.h"
#include "cmsis_os.h"
}

MainScreenView::MainScreenView()
{
}

void MainScreenView::setupScreen()
{
    MainScreenViewBase::setupScreen();
}

void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
}

// Chiamata dal presenter
void MainScreenView::updateTime(Tempo tempo) {
    digitalClock.setTime24Hour(tempo.hour, tempo.min, tempo.sec);
    // Invalidate chiede a TouchGFX di renderizzare nuovamente l'elemento
    digitalClock.invalidate();
}

// Chiamata dal presenter
void MainScreenView::updateMemoryGauge(double availMemory) {
    // Il secondo valore è la velocità dell'animazione della lancetta
    memoryGauge.updateValue(100 - availMemory, 30);
}

// Chiamata alla pressione del pulsante azzurro
void MainScreenView::getTime() {
    uint8_t message = OPCUA_REQUEST_TIME;
    osMessageQueuePut(opcuaRequestQueueHandle, &message, 0, 0);
}

```

### MainScreenView.hpp 

L’unica cosa che resta da fare è aggiungere le dichiarazioni nell’header, quindi come sempre CTRL + click nell’include corrispondente, oppure in **TouchGFX → gui → include → gui → mainscreen_screen → MainScreenView.hpp**

```cpp
#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

extern "C" 
{
#include "opcua.h"
}

class MainScreenView : public MainScreenViewBase
{
public:
    MainScreenView();
    virtual ~MainScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void getTime();
    void updateTime(Tempo tempo);
    void updateGauge(double availMemory);

protected:
};

#endif // MAINSCREENVIEW_HPP
```

Se si vuole, da questo file con CTRL + click sull’include del **MainScreenViewBase.hpp** si può andare a vedere la definizione della classe base generata da TouchGFX che contiene tutti gli elementi grafici e la funzione virtual del pulsante di cui abbiamo fatto l’overriding. Può essere utile per assicurarsi che il nome delle variabili e delle funzioni sia corretto, ma non va modificato.

## OPC UA Server 

Per testare se tutto funziona ci serve un server OPC UA a cui collegarci. Per esempio possiamo usare **[node-opcua](https://node-opcua.github.io/).** Una volta creato il server assicurarsi di configurare l’IP corretto in `OPCUA_SERVER_ENDPOINT` nel file **opcua.c**

L’unico nodo da aggiungere al server opcua, per il nostro esempio, è quello della memoria ram disponibile, che deve avere come identificativo (n=1,s=AvailableMemory). Il codice JavaScript per il server assomiglierà quindi a questo:

```jsx
const opcua = require("node-opcua");
const { OPCUAServer, Variant, DataType } = require("node-opcua");

const os = require("os");

(async () => {
    try {
        const server = new OPCUAServer({
            port: 4840,
            resourcePath: "",
            buildInfo: {
                productName: "OpcUa Server",
                buildDate: new Date(),
            }
        });

        await server.initialize();

        const addressSpace = server.engine.addressSpace;
        const namespace = addressSpace.getOwnNamespace();

        // Main Folder
        const myFolder = namespace.addFolder("ObjectsFolder", {
            browseName: "MyFolder"
        });

        // ServerStats Object
        const serverStats = namespace.addObject({
            organizedBy: addressSpace.rootFolder.objects.myFolder,
            browseName: "ServerStats"
        })

        // Free Memory Variable
        const freeMemory = namespace.addVariable({ //Variable
            componentOf: serverStats,
            browseName: "AvaiableMemory",
            description:
            {
                locale: "it-IT",
                text: "Memoria disponibile nel server"
            },
            nodeId: "s=AvailableMemory", // ID importante deve coincidere in opcua.c
            dataType: "Double",
            value: {
                refreshFunc: function (callback) {
                    let dataValue = new opcua.DataValue({
                        value: new opcua.Variant({
                            dataType: opcua.DataType.Double,
                            value: available_memory()
                        }),
                        sourceTimestamp: new Date()
                    });
                    callback(null, dataValue);
                }
            }
        });

        // Server Start
        await server.start();
        console.log("Server Started at: ", server.getEndpointUrl());

    } catch (err) {
        console.log(err);
        process.exit(1);
    }
})();

// Utility Functions

function available_memory() {
    const freeMemoryPercentage = os.freemem() / os.totalmem() * 100.0;
    return freeMemoryPercentage;
}
```

Avviamo quindi il server.

## Esecuzione 

Per caricare il progetto sulla board, colleghiamo la board al PC tramite USB. In STM32CubeIDE facciamo tasto destro su 🟦 **STM32F769I_DISCO** poi **Run as…** e clicchiamo sull’unica scelta disponibile**.** Dovrebbe partire la compilazione e subito dopo il flashing. Quando il processo sarà completato la board si riavvierà e il nuovo programma sarà stato caricato.

Nel frattempo, se abbiamo abilitato USART1 e sovrascritto `_write()`, possiamo aprire TeraTerm e configurarlo come spiegato in precedenza. Una volta che la board sarà attiva dovremmo vedere i messaggi di Log di open62541. All’inizio dovrebbe provare a connettersi al server ogni secondo. Spesso occorre qualche secondo prima che il DHCP assegni l’IP alla board. 

Una volta ottenuto l’IP dovrebbe partire la connessione del client, la creazione del canale, della sessione e infine della subscription. A quel punto si può provare a far variare la RAM occupata sul proprio pc per vedere se la lancetta del gauge sulla board si muove. Inoltre, premendo il pulsante azzurro, l’orologio digitale dovrebbe mostrare l’ora UTC attuale.

Il codice può essere facilmente esteso per monitorare altri nodi o per effettuare scritture sul server, ecc. Per esempio l’API di alto livello di open62541 per scrivere un valore di tipo Int32 è questa:

```c
UA_Variant *myVariant = UA_Variant_new();
UA_Variant_setScalarCopy(myVariant, &value, &UA_TYPES[UA_TYPES_INT32]);
UA_Client_writeValueAttribute(client, UA_NODEID_STRING(1, "IdNodoStringa"), myVariant);
UA_Variant_delete(myVariant);
```
