# Sistema de detección de apneas y variaciones de oxigenación durante el sueño
Trabajo Final - Taller de Sistemas Embebidos - FIUBA

|Alumno|Padron|
|------|-------|
|Tomás Musso Carboni|111548|
|Santiago Archetti|xxx xxx|
|Alan Romero|xxx xxx|


## 1.1 Justificación de la elección del proyecto
Este proyecto se eligió porque los problemas respiratorios durante el sueño, como la apnea<sup>[1]</sup>, son muy comunes y muchas veces no se detectan a tiempo. La falta de diagnóstico puede generar cansancio crónico, bajo rendimiento y riesgos cardiovasculares, pero los estudios clínicos tradicionales suelen ser costosos y difíciles de realizar en el hogar. Por eso, un dispositivo portátil que mida la respiración y la saturación de oxígeno durante la noche resulta una herramienta útil y accesible para identificar posibles alteraciones.
Desde el punto de vista tecnológico, el proyecto es adecuado para el área de sistemas embebidos porque integra sensores reales, procesamiento digital, comunicación inalámbrica y manejo de bajo consumo. Además, el hardware seleccionado (MAX30102 y STM32F302R8) es económico, fácil de conseguir y cuenta con buena documentación, lo que facilita el desarrollo y permite enfocarse en el funcionamiento del sistema sin una complejidad excesiva. En conjunto, esto hace que el proyecto sea realizable, educativo y al mismo tiempo relevante desde lo biomédico.

## 2.1 Objetivo del Proyecto
Diseñar y desarrollar un dispositivo portátil no invasivo capaz de medir en tiempo real la saturación de oxígeno (SpO₂) y la frecuencia respiratoria mediante técnicas de fotopletismografía. El objetivo es obtener señales fisiológicas confiables, procesarlas de forma local a través del microcontrolador STM32F302R8 y generar alertas inmediatas ante condiciones anómalas, garantizando un monitoreo continuo y seguro

## 2.2 Propósito del Proyecto
El propósito es brindar una herramienta accesible y de bajo consumo para el seguimiento básico de parámetros respiratorios y de oxigenación, destinada a aplicaciones clínicas simples, monitoreo domiciliario y apoyo en la detección temprana de eventos como apnea o desaturación. Se busca complementar el trabajo de profesionales de la salud y ofrecer al usuario información clara y oportuna sobre su estado fisiológico.


## 2.3 Alcance del Proyecto
El proyecto abarca el diseño electrónico, la implementación del firmware y el procesamiento digital de la señal proveniente del sensor MAX30102 para la obtención de SpO₂ y frecuencia respiratoria. Incluye la integración de alarmas locales (buzzer/LED) configuradas por umbrales, así como la transmisión inalámbrica de datos mediante Bluetooth hacia una aplicación móvil. El alcance no contempla funciones diagnósticas avanzadas ni interpretación médica, limitándose al monitoreo y notificación de parámetros básicos.
Se establece la primer mesa de feberero como fecha para finalizar el proyecto.

|Criterio|Descripción|Puntuación|Peso final|
|----------|-------------|------------|------------|
|Tiempo y facilidad de implementación. Peso (7) |El uso del sensor MAX30102 y el microcontrolador STM32F302R8 presenta una complejidad moderada, debido a la necesidad de configurar correctamente la comunicación I²C, filtrar la señal PPG y aplicar algoritmos de cálculo de SpO₂ y frecuencia respiratoria. Sin embargo, existe amplia documentación y librerías disponibles, lo que reduce las dificultades de implementación. En conjunto, se considera que la complejidad técnica es manejable dentro del marco de un proyecto de sistemas embebidos.| <p align="center">7</p> | 49 |
|Disponibilidad y costo de hardware. Peso (6) | Los componentes principales —sensor óptico MAX30102, microcontrolador STM32, módulo Bluetooth y un buzzer/LED para alarmas— son económicos, fáciles de conseguir y ampliamente utilizados en proyectos biomédicos educativos. Esto permite realizar pruebas sin un costo elevado y garantiza buena disponibilidad de reemplazos. |<p align="center">7</p>|42|
|Facilidad de realización de pruebas. Peso (4)|La verificación del funcionamiento del sistema puede realizarse mediante lectura directa de las señales PPG, monitoreo en tiempo real por UART/Bluetooth y observación de la activación de alarmas locales. Además, se pueden usar herramientas básicas como un multímetro o un osciloscopio para validar etapas eléctricas, lo que facilita enormemente el proceso de pruebas y depuración.| <p align="center">6</p>| 24 |
|Utilidad e interés personal en el proyecto. Peso (8)| El proyecto presenta un alto interés personal debido a su relación con aplicaciones biomédicas reales, la posibilidad de detectar apneas y desaturaciones durante el sueño, y su potencial mejora en versiones futuras. Además, permite aplicar conocimientos de sensores ópticos, filtrado digital y diseño embebido, lo que resulta atractivo tanto desde el punto de vista académico como práctico.|<p align="center"> 9</p>| 72 |

<p align="center"><b>Tabla 1: Evaluación de criterios del proyecto</b></p>

## 3.1 Arquitectura del sistema
El diseño se estructura en cuatro bloques funcionales: 
1.	Adquisición: Captura de la señal de flujo sanguíneo del dedo<sup>[2]</sup> del paciente mediante fotodiodos y LEDs (Rojo/IR). 
2.	Procesamiento: Filtrado digital y cálculo de algoritmos en el microcontrolador. 
3.	Interfaz: Indicadores visuales y sonoros para el usuario. 
4.	Comunicación: Transmisión inalámbrica de telemetría y almacenamiento de parámetros. 

## 3.2 Diseño de Hardware
-	Sensor MAX30102: Módulo integrado de oximetría y ritmo cardíaco. Se comunica vía I²C y opera a 3.3V. Provee las lecturas crudas de absorción de luz roja e infrarroja necesarias para calcular la saturación y derivar la respiración. 
-	Microcontrolador STM32F302R8: Seleccionado por su núcleo ARM Cortex-M4F (con unidad de punto flotante), esencial para ejecutar filtros digitales eficientes. Gestiona los buses I²C (sensor, EEPROM) y UART (Bluetooth). 
-	Módulo Bluetooth HM-10: Interfaz UART transparente para enviar datos seriales a dispositivos móviles (Smartphone/PC). 
-	Almacenamiento: Memoria EEPROM externa (serie 24C0x) para guardar configuraciones de usuario, como umbrales de alarma o logs de eventos. 
-	Gestión de Energía: Alimentación mediante batería Li-ion con regulación LDO a 3.3V. Implementa modos de bajo consumo (STOP) durante la inactividad. 

## 3.3 Procesamiento de la señal
El firmware, desarrollado en C con librerías HAL, ejecuta el siguiente flujo cíclico: 
1.	Adquisición de Datos: Lectura de la FIFO del MAX30102 para obtener muestras de los canales Rojo e Infrarrojo (IR). 
2.	Pre-procesamiento: Aplicación de un filtro para eliminar el componente de continua (DC) y ruido de alta frecuencia.<sup>[3]</sup> 
3.	Cálculo de SpO₂: Se basa en la relación de ratios ($R$) entre las componentes AC y DC de ambas longitudes de onda<sup>[4]</sup>: 
    $$ R = \frac{AC_{red}\cdot AC_{ir}}{DC_{red} \cdot Dc_{ir}}$$

    $$SpO_2 = 110 - 25 \cdot R$$
4.	Estimación de Frecuencia Respiratoria: Se realiza mediante el análisis de la variabilidad de la línea base o la modulación de amplitud (envolvente) de la señal PPG, extrayendo componentes de baja frecuencia (0.2 – 0.3 Hz). 
5.	Lógica de Control: Si el SpO₂ cae por debajo del 90%, se activa la alarma sonora. Finalmente, se empaquetan los datos y se envían por UART. 

# 4.1 Requerimientos funcionales

| Grupo  | ID   | Requerimiento Funcional    | Descripción                                                                   |
|---|------|-----------------------------|-------------------------------------------------------------------------------|
|Adquisición | 1.1  | Adquisición de señales     | Capturar señales ópticas Rojo/IR del MAX30102 vía I²C.                        |
|   | 1.2  | Lectura periódica          | Obtener muestras desde la FIFO del sensor.                                    |
|   | 1.3  | Análisis de variación      | Detectar variaciones necesarias para SpO₂ y respiración.                      |
| Procesamiento  | 2.1  | Filtrado digital           | Aplicar filtrado.                                                             |
|   | 2.2  | Cálculo de SpO₂            | Calcular SpO₂ mediante el ratio R (AC/DC Rojo y AC/DC IR).                    |
|   | 2.3  | Cálculo de respiración     | Estimar frecuencia respiratoria mediante análisis de envolvente o línea base. |
|   | 2.4  | Empaquetado de datos       | Generar paquete con SpO₂, respiración y estado.                               |
|  Indicadores | 3.1  | Alarma crítica             | Activar alarma sonora cuando SpO₂ < umbral.                                   |
|   | 3.2  | Indicadores LED            | Mostrar estados (crítico, normal, error) mediante LEDs.                       |
|  Comunicación | 4.1  | Comunicación Bluetooth     | Transmitir datos vía HM-10 por UART.                                          |
|   | 4.2  | Envío de parámetros        | Enviar SpO₂, respiraciones y estado del paciente a la app.                    |
|   | 4.3  | Registro de eventos        | Informar alarmas, desconexiones o estados especiales.                         |
|   | 4.4  | Recepción de configuración | Recibir umbrales y parámetros desde la aplicación.                            |
|  Memoria | 5.1  | Guardar configuración      | Almacenar umbrales y parámetros en EEPROM.                                    |
|   | 5.2  | Restaurar valores          | Recuperar valores predeterminados si hay error de memoria.                    |
| Alimentación  | 6.1  | Alimentación               | Funcionamiento con batería Li-Ion y regulación a 3.3V.                        |
|   | 6.2  | Bajo consumo               | Entrar en modo STOP cuando no hay actividad.                                  |
|   | 6.3  | Reactivación               | Retomar actividad ante movimiento o comando.                                  |
<p align="center"><b>Tabla 2: Requerimentos funcionales</b></p>

## 5.1 Modos de uso

| **Modo** | **Descripción** | **Cotas / Umbrales** | **Comportamiento Especial** |
|---------|-----------------|----------------------|------------------------------|
| **Adulto** | Modo estándar para usuarios adultos. Utiliza configuraciones típicas de umbral para SpO₂ y respiración. | • SpO₂ crítico: < **90%**  <br>• Frecuencia respiratoria: rangos normales adulto | • Activa alarmas ante valores críticos. <br>• Envío de datos por Bluetooth. |
| **Niño** | Modo adaptado para usuarios pediátricos, con umbrales más sensibles. | • SpO₂ crítico: < **92–93%**  <br>• Frecuencia respiratoria: rangos normales niño | • Mayor sensibilidad a variaciones pequeñas. <br>• Alarmas ajustadas a parámetros pediátricos. |
| **Sleep (Bajo Consumo)** | Modo de ahorro energético para monitoreo prolongado nocturno. | • Mantiene umbrales según modo Adulto/Niño seleccionado. | • Reduce frecuencia de muestreo y procesamiento. <br>• **Si pasan más de 10 minutos sin lectura válida, el sistema se apaga automáticamente.** |
<p align="center"><b>Tabla 3: Modos de uso</b></p>

## Referencias
1. National Institutes of Health (NIH) – Sleep Apnea Fact Sheet. Disponible en https://www.nhlbi.nih.gov/health/sleep-apnea
2. Sally K. Longmore et al., “A Comparison of Reflective Photoplethysmography for Detection of Heart Rate, Blood Oxygen Saturation, and Respiration Rate at Various Anatomical Locations”, *Sensors*, vol. 19, no. 8, 1874, Apr. 2019. Disponible en: https://www.mdpi.com/1424-8220/19/8/1874
3.  Park J., Seok H. S., Kim S. S., & Shin H. (2022). *Photoplethysmogram Analysis and Applications: An Integrative Review*. Frontiers in Physiology, 12, Article 808451. doi:10.3389/fphys.2021.808451. Disponible en: https://www.ncbi.nlm.nih.gov/pmc/articles/PMC8920970/ 
4.  MAX30102 – Pulse Oximeter and Heart-Rate Sensor Datasheet. Maxim Integrated. https://datasheets.maximintegrated.com/en/ds/MAX30102.pdf