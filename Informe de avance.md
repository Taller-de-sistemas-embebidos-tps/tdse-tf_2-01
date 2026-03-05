# Sistema de detección de apneas y variaciones de oxigenación durante el sueño
<div align="center">

Trabajo Final - Taller de Sistemas Embebidos - FIUBA

|Alumno|Padron|
|------|-------|
|Tomás Musso Carboni|111548|
|Santiago Archetti|110707|
|Alan Romero|108316|
</div>

### Informe de avance de trabajo final
A continuacion, detallamos, en funcion de los distintas funcionalid### Estado de implementación

| Estado | Descripción |
|---|---|
| 🟢 | Ya implementado |
| 🟡 | En proceso de implementación |
| 🔴 | No implementado |ades del proyecto, el estado de las tareas. Siguiendo la siguiente logica:


###  Requerimientos funcionales

| Grupo | ID | Requerimiento Funcional | Descripción | Estado |
|---|---|---|---|---|
| Adquisición | 1.1 | Adquisición de señales | Capturar señales ópticas Rojo/IR del MAX30102 vía I²C. |  🟢 |
|  | 1.2 | Lectura periódica | Obtener muestras desde la FIFO del sensor. |🟢   |
|  | 1.3 | Análisis de variación | Detectar variaciones necesarias para SpO₂ y respiración. |  🟢 |
| Procesamiento | 2.1 | Filtrado digital | Aplicar filtrado. |  🟢 |
|  | 2.2 | Cálculo de SpO₂ | Calcular SpO₂ mediante el ratio R (AC/DC Rojo y AC/DC IR). | 🟢   |
|  | 2.3 | Cálculo de respiración | Estimar frecuencia respiratoria mediante análisis de envolvente o línea base. | 🟢  |
|  | 2.4 | Empaquetado de datos | Generar paquete con SpO₂, respiración y estado. |  🟢 |
| Indicadores | 3.1 | Alarma crítica | Activar alarma sonora cuando se detecta apnea. |  🟡|
|  | 3.2 | Indicadores LED | Mostrar estados (crítico, normal, error) mediante LEDs. | 🟡 |
| Comunicación | 4.1 | Comunicación Bluetooth | Transmitir datos vía HM-10 por UART. |  🟡|
|  | 4.2 | Envío de parámetros | Enviar SpO₂, respiraciones y estado del paciente a la app. | 🟡 |
|  | 4.3 | Registro de eventos | Informar alarmas, desconexiones o estados especiales. | 🟡 |
|  | 4.4 | Recepción de configuración | Recibir umbrales y parámetros desde la aplicación. |🟡  |
| Memoria | 5.1 | Guardar configuración | Almacenar umbrales y parámetros en EEPROM. | 🔴 |
|  | 5.2 | Restaurar valores | Recuperar valores predeterminados si hay error de memoria. | 🔴 |
| Alimentación | 6.1 | Alimentación | Funcionamiento con batería Li-Ion y regulación a 5V / 3.3V. | 🟡 |
|  | 6.2 | Bajo consumo | Entrar en modo STOP cuando no hay actividad. | 🟢 |
|  | 6.3 | Reactivación | Retomar actividad ante movimiento o comando. | 🟢 |

<p align="center"><b>Tabla 1: Requerimientos funcionales</b></p>

Las tareas no realizadas vinculadas a la memoria se deben a una optimización en el almacenamiento de datos. Por ende, no será necesario el uso de una memoria externa, ya que la memoria interna del microcontrolador resulta suficiente para cubrir los requerimientos del sistema. A su vez, utilizaremos un display en el que mostraremos los datos en tiempo real.
