::::::: titlepage
::: center
![image](fiuba.pdf)\
[Universidad de Buenos Aires]{.smallcaps}\
[Facultad de Ingeniería]{.smallcaps}\
Año 2025 - 2^do^ cuatrimestre
:::

::: center
[[Taller de Sistemas Embebidos]{.smallcaps}]{.underline}\
[Trabajo Practico Final]{.smallcaps}
:::

::: center
[[Sistema de detección de apneas y variaciones de
oxigenación]{.smallcaps}]{.underline}\
[[durante el sueño]{.smallcaps}]{.underline}\
:::

::: tabbing
FECHA : 4/03/2026\
\
\
ESTUDIANTES: ̄̄\
Musso Carboni Tomás\
`tmusso@fi.uba.ar`\
Alan\
`fi.uba.ar`\
Archetti Santiago\
`sarchetti@fi.uba.ar`\
:::

------------------------------------------------------------------------

Taller de Sistemas Embebidos
:::::::

# Introducción general

## Sistema de detección de apneas y variaciones de oxigenación durante el Sueño

El sistema portátil de monitoreo respiratorio y de saturación de oxígeno
desarrollado en este trabajo cuenta con:

-   Una placa con microcontrolador *STM32 F103RB*, encargada de la
    adquisición, procesamiento digital de señales y control general del
    sistema.

-   Un sensor óptico *MAX30102*, que permite medir señales PPG mediante
    leds rojo e infrarrojo para el cálculo de $SpO_{2}$ y la estimación
    de la frecuencia respiratoria.

-   Un módulo de comunicación Bluetooth *HM-10*, que posibilita la
    transmisión inalámbrica de los datos hacia un dispositivo móvil para
    su visualización en tiempo real.

-   Un display, que muestra datos por pantalla en tiempo real.

-   Un sistema de alarmas locales compuesto por un buzzer y leds
    indicadores, que alertan al usuario ante condiciones críticas de
    saturación de oxígeno.

-   Procesamiento digital de la señal implementado en lenguaje C.

-   Lógica de control que activa alarmas cuando la saturación de oxígeno
    cae por debajo de los umbrales configurados.

-   Modos de funcionamiento configurables (modo adulto, modo niño).

    ![Diagrama de bloques del
    sistema](diagrama_de_bloques.png){#fig:diag_bloques
    width="0.6 \\linewidth"}

# Introducción específica

## Descripción del equipo

El dispositivo dispone de dos botones, uno de los cuales permite
seleccionar el modo de funcionamiento. Esta configuración es necesaria
debido a que los valores normales de saturación de oxígeno en sangre,
frecuencia respratoria y frecuencia cardíaca difieren según la edad del
usuario. Con esta consideración, se cuentan 2 modos de funcionamiento:
adulto o pediátrico. Una vez establecidos los parametros
correspondientes al modo seleccionado, el sensor óptico MAX30102 se
coloca en el dedo del usuario.

<figure id="fig:ppg2">
<p width=100% align=center ><img src="img/ppg.png" height=180 alt="image" /> <img height=180 src="img/ppg2.png" alt="image" /></p>
<figcaption>Funcionamiento del sensor MAX30102.</figcaption>
</figure>

Este sensor se utiliza para la medición de señales de fotopletismografía
(PPG), el mismo integra dos dfuentes de luz LED (una roja y una
infrarroja), un fotodiodo y un sistema de conversión analógico-digita.
Debido a que el volumen de sangre en los vasos varía con cada latido
cardíaco, la cantidad de luz absorbida también cambia de forma
periódica. Las variaciones de luz absorbida son captadas por el
fotodiodo del sensor, el cual convierte la intensidad de la luz
reflejada en una señal eléctrica. Los valores digitalizados corresponden
a las mediciones de luz detectada por los LEDs rojo e infrarrojo, los
cuales constituyen la señal PPG. Estos datos son transmitidos al
microcontrolador mediante $I^2C$. Posteriormente, las señales adquiridas
son sometidas a una etapa de procesamiento digital, en la cual se
aplican los algoritmos necesarios para calcular los valores de
saturación de oxígeno en sangre, frecuencia cardíaca y frecuencia
respiratoria.

Para la estimación de la frecuencia cardíaca, se utiliza principalmente
la señal PPG infrarroja. Se aplica un filtrado pasa-banda en el rango
aproximado de 0.5 a 5Hz, con el objetivo de eliminar ruidos para
posteriormente detectar los máximos locales de la señal filtrada, que
corresponden a latidos válidos. A partir del intervalo temporal entre
picos consecutivos, se calcula la frecuencia cardíaca en latidos por
minuto. En el caso de la saturación de oxígeno en sangre ($SpO_{2}$), se
utilizan ambas señales PPG. Cada una de estas señales se descompone en
dos componentes, una componente continua (DC) y una componente alterna
(AC). A partir de estas componentes se calcula una relación entre las
señales roja e infrerroja.

$$R = \frac{\frac{AC_{RED}}{DC_{RED}}}{\frac{AC_{IR}}{DC_{IR}}}$$

$$SpO_{2} \approx 110 -25*R$$

Por último, la frecuencia respiratoria puede estimarse analizando las
oscilaciones de baja frecuencia presentes en la señal infrarroja. Se
aplica un filtro pasa-bajos con una frecuencia cercana a 0.4 Hz.
Finalmente, mediante la detección de picos o el análisis de la
frecuencia dominante de la señal filtrada, se obtiene el número de
ciclos respiratorios por minuto. Luego de la etapa de procesamiento, y
considerando los rangos previamente establecidos según el modo de
funcionamiento seleccionado, el sistema transmite en tiempo real la
información obtenida tanto al display como a un dispositivo movil en
caso de haberse establecido la conexión mediante bluetooth. Los datos
transmitidos incluyen :

-   Detección de apnea (presencia o ausencia).

-   Frecuencia respiratoria actual.

-   Frecuencia cardíaca actual.

-   Saturacióin de oxígeno en sangre actual.

En caso de que la frecuencia respiratoria descienda por debajo de los
límites considerados peligrosos, el sistema activa el buzzer y un led
para alertar al usuario. Esta alarma puede desactivarse manualmente
mediante el botón de apagado correpondiente. Asimismo, el dispositivo
incorpora un modo de bajo consumo energético, el cual se activa
automáticamente cuando no se detecta contacto del usuario con el sensor
durante un período prolongado de tiempo.

## Estado del arte

La inspiración para el desarrollo de este proyecto se basa en el
funcionamiento de un oxímetro de pulso, el cual utiliza señales PPG
obtenidas mediante la emisión de luz roja e infrarroja para estimar la
saturación de oxígeno en sangre. Tomando como referencia este principio
de funcionamiento, el presente proyecto propone extender sus
capacidades, con el objetivo de desarrollar un sistema de monitoreo
completo durante el sueño. Además de estimar la saturación de
oxígeno,permite obtener y supervisar otros parámetros fisiológicos
relevantes, como la frecuencia cardíaca, la frecuencia respiratoria y la
detección de episodios de apnea, proporcionando así una herramienta más
completa para el seguimiento del estado fisiológico del usuario durante
el descanso.

<!-- ![Oxímetro de pulso](img/oximetro.png){#fig:oximetro width="40%"} -->
<p align=center> <img width=40% src=img/oximetro.png></p>

::: {#tab:product_comparison}
| **Characteristics** | **Product A**                                                                                          | **Product B**                                                                                          |
| ---------------------| --------------------------------------------------------------------------------------------------------| --------------------------------------------------------------------------------------------------------|
| Water-in port       | ½-inch connector                                                                                       | ½-inch connector                                                                                       |
| Irrigation circuits | Controls two independent irrigation circuits                                                           | Controls one irrigation circuit                                                                        |
| Operation modes     | Continuous: water flow is controlled by a button. Programmed irrigation: irrigation is time-controlled | Continuous: water flow is controlled by a button. Programmed irrigation: irrigation is time-controlled |
| Configuration       | The parameters "how often" to irrigate and "how long" to irrigate can be configured for each circuit   | The parameters "how often" to irrigate and "how long" to irrigate can be configured                    |
| User interface      | A rotary control key, two buttons, and a display                                                       | A set of buttons and a set of LEDs                                                                     |
| Power supply        | Two AA batteries                                                                                       | Four AA batteries                                                                                      |
| Sensors             | None                                                                                                   | None                                                                                                   |
| Price               | 80 USD                                                                                                 | 60 USD                                                                                                 |

  : Comparison of two commercial irrigation controller products.
:::


## Requerimientos funcionales

::: {#tab:tab_requerimientos}
| Grupo         | ID  | Requerimiento Funcional | Descripción                                                     |
| ---------------| -----| -------------------------| -----------------------------------------------------------------|
| Adquisición   | 1.1 | Adquisición de señales  | Capturar señales ópticas ROJO/IR del MAX30102 via I2C.          |
|               | 1.2 | Lectura periódica       | Obtener muestras desde la FIFO del sensor.                      |
|               | 1.3 | Análisis de variación   | Detectar variaciones innecesarias para $SpO_{2}$ y respiración  |
| Procesamiento | 2.1 | Filtrado digital        | Aplicar filtrado                                                |
|               | 2.2 | Cálculo de $SpO_{2}$    | Calcular $SpO_{2}$ mediante el ratio R                          |
|               | 2.3 | Cálculo de respiración  | Estimar frecuencia respiratoria mediante                        |
|               | 2.3 | Cálculo de respiración  | análisis de envolvente o línea base.                            |
|               | 2.4 | Empaquetado de datos    | Generar paquete con $SpO_{2}$ , respiración y estado.           |
| Indicadores   | 3.1 | Alarma crítica          | Activar alarma sonora cuando se detecta apnea.                  |
|               | 3.2 | Indicadores LED         | Mostrar estados (crítico, normal, error) mediante LEDs.         |
| Comunicación  | 4.1 | Bluetooth               | Transmitir datos vía HM-10 por UART.                            |
|               | 4.2 | Envío de parámetros     | Enviar $SpO_{2}$, respiraciones y estado del paciente a la app. |
|               | 4.3 | Registro de eventos     | Informar alarmas, desconexiones o estados especiales.           |
|               | 4.4 | Display                 | Mostrar parámetros de interes por pantalla.                     |
| Alimentación  | 5.1 | Alimentación            | Alimentación vía USB                                            |
|               | 5.2 | Bajo consumo            | Entrar en modo Sleep cuando no hay actividad.                   |
|               | 5.3 | Reactivación            | Retomar actividad ante movimiento o comando.                    |

  : Tabla de requerimientos
:::

## Casos de uso
Con respecto a los casos de uso, se pueden mencionar los siguientes:

El sistema cuenta con dos modos de funcionamiento: KID y ADULT, cada uno de los cuales establece parámetros específicos según el tipo de usuario.


Rangos de parámetros del modo KID: 



- Frecuencia cardíaca: {90,120}

- Frecuencia respiratoria: {15,30}


Rangos de parámetros del modo ADULT:

- Frecuencia cardíaca: {45,90}
- Frecuencia respiratoria : {10,20}

En ambos casos el límite para la saturación de oxígeno en sangre es $SP02_{LOW} = 80$.
Cabe destacar que el sistema puede utilizarse tanto con conexión Bluetooth como sin ella. Cuando la conexión está activa, los datos se envían al dispositivo móvil y, simultáneamente, se visualizan en la pantalla del sistema. En cambio, cuando no se establece la conexión Bluetooth, la información se muestra únicamente en la pantalla.
## Descripción de módulos externos utilizados

-   Display LCD 16x2

-   Sensor MAX30102

-   Módulo Bluetooth HM-10

### Display LCD 16x2

El display utilizado es un display LCD de 16x2 como se puede observar en
la figura [4](#fig:LCD){reference-type="ref" reference="fig:LCD"}.
Dichas conexiones se realizaron con una configuración de 4 bits. El
mismo efectúa la comunicación con la placa mediante pines *GPIO*.

<!-- ![Módulo 1602A](img/lcd_display.png){#fig:LCD width="50%"} -->
<p align=center><img width=50% src=img/lcd_display.png></p>

### HM-10

Para la transmisión de los datos hacia el dispositivo móvil del usuario
se emplea un módulo Bluetooth HM-10. La comunicación entre el módulo
Bluetooth y la placa de desarrollo se realiza usando la interfaz UART. A
través de esta interfaz serie, el microcontrolador transmite al módulo
los parámetros de interés para su posterior envío al dispositivo móvil
del usuario.

<!-- ![Módulo bluetooth HM-10](img/HM10.png){#fig:HM10 width="50%"} -->
<p align=center><img width=50% src=img/HM10.png></p>


### MAX30102

En cuanto a la conexión del módulo Bluetooth con la placa de desarrollo,
se realizaron cuatro conexiones principales: alimentación (VCC),
referencia a tierra (GND) y dos líneas de comunicación correspondientes
a la interfaz serie. Las líneas de datos se conectan mediante una
configuración cruzada entre los pines de transmisión y recepción. En
particular, el pin TX (transmit) del módulo se conecta al pin RX
(receive) del microcontrolador, mientras que el pin RX del módulo se
conecta al pin TX del microcontrolador permitiendo de esta forma el
intercambio de datos.

<!-- ![Sensor MAX30102](MAX30102.png){#fig: MAX30102 width="50%"} -->
<p align=center><img width=50% src=img/MAX30102.png></p>


# Diseño e Implementación
Este proyecto se eligió porque los problemas respiratorios durante el sueño, como la apnea, con muy comunes y muchas veces no se detectan a tiempo.
 La falta de diagnóstico puede generar cansancio crónico, bajo rendimiento y riesgos cardiovasculares, pero los estudios clínicos tradicionales 
, suelen ser costosos y difíciles de realizar en el hogar. Por eso, un dispositivo portátil que mida la respiración y la saturación de oxígeno 
durante la noche resulta una herramienta útil y accesible para identificar posibles alteraciones. 
Desde el punto de vista tecnológico, el proyecto es adecuado para el área de sistemas embebidos porque integra sensores reales, procesamiento digital, 
comunicación inalámbrica y manejo de bajo consumo. Además, el hardware seleccionado (MAX30102 y STM32F103RB) es económico, fácil de conseguir y cuenta 
con buena documentación, lo que facilita el desarrollo y permite enfocarse en el funcionamiento del sistema sin una complejidad excesiva. 
En conjunto, esto hace que el proyecto sea realizable, educativo y al mismo tiempo relevante desde lo biomédico. 
Para la elección del proyecto, se realiza un análisis profundo de los distintos criterios que lo componen, asignando un peso a cada uno de ellos para 
luego hacer un análisis total.

| Criterio                                             | Descripción                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 | Puntuación               | Peso final |
| ------------------------------------------------------| -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------| --------------------------| ------------|
| Tiempo y facilidad de implementación. Peso (7)       | El uso del sensor MAX30102 y el microcontrolador STM32F103RB presenta una complejidad moderada, debido a la necesidad de configurar correctamente la comunicación I²C, filtrar la señal PPG y aplicar algoritmos de cálculo de SpO₂ y frecuencia respiratoria. Sin embargo, existe amplia documentación y librerías disponibles, lo que reduce las dificultades de implementación. En conjunto, se considera que la complejidad técnica es manejable dentro del marco de un proyecto de sistemas embebidos. | <p align="center">7</p>  | 49         |
| Disponibilidad y costo de hardware. Peso (8)         | Los componentes principales —sensor óptico MAX30102, microcontrolador STM32, módulo Bluetooth y un buzzer/LED para alarmas— son económicos, fáciles de conseguir y ampliamente utilizados en proyectos biomédicos educativos. Esto permite realizar pruebas sin un costo elevado y garantiza buena disponibilidad de reemplazos.                                                                                                                                                                            | <p align="center">9</p>  | 72         |
| Facilidad de realización de pruebas. Peso (5)        | La verificación del funcionamiento del sistema puede realizarse mediante lectura directa de las señales PPG, monitoreo en tiempo real por UART/Bluetooth y observación de la activación de alarmas locales. Además, se pueden usar herramientas básicas como un multímetro o un osciloscopio para validar etapas eléctricas, lo que facilita enormemente el proceso de pruebas y depuración.                                                                                                                | <p align="center">7</p>  | 35         |
| Utilidad e interés personal en el proyecto. Peso (9) | El proyecto presenta un alto interés personal debido a su relación con aplicaciones biomédicas reales, la posibilidad de detectar apneas y desaturaciones durante el sueño, y su potencial mejora en versiones futuras. Además, permite aplicar conocimientos de sensores ópticos, filtrado digital y diseño embebido, lo que resulta atractivo tanto desde el punto de vista académico como práctico.                                                                                                      | <p align="center"> 9</p> | 81         |

<p align="center"><b>Tabla 1: Evaluación de criterios del proyecto</b></p>


## Hardware del sistema

### Comparación de módulos Bluetooth

| Módulo      | Tecnología                     | Interface            | Precio unitario [USD] |
| -------------| --------------------------------| ----------------------| -----------------------|
| HM-10       | Bluetooth Low Energy (BLE 4.0) | UART (TX/RX)         | 4                     |
| HC-05       | Bluetooth 2.0 (Classic)        | UART (TX/RX)         | 3                     |
| ESP32 (BLE) | Bluetooth + WiFi               | UART, SPI, I2C, GPIO | 8,50                  |

*Tabla: Comparación de módulos Bluetooth.*

---

### Comparación de sensores

| Sensor   | Tecnología                 | Interface | Precio unitario [USD] |
| ----------| ----------------------------| -----------| -----------------------|
| MAX30102 | PPG (Red + IR LED)         | I2C       | 5                     |
| MAX30100 | PPG (Red + IR LED)         | I2C       | 6                     |
| MAX30105 | PPG (IR + Particle sensor) | I2C       | 7                     |

*Tabla: Comparación de sensores.*

---

El módulo **HM-10** fue seleccionado debido a que implementa el estándar **Bluetooth Low Energy (BLE)**, lo que permite establecer comunicación con dispositivos móviles modernos manteniendo un bajo consumo de energía. Cabe destacar su bajo costo, tamaño reducido y amplia disponibilidad.

Con respecto al sensor **MAX30102**, fue elegido debido a la recomendación de especialistas en el área, su tamaño compacto y su costo accesible, lo cual lo hace adecuado para este proyecto.


A continuación se muestra un diagrama de bloques del Hardware del equipo
y se explicarán las conexiones para cada uno de los componentes.

<!-- ![Componentes presentes en la placa.](img/explotada.png){width=".5 \\textwidth"} -->
<p align=center><img src=img/explotada.png></p>

### Display LCD 16x2

| Nucleo | Display |
| --------| ---------|
| 5V     | VSS     |
| 5V     | VDD     |
| B12    | RS      |
| GND    | RW      |
| A11    | E       |
| \-     | D0      |
| \-     | D1      |
| \-     | D2      |
| \-     | D3      |
| PB5    | D4      |
| PB4    | D5      |
| PB13   | D6      |
| PB14   | D7      |
| GND    | K       |

  : Conexiones del Display

El pin A se conecta a 5V a través de una resistencia de $1 k\Omega$
mientras que el pin _Vo_ se conecta a _5V_ y _GND_ a través de un
potenciometro de $100k\Omega$.

### Sensor MAX30102

| Nucleo | Sensor MAX30102 |
| --------| -----------------|
| GND    | GND             |
| 3.3V   | VCC             |
| PB6    | SCL             |
| PB7    | SDA             |

  : Conexiones del sensor MAX30102

### Módulo bluetooth HM-10

| Nucleo | HM-10 |
| --------| -------|
| GND    | GND   |
| 5V     | VCC   |
| PA3    | TXD   |
| PA2    | RXD   |

  : Conexiones del módulo bluetooth HM-10

### Buzzer

| Nucleo | Buzzer |
| --------| --------|
| GND    | \-     |
| PC7    | \+     |

  : Conexiones del sensor MAX30102

### Botones

| Nucleo | BOTON       |
| --------| -------------|
| PC10   | SWITCH-MODE |
| PC11   | ALARM-OFF   |

  : Conexiones de los botones

Siendo SWITCH-MODE y ALARM-OFF los botones encargados de cambiar el modo
de uso y apagar la alarma respectivamente, ambos configurados en modo
pull-up y conectados a GND del otro extremo.

### LEDs

Para polarizar los LEDs con una fuente de 3.3V, las resistencias
limitadoras se calculan mediante la siguiente fórmula:
$$R = \frac{V_{CC}- V_f}{I_f}$$

-   $V_{CC}$ la tensión de alimentación (en este caso 3.3V).

-   $I_f$ la corriente deseada por el LED(normalmente se utiliza entre
    5ma y 10ma).

-   $V_f$ la tensión directa del LED.

Dado que se utilizan LEDs de diferentes colores el valor de $V_f$
dependerá de la caída de tensión típica según el color:

-   LED rojo $\longrightarrow$ indica estado de peligro junto con la
    alarma.

-   LED azul $\longrightarrow$ indica que la conexión bluetooth a sido
    exitosa.

-   LED verde $\longrightarrow$ modo adulto.

-   LED amarillo $\longrightarrow$ modo niño.

| Color LED | $V_f$ típico |
| -----------| --------------|
| Rojo      | 2V           |
| Amarillo  | 2.1V         |
| Verde     | 2.2V         |
| Azul      | 3.1V         |

  : Caída de tensión típica según el color

Se tomó como valor deseado de corriente $I_f = 10mA$. Los valores de las
resistencias utilizadas para los LEDs amarillo, verde y rojo son de
$R = 220 \Omega$ y $R= 30\Omega$ para el LED azul, conectados como se
observa en la figura [7](#fig:LEDs){reference-type="ref"
reference="fig:LEDs"}.

<figure id="fig:LEDs">
<div class="circuitikz">
<p>(-1,1) node<span>3,3V</span> (-.5,1) node[circ] – (0,1) to [led]
(2,1) to [R] (4,1) – (4.5,1) – ++(0,-1) node[tlground];</p>
</div>
<figcaption>Conexión de los LEDs.</figcaption>
</figure>

| Nucleo | LED           |
| --------| ---------------|
| PB10   | LED-Alarm     |
| PB11   | LED-Bluetooth |
| PB2    | LED-ADULT     |
| PB1    | LED-KID       |

  : Conexión de los LEDs

## Costo de componentes

| Componente   | Costo |
| --------------| -------|
| MAX30102     |       |
| HM10         |       |
| Display      |       |
| Resistencias |       |
| LEDs         |       |
| Buzzer       |       |
| Botones      |       |

  : Costos de componentes

## Firmware
<figure>
<p><span style="background-color: kicadpcbbg"><img src="img/softwaremodules.jpeg"
alt="image" /></span></p>
<figcaption>Diagrama de modulos de Software.</figcaption>
</figure>maqes
<figure>
<p><span style="background-color: kicadpcbbg"><img src="img/diagramaarchivos.jpeg"
alt="image" /></span></p>
<figcaption>Diagrama de archivos .h y .c.</figcaption>
</figure>

| Módulo               | Funcionalidad                                              | Rol        |
| ----------------------| ------------------------------------------------------------| ------------|
| Sleep Monitor System | Coordina de entradas y configuraciones de parametros       | Sistema    |
| PPG Sensor           | Lee el sensor PPG y procesa las lecturas.                  | Subsistema |
| Buttons              | Filtra ruido de los botones.                               | Subsistema |
| Bluetooth            | Envío de datos por bluetooth.                              | Driver     |
| Display              | Exhibe mediciones por pantalla.                            | Driver     |
| Buzzer               | Advierte estado crítico del usuario.                       | Subsistema |
| LEDs                 | Indican modo de uso y parámetros fuera de rangos normales. | Subsistema |


### Sensor

Este módulo representa un sensor que se comunica con un oxímetro de
pulso (*MAX30102*) y procesa la señal para obtener los datos en la
siguiente estructura de datos.

```C
        typedef struct {
            uint8_t heart_rate;
            uint8_t respiratory_rate;
            uint8_t spo2;
            uint8_t apnea;
            uint32_t timestamp;
        } task_sensor_results_dta_t;
```

La comunicación se realiza via $I^2C$ y en cada iteración se acceden los
datos al buffer FIFO del MAX30102 para reconstruir la señal. Una vez
procesada, cada `DEL_SEN_MAX_TICKS` se accede a la interfaz del sistema
y se actualiza la estructura de datos, además de activar una variable
booleana indicando que el valor no ha sido leído.

### Actuador

Consiste en los indicadores al usuario. Consta de 4 estados. Pueden
estar prendidos, apagados, parpadeando o emtiendo un pulso. Éstos
últimos permiten ajustar la frecuencia con la que un led parpadea, o la
frecuencia de un buzzer.

### Botones

Modela el comportamiento de un pulsador mecanico. Permite filrar por
software el ruido durante los cambios de estado del botón
(_debouncing_). Aunque el botón es mecánico, se puede modelar otras
entradas digitales con ruido de la misma manera.

### Sistema

Es la tarea principal encargada de recibir las entradas desde los
botones y sensores y producir los efectos correspondientes en los otros
módulos. Como es relevante no perder ninguna entrada, cuenta con una
interfaz que gestiona una cola de eventos. Ésta, además, es donde el
sensor guarda los datos que toma cada cierta cantidad de ticks. En cada
iteración, el sistema pregunta a la cola si hay algun evento disponible
y si hay algun dato para enviar. En caso de haber un dato disponible del
sensor, lo envía por UART al HM10, que se encarga de transmitirlo por
bluetooth, y se muestra por el display

| Nombre del elemento | Tipo | Funcionalidad |
| ---------------------| ------| ---------------|
<figcaption>Objetos y variables del módulo X.

<figure>
<p><span style="background-color: kicadpcbbg"><img src="img/statechart_system.png"
alt="image" /></span></p>
<figcaption>Máquina de estados del Sistema.</figcaption>
</figure>

Como se puede observar en la maquina de estados del sistema, el mismo se encarga de controlar el funcionamiento general del dispositivo. Se gestionan el modo modo de funcionamiento como también la lectura de sensores y encendido o apagado de alarmas. 
Cuenta con dos modos de funcionamiento: KID y ADULT, los cuales se seleccionan mediante el evento AV_SYS_BTN_MODE_PRESSED, correspondiente a la presión del botón de modo. Por default el sistema se inicializa en modo KID. El cambio de modo viene acompañado por el encendido del led correspondiente (ID_LED_KID O ID_LED_ADULT según el caso) asi como también del seteo de los parámetros correspondientes a ese modo. 
Mediante el evento EV_SYS_SEN_READ se realiza la lectura de los sensores. Los datos obtenidos se muestran por display y además se envían a través de bluetoth.
A partir de los datos medidos, el sistema evalúa distintas condiciones de riesgo. Si se detecta  una situación de apnea, se activa el buzzer y el led de alarma. Asimismo, si alguno de los parametros fisiologicos se encuantra fuera de los valores normales el sistema también enciende el LED de alarma para advertir al usuario. 
Finalmente, el usuario puede desactivar manualmente la alarma mediante el botón correspondiente, generando el evento EV_SYS_BTN_ALARM_PRESSED, el cual apaga el LED de alarma y el Buzzer. 
<p><span style="background-color: kicadpcbbg"><img src="img/statechart_sensor.png"
alt="image" /></span></p>
<figcaption>Máquina de estados del Sensor.</figcaption>
</figure>
Inicialmente el sistema comienza en el estado ST_SEN_IDLE, en el cual el sensor se encuentra en resposo esperando que el usuario coloque el dedo sobbre el sensór. Cuando se detecta la presencia del dedo, se genera el evento EV_SEN_FINGER_IN, esto provoca la transición al estado ST_SEN_DETECTING, en este estado se inicia un tick que permite verificar que la señal del sensor sea estable antes de comenzar a medir. 
Mientras el sistema permanece en ST_SEN_DETECTING, el temporizador se va decrementando. Si el dedo permanece colocado el tiempo mínimo requerido DEL_SEN_XX_MIN, el sistema pasa al estado ST_SEN_ACTIVE. En este estado el sensor realiza el procesamiento de la señal PPG con la función ppg_compute(). 
Si durante el funcionamiento el usuario retira el dedo del sensor, se genera el evento EV_SEN_FINGER_OUT, provocando una transición al estado ST_SEN_LOSING. Si el dedo permanece fuera del sensor durante un tiempo establecido, el sistema vuelve a ST_SEN_IDLE, a la espera de una nueva medición. 
<figure>
<p><span style="background-color: kicadpcbbg"><img src="img/statechart_button.png"
alt="image" /></span></p>
<figcaption>Máquina de estados de Botones.</figcaption>
</figure>
La función principal de esta máquina de estados es detectar de forma segura la presión y liberación de los botones evitando lecturas incorrectas por rebotes mecánicos. 
El sistema comienza en estado ST_BTN_XX_UP, el cual representa la condición en la cual el botón se encuentra sin presionar. Cuando se detecta una transición del botón hacia el estado presionado(EV_BTN_XX_DOWN), EL SISTEMA PASA AL ESTADO ST_BTN_XX_FALLING e inicia un tick. Dicho estado se utiliza para verificar que la presión del botón sea estable y no corresponda a un rebote. 
Mientras el sistema permanece en ST_BTN-XX_FALLING, el temporizador se decrementa, si continua presionado durante un tiempo mínimo se considera que la presión fue válida y el sistema pasa a ST_BTN-XX_DOWN. En ese momento se genera el evento EV_SYS_BTN_XX_PRESSED, que es enviado al sistema principal para indicar que el botón fue efectivamente presionado.
Cuando el usuario libera el botón (EV_BTN_XX_UP), el sistema pasa al estado ST_BTN_XX_RISING, donde nuevamente se utiliza el temporizador para confirmar que la liberación del botón sea estable.
Si el botón permanece liberado durante el tiempo mínimo establecido, el sistema retorna al estado ST_BTN_XX_UP y se genera el evento EV_SYS_BTN_XX_RELEASED, indicando al sistema que el botón fue liberado correctamente.
<figure>
<p><span style="background-color: kicadpcbbg"><img src="img/statechart_actuator.png"
alt="image" /></span></p>
<figcaption>Máquina de estados Actuador.</figcaption>
</figure>

## Diseño

Se utilizó *KiCAD* durante la etapa de diseño, para preparar la pcb del
dispositivo.

<figure>
<p><span style="background-color: kicadpcbbg"><img src="img/tp3.png"
alt="image" /></span></p>
<figcaption>Diseño de la PCB en KiCAD.</figcaption>
</figure>

La disposición de la placa fue pensada para soldarse en una placa
experimental. Se representaron los cables como pistas y se tuvo en
cuenta el espaciado de los agujeros.

<figure id="renders">
<p><img src="img/render_side.png" alt="image" /> <img src="img/render.png"
alt="image" /></p>
<figcaption>Previsualización 3D de la placa.</figcaption>
</figure>

Se cuenta con espacio suficiente para agregar una batería para hacer al
dispositivo portable. Considerando que solo se precisa el integrado y el
uso de una pantalla con menor tamaño y similar o mejor resolución queda
margen para un modelo más compacto.

## Ensamblaje


# Ensayos y resultados

## Pruebas funcionales del hardware

Con el objetivo de verificar el correcto funcionamiento de los
componentes físicos del sistema, se realizaron diversas pruebas
funcionales sobre el hardware desarrollado. Estas pruebas permitieron
validar la correcta operación de los módulos utilizados, así como la
integridad de las conexiones eléctricas entre los distintos dispositivos
que componen el sistema.

### Metodología de ensayo

Las pruebas se realizaron verificando individualmente cada uno de los
módulos del sistema. En primer lugar, se comprobó el correcto suministro
de alimentación al circuito mediante una fuente USB, verificando las
tensiones presentes en los distintos puntos del sistema.

Posteriormente, se evaluó la comunicación I2C con el sensor MAX30102,
confirmando la correcta lectura de los registros del dispositivo y la
recepción de datos provenientes de la FIFO del sensor. Para validar el
funcionamiento del sensor óptico, se colocó el módulo en el dedo del
usuario y se observó la variación de la señal PPG adquirida.

Asimismo, se verificó el funcionamiento del módulo Bluetooth HM-10
mediante la transmisión de datos hacia una aplicación externa,
confirmando la correcta comunicación UART entre el microcontrolador y el
módulo inalámbrico.

Finalmente, se probaron los dispositivos de salida del sistema,
incluyendo los indicadores LED, el buzzer de alarma y el display,
verificando su activación ante distintos estados del sistema.

### Resultados obtenidos

Durante los ensayos se comprobó el correcto funcionamiento de los
distintos módulos de hardware. La Tabla
[3](#tab:hardware_tests){reference-type="ref"
reference="tab:hardware_tests"} resume los resultados obtenidos en las
pruebas realizadas.

::: {#tab:hardware_tests}
| Componente      | Prueba realizada               Resultado            |
| -----------------| -----------------------------------------------------|
| Alimentación    | Verificación de tensión de alimentación    Correcto |
| Sensor MAX30102 | Comunicación I2C y lectura de registros    Correcto |
| Sensor MAX30102 | Adquisición de señal PPG           Correcto         |
| Bluetooth HM-10 | Comunicación UART y transmisión de datos   Correcto |
| LEDs            | Encendido según estado del sistema      Correcto    |
| Buzzer          | Activación ante condición crítica       Correcto    |
| Display         | Visualización de parámetros fisiológicos   Correcto |

  : Resultados de las pruebas funcionales del hardware.
:::

![Led indicador de modo encendido.](img/Actuador.png){#fig:actuador
width="0.6 \\linewidth"}

![Tensión de alimentacion del
MAX30102.](img/5,5V.jpeg){width=".5 \\textwidth"}

### Análisis de resultados

Los resultados obtenidos muestran que todos los módulos de hardware del
sistema funcionan correctamente y que las conexiones entre los distintos
dispositivos permiten una operación adecuada del sistema. La
comunicación con el sensor MAX30102 se realizó sin inconvenientes,
permitiendo la adquisición de las señales necesarias para el cálculo de
los parámetros fisiológicos.

Asimismo, la transmisión de datos mediante el módulo Bluetooth HM-10 se
llevó a cabo de forma estable, y los dispositivos de salida (LEDs,
buzzer y display) respondieron correctamente ante los diferentes estados
del sistema.

En base a estas pruebas, se concluye que el hardware del sistema se
encuentra en condiciones adecuadas para la ejecución del firmware y la
realización de las pruebas funcionales del software.

## Pruebas funcionales del firmware

Con el objetivo de verificar el correcto funcionamiento del firmware
desarrollado, se realizaron distintos ensayos funcionales sobre el
sistema completo.

Durante los ensayos se evaluaron las principales funcionalidades del
sistema, incluyendo la adquisición de señales, el procesamiento de datos
fisiológicos, la detección de eventos críticos y la transmisión de
información.

### Metodología de ensayo

Las pruebas se realizaron colocando el sensor óptico en el dedo del
usuario, permitiendo la adquisición de las señales infrarroja y roja
necesarias para los distintos calculos. El firmware ejecutó de manera
periódica las tareas de lectura del sensor, filtrado de la señal,
cálculo de parámetros fisiológicos y transmisión de los datos obtenidos.

Adicionalmente, se verificó el correcto funcionamiento de los
indicadores visuales y sonoros ante la detección de eventos críticos,
como disminuciones significativas en la saturación de oxígeno o
irregularidades en el patrón respiratorio y/o cardiaco.

### Resultados obtenidos

Durante los ensayos se registraron los valores de $SpO_{2}$ y frecuencia
respiratoria estimados por el sistema. La Tabla
[4](#tab:resultados_prueba){reference-type="ref"
reference="tab:resultados_prueba"} presenta un conjunto representativo
de las mediciones obtenidas.

::: {#tab:resultados_prueba}
| Muestra | $SpO_{2}$ (%) | Frecuencia respiratoria (rr) | Frecuencia Cardiaca (hr) | Apnea |
| ---------| ---------------| ------------------------------| --------------------------| -------|
| 1       | 97            | 20                           | 61                       | False |
| 2       | 98            | 21                           | 61                       | False |
| 3       | 95            | 20                           | 56                       | False |
| 4       | 94            | 23                           | 53                       | False |
| 5       | 95            | 22                           | 55                       | False |

  : Ejemplo de resultados obtenidos durante las pruebas funcionales del
  sistema.
:::

![Captura parametros medidos mediante el debugger del
IDE.](img/parameters.jpeg){#fig:debuger width="0.6 \\linewidth"}

![Deteccion exitososa del modulo HM-10 via
Bluetooth](img/Zblutuconecting.png){#fig:blutuconecting
width="0.4 \\linewidth"}

### Análisis de resultados

Los resultados obtenidos durante los ensayos muestran que el sistema es
capaz de adquirir y procesar correctamente las señales provenientes del
sensor óptico, permitiendo estimar los datos fisiologicos deseados.

Además, se verificó el correcto funcionamiento del sistema de
comunicación y de los mecanismos de alerta implementados. En general, el
comportamiento del firmware resultó consistente con los requerimientos
funcionales definidos previamente.

## Pruebas de integración

::: {#tab:casos_uso}
| **\#** | **Caso de uso**                                                                                                                                                        | **Cumplido** |
| --------| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------| --------------|
| 1      | El usuario inicia el monitoreo del sueño y el dispositivo comienza a adquirir señales PPG del sensor MAX30102 para calcular los datos fisiológicos.                    | $\checkmark$ |
| 2      | El sistema monitorea continuamente los parámetros fisiológicos durante el sueño y transmite los valores de $SpO_{2}$ y respiración a la aplicación mediante Bluetooth. | $\checkmark$ |
| 3      | El sistema detecta una disminución crítica de $SpO_{2}$ o un episodio de apnea y activa una alarma sonora y visual para alertar al usuario.                            | $\checkmark$ |
| 4      | El usuario puede visualizar en tiempo real los parámetros fisiológicos en la aplicación o en el display del dispositivo.                                               | $\checkmark$ |

  : Evaluación de los casos de uso del sistema de detección de apnea y
  monitoreo de $SpO_{2}$.
:::

![Datos fisiológicos enviados a través de
Bluetooth](img/zdatosblutu.png){#fig:datosblutu width="0.6 \\linewidth"}

![Datos fisiológicos enviados a través del
Display](img/Datosdisplay.jpeg){#fig:datosdisplay
width="0.6 \\linewidth"}

## Cumplimiento de requisitos 

::: {#tab:tab_requerimientos2}
| Grupo         | ID                     | Requerimiento Funcional                                                                         | Descripción                                                  | Cumplido |
| ---------------| ------------------------| -------------------------------------------------------------------------------------------------| --------------------------------------------------------------| ----------|
| Adquisición   | 1.1                    | Adquisición de señales                                                                          | Capturar señales ópticas ROJO/IR del MAX30102 vía I2C.       |          |
| 1.2           | Lectura periódica      | Obtener muestras desde la FIFO del sensor.                                                      |                                                              |          |
| 1.3           | Análisis de variación  | Detectar variaciones relevantes para el cálculo de $SpO_{2}$ y respiración.                     |                                                              |          |
| Procesamiento | 2.1                    | Filtrado digital                                                                                | Aplicar filtrado digital para reducir ruido en la señal PPG. |          |
| 2.2           | Cálculo de $SpO_{2}$   | Calcular $SpO_{2}$ mediante el método del ratio R.                                              |                                                              |          |
| 2.3           | Cálculo de respiración | Estimar la frecuencia respiratoria mediante análisis de envolvente o variaciones de línea base. |                                                              |          |
| 2.4           | Empaquetado de datos   | Generar paquetes de datos con $SpO_{2}$, respiración y estado del sistema.                      |                                                              |          |
| Indicadores   | 3.1                    | Alarma crítica                                                                                  | Activar alarma sonora cuando se detecta apnea.               |          |
| 3.2           | Indicadores LED        | Mostrar estados (crítico, normal, error) mediante LEDs.                                         |                                                              |          |
| Comunicación  | 4.1                    | Bluetooth                                                                                       | Transmitir datos vía HM-10 utilizando UART.                  |          |
| 4.2           | Envío de parámetros    | Enviar $SpO_{2}$, respiración y estado del paciente a la aplicación.                            |                                                              |          |
| 4.3           | Registro de eventos    | Informar alarmas, desconexiones o estados especiales.                                           |                                                              |          |
| 4.4           | Display                | Mostrar parámetros fisiológicos relevantes en pantalla.                                         |                                                              |          |
| Alimentación  | 5.1                    | Alimentación                                                                                    | Alimentación del sistema mediante conexión USB.              |          |
| 5.2           | Bajo consumo           | Entrar en modo Sleep cuando no hay actividad.                                                   |                                                              |          |
| 5.3           | Reactivación           | Retomar actividad ante interrupciones o comandos.                                               |                                                              |          |

  : Requerimientos funcionales del sistema de detección de apnea y
  monitoreo de $SpO_{2}$.
:::

## 4.4 Medición y análisis de consumo

### Procedimiento realizado

La placa NUCLEO-F103RB permite medir el consumo del microcontrolador utilizando el jumper **JP6 (IDD)**.

Según el manual de usuario de la placa, el jumper JP6 puede retirarse para insertar un amperímetro en serie y medir la corriente consumida por el microcontrolador.

Funcionamiento del jumper:

| Estado del jumper | Descripción |
|---|---|
| JP6 conectado | El microcontrolador recibe alimentación normalmente |
| JP6 removido | Se puede conectar un amperímetro para medir el consumo |

Para realizar la medición se retira el jumper JP6 y se conecta un miliamperímetro entre los dos pines del conector. De esta manera se obtiene la corriente consumida por el STM32 durante la ejecución de la aplicación.

La potencia consumida se estimó mediante la expresión:

P = V * I

donde:

- **V** es la tensión de alimentación del sistema.
- **I** es la corriente medida en la línea de alimentación.


Esta medición permite analizar el consumo energético del sistema y evaluar el impacto de los modos.


---

### Modos de operación medidos

Se realizaron mediciones del consumo del sistema bajo diferentes condiciones de funcionamiento, representativas del uso normal del dispositivo.

| Modo de operación | I pico @5V [mA] | P pico @5V [mW] | Observaciones |
|------------------|----------------|---------------|--------------|
| Sistema esperando dedo | 10,8 | 54 | Menor consumo, no se procesan datos |
| Sistema sensanso datos | 28,6 | 143 | |
| Sistema transmitiendo datos via Bluetooth | 32,4| 162 | |
| Sistema en estado de alarma (buzzer + LED) | 42,8| 214 | Mayor consumo, proceso de datos y actuadores |

**Tabla X — Consumo total medido del jumper J6 en distintos modos de operación.**

---

### Alcance de la medición

La medición realizada representa el **consumo total del sistema a la tensión de entrada de 5 V**.  
Los subsistemas alimentados a **3,3 V** quedan incluidos indirectamente en esta medición, dado que dicha tensión es generada mediante el regulador de la propia placa.

Por lo tanto, la corriente medida corresponde al consumo global del dispositivo completo en cada modo de funcionamiento.

---

El análisis de los resultados permite determinar el **consumo máximo del sistema**, correspondiente al caso del sistema en estado de alarma.


## 4.5 Medición y análisis de tiempos de ejecución (WCET)

### Metodología aplicada

Con el objetivo de evaluar el desempeño temporal del sistema, se realizó un análisis del **tiempo de ejecución de las tareas principales**, determinando su **Worst Case Execution Time (WCET)**, es decir, el mayor tiempo observado durante múltiples ejecuciones.

La medición se realizó utilizando el **contador de ciclos del procesador (DWT – Data Watchpoint and Trace)** disponible en los microcontroladores ARM Cortex-M. Este contador permite registrar con alta precisión el número de ciclos de reloj transcurridos durante la ejecución de una sección de código.

El procedimiento consiste en:

1. Leer el valor del contador de ciclos antes de ejecutar la tarea.
2. Ejecutar la tarea o función a medir.
3. Leer nuevamente el contador al finalizar la ejecución.
4. Calcular la diferencia entre ambas lecturas para obtener el tiempo de ejecución.

El tiempo medido en ciclos se convierte a unidades de tiempo mediante la frecuencia de reloj del microcontrolador:

t = ciclos / f_CPU

donde:

- **ciclos**: número de ciclos de reloj medidos.
- **f_CPU**: frecuencia de operación del procesador.

---

## Tareas analizadas

Se midieron los tiempos de ejecución de las principales tareas del sistema, responsables de la adquisición de datos, procesamiento de señal y comunicación.

| Tarea | Archivo asociado | Descripción |
|------|------|------|
| task_sensor | task_sensor.c | Adquisición de datos desde el sensor |
| task_system | task_system.c | Control general del sistema |
| task_actuator | task_actuator.c | Control de actuadores (LED, buzzer) |
| task_button | task_button.c | Lectura de botones |

**Tabla X — Tareas principales del sistema analizadas para WCET.**

## Resultados de medición

Los tiempos de ejecución se obtuvieron ejecutando cada tarea múltiples veces durante el funcionamiento normal del sistema y registrando el **máximo valor observado**.


| Tarea | WCET [µs] | Período [ms] | Utilización |
|------|------|------|------|
| task_button | 22| 1 | 0.022|
| task_sensor | 879|1 | 0.879 |
| task_system |2536 |3000 |0.000845|
| task_actuator |30|1 | 0.03 |

**Tabla X — Tiempos de ejecución medidos para las tareas del sistema.**

Como se observa en la Tabla X, para el análisis de la tarea `task_system` se utilizó su período real de ejecución (determinado por la cadencia de actualización de datos en la pantalla LCD, cada 3000 ms) en lugar del período de *polling* del planificador base (1 ms). Asumir la tasa de *polling* para una rutina asíncrona de baja frecuencia resultaría en un error analítico y en una sobreestimación matemática del factor de uso.

## 4.6 Cálculo del Factor de Uso (U) de la CPU

Con el objetivo de analizar la carga computacional del sistema, se estimó el **factor de uso del procesador (U)** a partir de los tiempos de ejecución de las tareas y sus respectivos períodos de activación.

El factor de uso se calcula mediante la expresión:

U = Σ (WCET_i / Periodo_i)

Dando como resultado U = 0.933 = 93% 


Este valor permite estimar qué fracción del tiempo total de CPU es utilizada por el conjunto de tareas del sistema.

U = 0.933 = 93 %

---

### Interpretación del resultado

El valor total obtenido para **U** representa la fracción de tiempo en la cual el procesador se encuentra ejecutando tareas activas.

- Si **U < 1**, el sistema es temporalmente viable, ya que el procesador puede completar todas las tareas dentro de sus plazos.
- Si **U ≪ 1**, el procesador permanece ocioso durante una fracción significativa del tiempo.

En este sistema, se observa que el valor de **U** (93.3%) se encuentra muy cercano al límite de la capacidad total del procesador. Esto indica que la CPU se mantiene fuertemente exigida y el margen de maniobra ante interrupciones asíncronas es mínimo. 

Como consecuencia directa de esta alta carga computacional, se descarta la implementación de modos de bajo consumo (como *Sleep* o *Wait For Interrupt*). Dado que el tiempo ocioso remanente es de apenas ~7%, se estaria aportando un ahorro energético insignificante frente al riesgo de inestabilidad del sistema.

---

## Consola

Adjuntamos Captura de pantalla de **Console & Build Analyzer** luego de
compilar la versión final

![\"Console and Build Analyzer\"](explotada){#fig:console
width="0.6 \\linewidth"}

## Documentación del desarrollo realizado

En la Tabla [7](#tab:project_summary){reference-type="ref"
reference="tab:project_summary"} se presenta un conjunto de elementos
que resumen la información más relevante acerca del diseño y la
implementación del sistema propuesto para la detección de apneas y
variaciones en la saturación de oxígeno durante el sueño.

A partir del análisis de estos elementos, es posible comprender los
principales aspectos del proyecto, incluyendo los objetivos del sistema,
su arquitectura de hardware y software, así como los resultados
obtenidos durante la etapa de implementación y evaluación..

::: {#tab:project_summary}
| **Elemento**                                                                | **Referencia** |
| -----------------------------------------------------------------------------| ----------------|
| Justificación y objetivos del proyecto                                      | Sección 2      |
| Requisitos funcionales y no funcionales del sistema                         | Sección 3      |
| Casos de uso del sistema                                                    | Sección 3      |
| Arquitectura de hardware del dispositivo                                    | Sección 4      |
| Diagrama de conexiones entre los componentes del sistema                    | Sección 4      |
| Lista de materiales (BOM)                                                   | Sección 4      |
| Arquitectura del software del sistema embebido                              | Sección 5      |
| Definición de módulos de software (funciones, variables y tareas)           | Sección 5      |
| Algoritmos de procesamiento de señales (detección de respiración y SpO$_2$) | Sección 5      |
| Implementación del sistema                                                  | Sección 6      |
| Evaluación del cumplimiento de los requisitos del sistema                   | Sección 7      |
| Evaluación de los casos de uso                                              | Sección 7      |
| Propuestas de mejoras o trabajos futuros                                    | Sección 8      |
| Conclusiones finales                                                        | Sección 9      |

  : Elementos que resumen la información más importante del sistema de
  detección de apnea y monitoreo de SpO$_2$.
:::

# Conclusiones

Se verificaron

## Resultados

## Proximos pasos

