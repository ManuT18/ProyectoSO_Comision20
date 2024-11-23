# Variables
CC = gcc
CFLAGS = -Wall -lm -pthread

# Directorios de fuente y binario
SRC = src
BIN = bin

# Lista de todos los fuentes en la carpeta MiniShell/cmd
MINISHELL_COMMANDS_SOURCES = $(wildcard $(SRC)/Process_Com/MiniShell/cmd/*.c)
# Transformación de los nombres de los fuentes a estilo binario (sin extensión)
MINISHELL_COMMANDS_BINARIES = $(patsubst $(SRC)/Process_Com/MiniShell/cmd/%.c, $(BIN)/Process_Com/MiniShell/cmd/%, $(MINISHELL_COMMANDS_SOURCES))

# Ejecutables en el directorio bin
MINISHELL = $(BIN)/Process_Com/MiniShell/minishell
PUMPER_QUEUES = $(BIN)/Process_Com/PumperInc/pumper_queues
PUMPER_PIPES = $(BIN)/Process_Com/PumperInc/pumper_pipes
MOTOS = $(BIN)/Process_Sync/Motos/motos
SANTACLAUS = $(BIN)/Process_Sync/SantaClaus/santaclaus

## Compilación de cada archivo, enviando los binarios a la carpeta bin.

# Compilación de los comandos de minishell
$(BIN)/Process_Com/MiniShell/cmd/%: $(SRC)/Process_Com/MiniShell/cmd/%.c | $(BIN)/Process_Com/MiniShell/cmd
	$(CC) $(CFLAGS) -o $@ $<

$(MINISHELL): $(SRC)/Process_Com/MiniShell/minishell.c | $(BIN)/Process_Com/MiniShell
	$(CC) $(CFLAGS) -o $(MINISHELL) $(SRC)/Process_Com/MiniShell/minishell.c

$(PUMPER_QUEUES): $(SRC)/Process_Com/PumperInc/pumper_queues.c | $(BIN)/Process_Com/PumperInc
	$(CC) $(CFLAGS) -o $(PUMPER_QUEUES) $(SRC)/Process_Com/PumperInc/pumper_queues.c

$(PUMPER_PIPES): $(SRC)/Process_Com/PumperInc/pumper_pipes.c | $(BIN)/Process_Com/PumperInc
	$(CC) $(CFLAGS) -o $(PUMPER_PIPES) $(SRC)/Process_Com/PumperInc/pumper_pipes.c

$(MOTOS): $(SRC)/Process_Sync/Motos/motos.c | $(BIN)/Process_Sync/Motos
	$(CC) $(CFLAGS) -o $(MOTOS) $(SRC)/Process_Sync/Motos/motos.c

$(SANTACLAUS): $(SRC)/Process_Sync/SantaClaus/santaclaus.c | $(BIN)/Process_Sync/SantaClaus
	$(CC) $(CFLAGS) -o $(SANTACLAUS) $(SRC)/Process_Sync/SantaClaus/santaclaus.c

# Objetivo para compilar todos los ejecutables
all: $(MINISHELL_COMMANDS_BINARIES) $(MINISHELL) $(PUMPER_QUEUES) $(PUMPER_PIPES) $(MOTOS) $(SANTACLAUS)

# Objetivos para ejecutar cada programa individualmente
run_minishell: $(MINISHELL)
	./$(MINISHELL)

run_pumper_queues: $(PUMPER_QUEUES)
	./$(PUMPER_QUEUES)

run_pumper_pipes: $(PUMPER_PIPES)
	./$(PUMPER_PIPES)

run_motos: $(MOTOS)
	./$(MOTOS)

run_santaclaus: $(SANTACLAUS)
	./$(SANTACLAUS)

# Crear directorios bin si no existen

$(BIN)/Process_Com/MiniShell/cmd:
	mkdir -p $(BIN)/Process_Com/MiniShell/cmd

$(BIN)/Process_Com/MiniShell:
	mkdir -p $(BIN)/Process_Com/MiniShell

$(BIN)/Process_Com/PumperInc:
	mkdir -p $(BIN)/Process_Com/PumperInc

$(BIN)/Process_Sync/Motos:
	mkdir -p $(BIN)/Process_Sync/Motos

$(BIN)/Process_Sync/SantaClaus:
	mkdir -p $(BIN)/Process_Sync/SantaClaus

# Limpiar la carpeta de binarios
clean:
	rm -f $(MINISHELL_COMMANDS_BINARIES) $(MINISHELL) $(PUMPER_QUEUES) $(PUMPER_PIPES) $(MOTOS) $(SANTACLAUS)
	rm -R $(BIN)
