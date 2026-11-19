# Desarrollo de la máquina expendedora

## 1. Primeras pruebas de los componentes

Antes de comenzar a desarrollar el programa completo de la máquina expendedora, fui probando los distintos componentes por separado. La idea era comprobar que cada uno funcionaba correctamente y entender cómo debía utilizarlo antes de integrarlo con el resto del sistema.

Para organizar estas pruebas, creé una carpeta llamada `Codigos_Prueba` en el repositorio de GitHub. Dentro fui guardando los programas individuales que utilizaba para comprobar cada componente:

- **Boton.ino:** pruebas iniciales del funcionamiento del pulsador.
- **Conexiones.ino:** comprobación de las conexiones de los distintos elementos.
- **Joystick.ino:** pruebas de los movimientos del joystick y de su pulsador central.
- **LED.ino:** comprobación del encendido y apagado de los LED.
- **PruebaCrystal:** primeras pruebas con la pantalla LCD, verificando que podía mostrar mensajes correctamente.

Esta primera fase me permitió detectar posibles errores de conexión y comprobar que cada elemento respondía como esperaba antes de empezar a trabajar con todos ellos a la vez.

## 2. Integración del pulsador

El primer componente que integré en el proyecto fue el pulsador principal. Aunque inicialmente parecía uno de los elementos más sencillos, terminó siendo una de las partes que más trabajo me dio durante el desarrollo.

El principal problema que encontré fueron los rebotes del pulsador. Al presionarlo o soltarlo, el contacto eléctrico no cambia de estado de forma completamente limpia, sino que puede producir pequeñas variaciones muy rápidas. Esto provocaba que el Arduino pudiera interpretar una única pulsación como varias pulsaciones consecutivas.

Para solucionar este problema tuve que investigar bastante sobre el funcionamiento de los pulsadores y las distintas formas de tratar los rebotes. Una de las referencias que consulté fue el libro *Arduino Cookbook, 3rd Edition*, concretamente el capítulo 5.4, titulado **«Determining How Long a Switch Is Pressed»**.

Esta sección me resultó especialmente útil porque explica cómo determinar durante cuánto tiempo se mantiene pulsado un interruptor. A partir de esta documentación fui desarrollando la lógica necesaria para distinguir entre una pulsación y una liberación, evitando que los rebotes afectasen a la medición del tiempo.

Otra de las cosas que tuve que entender fue el funcionamiento de las resistencias pull-up. En mi caso, al utilizar un Arduino Mega, tuve que investigar cómo funcionaba esta configuración y por qué era necesaria para leer correctamente el pulsador. Al principio me costó entenderlo, ya que no estaba acostumbrado a trabajar con este tipo de configuración.

Lo que hace una resistencia pull-up es mantener la entrada del Arduino en un estado HIGH (5 V) cuando el pulsador no está presionado. De esta manera, la entrada siempre tiene un valor definido y no se queda en un estado indeterminado. Cuando se pulsa el botón, este conecta la entrada con GND, haciendo que pase a LOW (0 V). Así, el Arduino puede distinguir fácilmente entre el botón pulsado y sin pulsar.

Para este proyecto utilicé la resistencia pull-up interna del propio Arduino, configurando el pin como `INPUT_PULLUP`, por lo que no necesité añadir una resistencia externa.

También decidí utilizar una **interrupción por hardware** para detectar el pulsador principal. Esto lo hice porque quería que la pulsación se detectase en cualquier momento, independientemente de la parte del programa que se estuviese ejecutando.

Si hubiese comprobado el estado del botón únicamente dentro del `loop()`, podría ocurrir que, justo cuando lo pulsase, el programa estuviese realizando otra operación y todavía no hubiese llegado a la parte encargada de leer el pulsador. Dependiendo de lo que estuviese haciendo el programa, podría tardar en detectarlo o incluso perder una pulsación muy breve.

Al utilizar una interrupción por hardware, el microcontrolador puede ejecutar una pequeña función cuando detecta el cambio de estado del pin, sin tener que esperar a que el programa llegue a una instrucción concreta del bucle principal.

Finalmente, conseguí utilizar el pulsador para realizar diferentes acciones dependiendo de cuánto tiempo se mantuviera presionado. Esto sería importante más adelante, ya que el mismo botón permite acceder al Modo Administración o reiniciar la máquina, dependiendo de la duración de la pulsación.

## 3. Integración del resto de componentes

Una vez que conseguí que el pulsador funcionase correctamente, fui incorporando progresivamente el resto de componentes.

Primero integré los LED, comprobando que podía controlar su encendido y apagado. Más adelante utilicé uno de ellos para indicar visualmente los distintos estados de la máquina y otro para representar el progreso de la preparación de una bebida mediante cambios progresivos de intensidad.

https://github.com/user-attachments/assets/136097a4-18f3-4cae-af65-b4c667bbeb81

Después incorporé la pantalla LCD, que sería la encargada de mostrar los mensajes y la información de la máquina. Realicé diferentes pruebas para comprobar que podía escribir en sus dos líneas y actualizar el contenido según las necesidades del programa.

A continuación integré el joystick. Comprobé el funcionamiento de sus dos ejes y utilicé también el pulsador central para confirmar las selecciones. Durante las pruebas tuve que ajustar el sentido del movimiento, ya que inicialmente los desplazamientos verticales se interpretaban al revés.

Por último, incorporé los sensores de temperatura y humedad DHT11 y el sensor ultrasónico HC-SR04. El primero permite obtener información del entorno, mientras que el segundo sirve para detectar la presencia de un cliente delante de la máquina.

Durante esta fase fui probando cada incorporación y comprobando que los componentes que ya estaban funcionando seguían respondiendo correctamente.

## 4. Organización del programa mediante hilos (Threads)

Cuando el programa empezó a incorporar más componentes y funcionalidades, fue necesario organizar mejor su funcionamiento. En lugar de tener todas las operaciones concentradas en una única secuencia, utilicé la biblioteca Thread para distribuir las diferentes tareas en hilos.

Cada hilo se encarga de ejecutar una parte concreta del programa con una frecuencia determinada. De esta forma, no es necesario comprobar continuamente todos los elementos en cada vuelta del bucle principal.

Los principales hilos que fui incorporando son los siguientes:

- **Hilo del pulsador:** comprueba el estado del botón principal y calcula cuánto tiempo se mantiene pulsado. Es necesario para diferenciar entre una pulsación corta y una pulsación larga, que permiten realizar acciones distintas.

- **Hilo del joystick:** lee los movimientos de los dos ejes y detecta cuándo se pulsa el botón central. Permite controlar los menús sin tener que utilizar el pulsador principal.

- **Hilo del sensor de distancia:** obtiene periódicamente la distancia medida por el HC-SR04. Es necesario para detectar cuándo se acerca una persona a la máquina y comenzar el proceso de atención.

- **Hilo de temperatura y humedad:** realiza las lecturas del DHT11. Como este sensor no necesita actualizarse constantemente, se estableció un intervalo entre lecturas para evitar consultas innecesarias.

- **Hilo del contador:** actualiza el tiempo que lleva encendida la placa. Esta información se utiliza en el apartado de administración para consultar el tiempo de funcionamiento del sistema.

- **Hilo del servicio:** controla las diferentes fases del funcionamiento normal de la máquina, como esperar a un cliente, mostrar la información ambiental, permitir seleccionar un producto, preparar la bebida y avisar de que se puede retirar.

- **Hilo de la pantalla:** actualiza la información que aparece en la LCD dependiendo del modo y del estado en el que se encuentre la máquina.

Es importante tener en cuenta que estos hilos no se ejecutan realmente en paralelo. La biblioteca Thread permite organizar las tareas y ejecutarlas de forma periódica, pero el Arduino Mega va atendiendo cada una de ellas de manera secuencial.

Esta organización hace que el programa sea más sencillo de mantener y permite separar las distintas funcionalidades, evitando tener toda la lógica mezclada en un único bloque.

## 5. Desarrollo del Modo Servicio

Una vez integrados los componentes principales y organizadas las tareas, comencé a desarrollar el funcionamiento normal de la máquina, denominado Modo Servicio.

El sistema comienza esperando a que un cliente se acerque. Para ello utiliza el sensor ultrasónico, que comprueba si hay alguien a menos de un metro de distancia.

Cuando detecta a una persona, comienza mostrando durante cinco segundos la temperatura y la humedad del entorno. Después pasa al catálogo de productos, que se muestra en la pantalla LCD.

El usuario puede desplazarse por los diferentes productos utilizando el joystick y confirmar su elección pulsando el propio joystick. El catálogo inicial incluye café solo, café cortado, café doble, café premium y chocolate, cada uno con su precio correspondiente.

Una vez seleccionado un producto, comienza la preparación. Para simular este proceso, se establece una duración aleatoria de entre cuatro y ocho segundos. Durante ese tiempo, uno de los LED aumenta progresivamente su intensidad, indicando visualmente que la bebida se está preparando.

Cuando termina la preparación, la pantalla muestra el mensaje «RETIRE BEBIDA» durante tres segundos. Después, la máquina vuelve al estado inicial de espera.

## 6. Desarrollo del Modo Administración

Después de completar el funcionamiento básico del servicio, añadí un segundo modo destinado a la administración de la máquina.

Para acceder a él hay que mantener pulsado el botón principal durante al menos cinco segundos. Una vez dentro, el LED correspondiente indica que la máquina se encuentra en este modo.

Al principio, implementé únicamente las opciones de consulta de temperatura y humedad, distancia y tiempo que llevaba encendida la placa. Estas fueron las primeras funcionalidades del Modo Administración que desarrollé y se pueden ver en el vídeo:

https://github.com/user-attachments/assets/f02350d5-1739-4f0e-bf0f-b0623d445a9c

Más adelante fui ampliando este modo. Una de las últimas funcionalidades que añadí fue la posibilidad de consultar y modificar los precios de los productos. Desde el menú se puede seleccionar un producto y aumentar o disminuir su precio en intervalos de cinco céntimos utilizando el joystick. Para guardar el nuevo precio hay que pulsar el joystick, mientras que el movimiento hacia la izquierda permite cancelar los cambios.

Los precios modificados se utilizan posteriormente en el Modo Servicio, por lo que los cambios realizados desde administración se reflejan automáticamente cuando un cliente consulta el catálogo.

Para salir del Modo Administración hay que mantener pulsado el botón principal durante cinco segundos. De esta manera se vuelve al funcionamiento normal de la máquina.

## 7. Modo Arranque

También incorporé un pequeño proceso de arranque para indicar visualmente que la máquina está iniciándose.

Cada vez que se enciende o reinicia el Arduino, la pantalla muestra el mensaje «CARGANDO...». Al mismo tiempo, el primer LED parpadea tres veces, con intervalos de un segundo entre cada ciclo de encendido y apagado.

Cuando termina esta secuencia, el LED queda apagado y la máquina entra automáticamente en el Modo Servicio.

## 8. Problemas encontrados durante el desarrollo: el reinicio y el Watchdog

Durante el desarrollo también tuve que resolver algunos problemas relacionados con el reinicio de la placa.

Inicialmente, para reiniciar el Arduino utilizaba una función que apuntaba directamente a la dirección de memoria `0x0000`. De esta forma, al ejecutar esa función se provocaba que el programa volviese a empezar desde el principio.

Sin embargo, investigando un poco más, descubrí que podía utilizar el watchdog para realizar esta misma función de una forma más sencilla. El watchdog es un mecanismo de seguridad que incorpora el microcontrolador y que permite reiniciarlo cuando no recibe una señal de que el programa sigue funcionando correctamente dentro de un tiempo determinado.

Por ello, decidí utilizar el watchdog para implementar el reinicio de la máquina en lugar de recurrir al salto a la dirección `0x0000`.

Durante las pruebas me encontré con otro problema: la placa se reiniciaba continuamente al iniciar el programa. Esto ocurría porque, después de un reinicio provocado por el watchdog, este podía permanecer activo durante el siguiente arranque. Como no lo estaba desactivando suficientemente pronto, volvía a provocar otro reinicio antes de que el programa pudiera inicializarse correctamente.

Para solucionarlo, cambié el orden de inicialización. Ahora desactivo el watchdog al comienzo del `setup()`, antes de inicializar el resto de elementos y configurar los hilos. Una vez que el sistema está preparado, vuelvo a activarlo.

De esta manera evito que la placa entre en un bucle de reinicios durante el arranque. Además, cuando el watchdog está activo, es necesario reiniciarlo periódicamente durante la ejecución normal para indicarle que el programa continúa funcionando correctamente.

## 9. Ajustes finales de la pantalla LCD

Una de las últimas mejoras que realicé fue incorporar un potenciómetro para regular el contraste de la pantalla LCD.

Al principio, el contraste de la pantalla no era adecuado y los caracteres se veían demasiado claros, por lo que costaba leer correctamente los mensajes. Después de revisar las conexiones de la pantalla, decidí añadir un potenciómetro de 10 kΩ para poder ajustar manualmente el contraste.

Conecté sus dos extremos a 5 V y GND, y el terminal central a la entrada VO de la pantalla LCD. De esta forma, podía regular la tensión aplicada a la entrada de contraste y conseguir que los caracteres se visualizaran correctamente.

Fue uno de los últimos ajustes que incorporé, pero mejoró bastante la visibilidad de la pantalla y facilitó la lectura de la información que muestra la máquina.

## 10. Documentación del proyecto con Doxygen

Como parte del desarrollo del proyecto, también he intentado generar documentación automática del programa utilizando Doxygen.

La idea era conseguir un documento que explicase un poco mejor cómo está organizado el código y qué función tiene cada una de sus partes. Para ello, fui añadiendo comentarios especiales al programa, explicando las variables principales, las funciones, los sensores, los diferentes estados de funcionamiento y la organización mediante hilos.

También estuve configurando Doxygen para que reconociese los archivos de Arduino (`.ino`), ya que inicialmente no conseguía que analizase correctamente el programa. Tuve que modificar algunos parámetros de configuración, como los patrones de archivos que debía buscar y la opción de extraer toda la información disponible.

Además, intenté incluir una imagen externa del circuito, llamada `circuito.png`, para acompañar la explicación del funcionamiento del sistema y mostrar cómo estaban conectados los componentes.

Aunque al principio la documentación generada estaba bastante vacía y tuve que ir ajustando la configuración, conseguí avanzar hasta obtener un documento con información sobre la estructura del programa. Me pareció interesante probar esta herramienta porque permite entender mejor el proyecto sin tener que revisar todo el código directamente.

## 11. Circuito de la máquina expendedora

Para completar la documentación del proyecto, también he incluido una imagen del circuito con las conexiones de los diferentes componentes.

En ella se puede observar cómo están conectados los elementos principales al Arduino Mega: la pantalla LCD, el pulsador principal, los LED, el joystick, el sensor de temperatura y humedad DHT11 y el sensor ultrasónico HC-SR04. También aparece el potenciómetro utilizado para regular el contraste de la pantalla.

Esta imagen permite tener una visión general del montaje y entender cómo se relacionan los distintos componentes con la placa. Además, sirve como referencia para comprobar las conexiones físicas y facilita la comprensión del funcionamiento del sistema.

<img width="1876" height="1841" alt="circuito" src="https://github.com/user-attachments/assets/13308bb8-37e4-4909-8d1f-54938d445669" />

## 12. Resultado actual

A lo largo del desarrollo he pasado de probar cada componente de forma independiente a integrarlos en un único sistema.

He ido incorporando progresivamente el pulsador, los LED, la pantalla, el joystick y los sensores, comprobando su funcionamiento y solucionando los problemas que iban apareciendo. Posteriormente he organizado el programa mediante hilos y he desarrollado los distintos estados de funcionamiento de la máquina.

También he trabajado en aspectos que van más allá de la programación, como el ajuste del contraste de la pantalla, la documentación del código mediante Doxygen y la representación del circuito.

Actualmente, el proyecto cuenta con un Modo Arranque, un Modo Servicio y un Modo Administración, integrando tanto la interacción con el usuario como la consulta y modificación de la información de la máquina.

https://youtu.be/DZK6Qa-B8Ao
