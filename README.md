
# **Remote Performance Monitor**

## Requirements.

* GNU-Linux operating system based on debian / ubuntu (may not work on other distributions).
	* Protocol Buffers:
	* [**protobuf:**](https://github.com/protocolbuffers/protobuf) Standard library developed by Google to serialize data.
	* [**protobuf-c:**](https://github.com/protobuf-c/protobuf-c) Protobuf Wrapper to support the C language.

---

## Compilation.

1. After cloning the repository, move inside the project folder using the command line.
2. Make sure you have installed the compilation tool [**Make**](https://www.gnu.org/software/make/) if not, just run: sudo apt-get install make
3. Execute the "make" command

	* Compile the client:
		$ make monitord

	* Compile the server:
		$ make monitorc

	* Compile everything:
		$ make

---

## Usage:

### monitord (server program):

**$ ./monitord**

	Run as a daemon on localhost 8000


**$ ./monitord -c [<port>]**

	./monitord -c
		It runs in console mode on port 8000.
	./monitord -c 8888
		It runs in console mode on port 8888.


**$ ./monitord [-l <log_file>] [<port>]**

	./monitord -l mylog.log
		Run daemon on port 8000, the messages of the events will be saved in the mylog.log file
	./monitord -l mylog.log 9999
		Run daemon on port 9999, the messages of the events will be saved in the mylog.log file


**$ ./monitord -h**

	Show the help


### monitorc (client program):

**$ ./monitorc**

	It connects to localhost 8000


**$ ./monitorc [<host>] [<port>]**

	./monitorc host.myhost.xyz 8989
		It connects to host host.myhost.xyz, port: 8989.

	when the client connects succefully, scroll up the scrollbar to see the monitor data running 
	(this is a problem we are trying to fix)


**$ ./monitorc -h**
	Show the help

---



# **Monitor Remoto de Rendimiento.**

## Requerimientos.
	
* Systema operativo GNU-Linux basado en debian/ubuntu (No se asegura funcionamiento para otras distribuciones).
* Protocol Buffers:
	* [**protobuf:** ](https://github.com/protocolbuffers/protobuf)  Libreria estandar desarrollda por Google para serializar datos.
	* [**protobuf-c:**](https://github.com/protobuf-c/protobuf-c) Wrapper de protobuf para dar soporte al lenguaje c.

---

## Compilacion.

1. Luego de clonar el repositorio, moverse dentro de la carpeta del proyecto mediante la linea de comandos.
2. Asegurarse de tener instalada la herramienta de compilación [**Make**](https://www.gnu.org/software/make/)  si no, solo ejecutar:  sudo apt-get install make
3. Ejecutar el comando "make"

	* Compilar el cliente: 
		$ make monitord

	* Compilar el servidor:
		$ make monitorc

	* Compilar todo:
		$ make

---

## Modo de Uso:

### monitord (programa servidor):

**$ ./monitord**
	
			Se ejecuta como daemon en localhost 8000
	

**$ ./monitord -c [<port>]**
	
		./monitord -c
			Se ejecuta en modo consola en el puerto 8000.
		./monitord -c 8888
			Se ejecuta en modo consola en el puerto 8888.


**$ ./monitord [-l <log_file>] [<port>]**
	
		./monitord -l mylog.log
			Ejecuta daemon en el puerto 8000, los mensajes de los eventos se guardarán en el 
			archivo mylog.log
		./monitord -l mylog.log 9999
			Ejecuta daemon en el puerto 9999, los mensajes de los eventos se guardarán en el 
			archivo mylog.log


**$ ./monitord -h**
	
		Muestra la ayuda.
	

### monitorc (programa cliente):

**$ ./monitorc**

			Se conecta a localhost 8000


**$ ./monitorc [<host>] [<port>]**

		./monitorc host.myhost.xyz 8989
			Se conecta en el host host.myhost.xyz, puerto: 8989.

			Una vez en conexion desplazar hacia arriba el scrollbar para obersevar los datos del monitor corriendo


**$ ./monitorc -h**

		Muestra la ayuda.

---
