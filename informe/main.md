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

## Análisis de sistemas similares al desarrollado

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

![Oxímetro de pulso](oximetro.png){#fig:oximetro width="40%"}

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
<p><img src="ppg" alt="image" /> <img src="ppg2" alt="image" /></p>
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

## Requerimientos funcionales

::: {#tab:tab_requerimientos}
       Grupo       ID    Requerimiento Funcional                             Descripción
  --------------- ----- ------------------------- -----------------------------------------------------------------
    Adquisición    1.1   Adquisición de señales        Capturar señales ópticas ROJO/IR del MAX30102 via I2C.
                   1.2      Lectura periódica                Obtener muestras desde la FIFO del sensor.
                   1.3    Análisis de variación    Detectar variaciones innecesarias para $SpO_{2}$ y respiración
   Procesamiento   2.1      Filtrado digital                              Aplicar filtrado
                   2.2    Cálculo de $SpO_{2}$                 Calcular $SpO_{2}$ mediante el ratio R
                   2.3   Cálculo de respiración               Estimar frecuencia respiratoria mediante
                   2.3   Cálculo de respiración                 análisis de envolvente o línea base.
                   2.4    Empaquetado de datos          Generar paquete con $SpO_{2}$ , respiración y estado.
    Indicadores    3.1       Alarma crítica                Activar alarma sonora cuando se detecta apnea.
                   3.2       Indicadores LED           Mostrar estados (crítico, normal, error) mediante LEDs.
   Comunicación    4.1          Bluetooth                       Transmitir datos vía HM-10 por UART.
                   4.2     Envío de parámetros     Enviar $SpO_{2}$, respiraciones y estado del paciente a la app.
                   4.3     Registro de eventos          Informar alarmas, desconexiones o estados especiales.
                   4.4           Display                     Mostrar parámetros de interes por pantalla.
   Alimentación    5.1        Alimentación                              Alimentación vía USB
                   5.2        Bajo consumo                  Entrar en modo Sleep cuando no hay actividad.
                   5.3        Reactivación                  Retomar actividad ante movimiento o comando.

  : Tabla de requerimientos
:::

## Casos de uso

## Descripción de módulos externos utilizados

-   Display LCD 16x2

-   Sensor MAX30102

-   Módulo Bluetooth HM-10

### Display LCD 16x2

El display utilizado es un display LCD de 16x2 como se puede observar en
la figura [4](#fig:LCD){reference-type="ref" reference="fig:LCD"}.
Dichas conexiones se realizaron con una configuración de 4 bits. El
mismo efectúa la comunicación con la placa mediante pines *GPIO*.

![Módulo 1602A](lcd_display.png){#fig:LCD width="50%"}

### HM-10

Para la transmisión de los datos hacia el dispositivo móvil del usuario
se emplea un módulo Bluetooth HM-10. La comunicación entre el módulo
Bluetooth y la placa de desarrollo se realiza usando la interfaz UART. A
través de esta interfaz serie, el microcontrolador transmite al módulo
los parámetros de interés para su posterior envío al dispositivo móvil
del usuario.

![Módulo bluetooth HM-10](HM10.png){#fig:HM10 width="50%"}

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

![Sensor MAX30102](MAX30102.png){#fig: MAX30102 width="50%"}

# Diseño e Implementación

## Hardware del sistema

A continuación se muestra un diagrama de bloques del Hardware del equipo
y se explicarán las conexiones para cada uno de los componentes.

### Display LCD 16x2

   Nucleo   Display
  -------- ---------
     5V       VSS
     5V       VDD
    B12       RS
    GND       RW
    A11        E
     \-       D0
     \-       D1
     \-       D2
     \-       D3
    PB5       D4
    PB4       D5
    PB13      D6
    PB14      D7
    GND        K

  : Conexiones del Display

El pin A se conecta a 5V a través de una resistencia de $1k\ohm$
mientras que el pin Vo se conecta a 5V y GND a través de un
potenciometro de $100k\ohm$.

### Sensor MAX30102

   Nucleo   Sensor MAX30102
  -------- -----------------
    GND           GND
    3.3V          VCC
    PB6           SCL
    PB7           SDA

  : Conexiones del sensor MAX30102

### Módulo bluetooth HM-10

   Nucleo   HM-10
  -------- -------
    GND      GND
     5V      VCC
    PA3      TXD
    PA2      RXD

  : Conexiones del módulo bluetooth HM-10

### Buzzer

   Nucleo   Buzzer
  -------- --------
    GND       \-
    PC7       \+

  : Conexiones del sensor MAX30102

### Botones

   Nucleo      BOTON
  -------- -------------
    PC10    SWITCH-MODE
    PC11     ALARM-OFF

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

   Color LED   $V_f$ típico
  ----------- --------------
     Rojo           2V
   Amarillo        2.1V
     Verde         2.2V
     Azul          3.1V

  : Caída de tensión típica según el color

Se tomó como valor deseado de corriente $I_f = 10mA$. Los valores de las
resistencias utilizadas para los LEDs amarillo, verde y rojo son de
$R = 220 \ohm$ y $R= 30\ohm$ para el LED azul, conectados como se
observa en la figura [7](#fig:LEDs){reference-type="ref"
reference="fig:LEDs"}.

<figure id="fig:LEDs">
<div class="circuitikz">
<p>(-1,1) node<span>3,3V</span> (-.5,1) node[circ] – (0,1) to [led]
(2,1) to [R] (4,1) – (4.5,1) – ++(0,-1) node[tlground];</p>
</div>
<figcaption>Conexión de los LEDs.</figcaption>
</figure>

   Nucleo        LED
  -------- ---------------
    PB10      LED-Alarm
    PB11    LED-Bluetooth
    PB2       LED-ADULT
    PB1        LED-KID

  : Conexión de los LEDs

## Costo de componentes

    Componente    Costo
  -------------- -------
     MAX30102    
       HM10      
     Display     
   Resistencias  
       LEDs      
      Buzzer     
     Botones     

  : Costos de componentes

## Firmware

### Sensor {#sec:sensor}

Este módulo representa un sensor que se comunica con un oxímetro de
pulso (*MAX30102*) y procesa la señal para obtener los datos en la
siguiente estructura de datos.

        typedef struct {
            uint8_t heart_rate;
            uint8_t respiratory_rate;
            uint8_t spo2;
            uint8_t apnea;
            uint32_t timestamp;
        } task_sensor_results_dta_t;

La comunicación se realiza via $I^2C$ y en cada iteración se acceden los
datos al buffer FIFO del MAX30102 para reconstruir la señal. Una vez
procesada, cada `DEL_SEN_MAX_TICKS` se accede a la interfaz del sistema
y se actualiza la estructura de datos, además de activar una variable
booleana indicando que el valor no ha sido leído.

### Actuador {#sec:actuador}

Consiste en los indicadores al usuario. Consta de 4 estados. Pueden
estar prendidos, apagados, parpadeando o emtiendo un pulso. Éstos
últimos permiten ajustar la frecuencia con la que un led parpadea, o la
frecuencia de un buzzer.

### Botones {#sec:botones}

Modela el comportamiento de un pulsador mecanico. Permite filrar por
software el ruido durante los cambios de estado del botón
(*debouncing*). Aunque el botón es mecánico, se puede modelar otras
entradas digitales con ruido de la misma manera.

### Sistema {#sec:sistema}

Es la tarea principal encargada de recibir las entradas desde los
botones y sensores y producir los efectos correspondientes en los otros
módulos. Como es relevante no perder ninguna entrada, cuenta con una
interfaz que gestiona una cola de eventos. Ésta, además, es donde el
sensor guarda los datos que toma cada cierta cantidad de ticks. En cada
iteración, el sistema pregunta a la cola si hay algun evento disponible
y si hay algun dato para enviar. En caso de haber un dato disponible del
sensor, lo envía por UART al HM10, que se encarga de transmitirlo por
bluetooth, y se muestra por el display

## Diseño {#sub:diseño}

Se utilizó *KiCAD* durante la etapa de diseño, para preparar la pcb del
dispositivo.

<figure>
<p><span style="background-color: kicadpcbbg"><img src="tp3.png"
alt="image" /></span></p>
<figcaption>Diseño de la PCB en KiCAD.</figcaption>
</figure>

La disposición de la placa fue pensada para soldarse en una placa
experimental. Se representaron los cables como pistas y se tuvo en
cuenta el espaciado entre orificios.

<figure id="fig:">
<p><img src="render_side.png" alt="image" /> <img src="render.png"
alt="image" /></p>
<figcaption>Previsualización 3D de la placa.</figcaption>
</figure>

Se cuenta con espacio suficiente para agregar una batería para hacer al
dispositivo portable. Considerando que solo se precisa el integrado y el
uso de una pantalla con menor tamaño y similar o mejor resolución queda
margen para un modelo más compacto.

en conclusiones tal vez?

## Ensamblaje {#sub:ensamblaje}

# Ensayos y resultados {#sec:ensayos_y_resultados}
