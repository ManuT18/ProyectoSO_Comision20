# Variables
CC = gcc
CFLAGS = -Wall -lm -pthread

# Directorios de fuente y binario
SRC_DIR = src
BIN_DIR = bin

# Ejecutables en el directorio bin
MINISHELL = $(BIN_DIR)/Process_Com/MiniShell/minishell
PUMPER_QUEUES = $(BIN_DIR)/Process_Com/PumperInc/pumper_queues
PUMPER_PIPES = $(BIN_DIR)/Process_Com/PumperInc/pumper_pipes
MOTOS = $(BIN_DIR)/Process_Sync/Motos/motos
SANTACLAUS = $(BIN_DIR)/Process_Sync/SantaClaus/santaclaus

# Compilación de cada archivo, enviando los binarios a la carpeta bin
$(MINISHELL): $(SRC_DIR)/Process_Com/MiniShell/minishell.c | $(BIN_DIR)/Process_Com/MiniShell
	$(CC) $(CFLAGS) -o $(MINISHELL) $(SRC_DIR)/Process_Com/MiniShell/minishell.c

$(PUMPER_QUEUES): $(SRC_DIR)/Process_Com/PumperInc/pumper_queues.c | $(BIN_DIR)/Process_Com/PumperInc
	$(CC) $(CFLAGS) -o $(PUMPER_QUEUES) $(SRC_DIR)/Process_Com/PumperInc/pumper_queues.c

$(PUMPER_PIPES): $(SRC_DIR)/Process_Com/PumperInc/pumper_pipes.c | $(BIN_DIR)/Process_Com/PumperInc
	$(CC) $(CFLAGS) -o $(PUMPER_PIPES) $(SRC_DIR)/Process_Com/PumperInc/pumper_pipes.c

$(MOTOS): $(SRC_DIR)/Process_Sync/Motos/motos.c | $(BIN_DIR)/Process_Sync/Motos
	$(CC) $(CFLAGS) -o $(MOTOS) $(SRC_DIR)/Process_Sync/Motos/motos.c

$(SANTACLAUS): $(SRC_DIR)/Process_Sync/SantaClaus/santaclaus.c | $(BIN_DIR)/Process_Sync/SantaClaus
	$(CC) $(CFLAGS) -o $(SANTACLAUS) $(SRC_DIR)/Process_Sync/SantaClaus/santaclaus.c

# Objetivo para compilar todos los ejecutables
all: $(MINISHELL) $(PUMPER_QUEUES) $(PUMPER_PIPES) $(MOTOS) $(SANTACLAUS)

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
$(BIN_DIR)/Process_Com/MiniShell:
	mkdir -p $(BIN_DIR)/Process_Com/MiniShell

$(BIN_DIR)/Process_Com/PumperInc:
	mkdir -p $(BIN_DIR)/Process_Com/PumperInc

$(BIN_DIR)/Process_Sync/Motos:
	mkdir -p $(BIN_DIR)/Process_Sync/Motos

$(BIN_DIR)/Process_Sync/SantaClaus:
	mkdir -p $(BIN_DIR)/Process_Sync/SantaClaus

# Limpiar todos los ejecutables
clean:
	rm -f $(MINISHELL) $(PUMPER_QUEUES) $(PUMPER_PIPES) $(MOTOS) $(SANTACLAUS)
