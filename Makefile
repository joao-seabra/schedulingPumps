# ==============================================================================
#  Makefile - BCC466 Tecnicas Metaheuristicas para Otimizacao Combinatoria
#  Compativel com Linux (g++) e Windows (MinGW-w64 / g++).
# ==============================================================================

TARGET  = exe
SRC_DIR = src
OBJ_DIR = obj

CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I$(SRC_DIR)
LIBS =

# CORREÇÃO 1: Wildcard corrigido usando ESPAÇOS em vez de vírgulas
# Inclui a raiz do src e todas as subpastas necessárias
SOURCES = $(wildcard $(SRC_DIR)/*.cpp \
                     $(SRC_DIR)/interfaces/*.cpp \
                     $(SRC_DIR)/meta_heuristicas/*.cpp \
                     $(SRC_DIR)/refinamento/*.cpp \
                     $(SRC_DIR)/utilitarios/*.cpp \
                     $(SRC_DIR)/definicao_problema/*.cpp \
                     $(SRC_DIR)/construtivos/*.cpp)

HEADERS = $(wildcard $(SRC_DIR)/*.h \
                     $(SRC_DIR)/interfaces/*.h \
                     $(SRC_DIR)/meta_heuristicas/*.h \
                     $(SRC_DIR)/refinamento/*.h \
                     $(SRC_DIR)/utilitarios/*.h \
                     $(SRC_DIR)/definicao_problema/*.h \
                     $(SRC_DIR)/construtivos/*.h)

# CORREÇÃO 2: Mapeia de forma inteligente preservando a estrutura de subpastas
# Exemplo: src/interfaces/foo.cpp vira obj/interfaces/foo.o
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Detecta o sistema operacional para ajustar comandos e extensões
ifeq ($(OS),Windows_NT)
    EXECUTAVEL = $(TARGET).exe
    # No Windows, usamos o shell nativo cmd para criar as subpastas mapeadas
    MKDIR_P    = @if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
    CLEANCMD   = rmdir /S /Q $(OBJ_DIR) 2>NUL & del /Q $(EXECUTAVEL) 2>NUL
else
    EXECUTAVEL = $(TARGET)
    MKDIR_P    = mkdir -p $(dir $@)
    CLEANCMD   = rm -rf $(OBJ_DIR) $(EXECUTAVEL)
endif

all: $(EXECUTAVEL)

$(EXECUTAVEL): $(OBJECTS)
	@echo 'Ligando o executavel: $@'
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LIBS)
	@echo 'Compilacao concluida: $@'

# CORREÇÃO 3: Regra de padrão genérica que aceita subpastas dinamicamente
# O pré-requisito | garante que a subpasta específica dentro de obj/ seja criada antes
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo 'Compilando: $<'
	$(MKDIR_P)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(EXECUTAVEL)

clean:
	@echo 'Limpando o projeto...'
	$(CLEANCMD)

.PHONY: all run clean
