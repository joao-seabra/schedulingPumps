# ==============================================================================
#  Makefile - BCC466 Tecnicas Metaheuristicas para Otimizacao Combinatoria
#  Compativel com Linux (g++) e Windows (MinGW-w64 / g++).
#
#  Alvos:
#    make               -> compila o executavel normal (exe), com src/main.cpp
#    make exe_irace     -> compila o executavel do irace, com src/main_irace.cpp
#                          (no Windows: make exe_irace.exe)
#    make irace         -> compila exe_irace e roda o irace
#    make run | clean
# ==============================================================================

TARGET        = exe
TARGET_IRACE  = exe_irace
SRC_DIR       = src
OBJ_DIR       = obj
IRACE_DIR     = irace

# Os dois arquivos que definem main(). NAO podem ser ligados juntos.
# Ajuste MAIN_IRACE se o seu arquivo tiver outro nome/caminho.
MAIN          = $(SRC_DIR)/main.cpp
MAIN_IRACE    = $(IRACE_DIR)/main_irace.cpp

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I$(SRC_DIR)
LIBS     =

SOURCES = $(wildcard $(SRC_DIR)/*.cpp \
                     $(SRC_DIR)/interfaces/*.cpp \
                     $(SRC_DIR)/meta_heuristicas/*.cpp \
                     $(SRC_DIR)/refinamento/*.cpp \
                     $(SRC_DIR)/utilitarios/*.cpp \
                     $(SRC_DIR)/definicao_problema/*.cpp \
                     $(SRC_DIR)/construtivos/*.cpp \
                     $(IRACE_DIR)/*.cpp)

HEADERS = $(wildcard $(SRC_DIR)/*.h \
                     $(SRC_DIR)/interfaces/*.h \
                     $(SRC_DIR)/meta_heuristicas/*.h \
                     $(SRC_DIR)/refinamento/*.h \
                     $(SRC_DIR)/utilitarios/*.h \
                     $(SRC_DIR)/definicao_problema/*.h \
                     $(SRC_DIR)/construtivos/*.h \
                     $(IRACE_DIR)/*.h*)

# CORRECAO: o wildcard de src/*.cpp pegava main.cpp E main_irace.cpp, e os dois
# definem main() -> "multiple definition of main". Aqui os mains saem da lista
# comum e cada executavel liga a lista comum + o seu proprio main.
COMMON_SOURCES = $(filter-out $(MAIN) $(MAIN_IRACE),$(SOURCES))

COMMON_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(COMMON_SOURCES))
MAIN_OBJECT    = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN))
IRACE_OBJECT   = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_IRACE))

# Detecta o sistema operacional para ajustar comandos e extensoes
ifeq ($(OS),Windows_NT)
    EXECUTAVEL        = $(TARGET).exe
    EXECUTAVEL_IRACE  = $(TARGET_IRACE).exe
    MKDIR_P    = @if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
    CLEANCMD   = rmdir /S /Q $(OBJ_DIR) 2>NUL & del /Q $(EXECUTAVEL) $(EXECUTAVEL_IRACE) 2>NUL
else
    EXECUTAVEL        = $(TARGET)
    EXECUTAVEL_IRACE  = $(TARGET_IRACE)
    MKDIR_P    = mkdir -p $(dir $@)
    CLEANCMD   = rm -rf $(OBJ_DIR) $(EXECUTAVEL) $(EXECUTAVEL_IRACE)
endif

all: $(EXECUTAVEL)

$(EXECUTAVEL): $(COMMON_OBJECTS) $(MAIN_OBJECT)
	@echo 'Ligando o executavel: $@'
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)
	@echo 'Compilacao concluida: $@'

$(EXECUTAVEL_IRACE): $(COMMON_OBJECTS) $(IRACE_OBJECT)
	@echo 'Ligando o executavel do irace: $@'
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)
	@echo 'Compilacao concluida: $@'

# Regra de padrao generica que aceita subpastas (src/x/foo.cpp -> obj/x/foo.o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo 'Compilando: $<'
	$(MKDIR_P)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(EXECUTAVEL)

# CORRECAO: depende do exe_irace (antes dependia de 'all', que nao gera o exe_irace)
irace: $(EXECUTAVEL_IRACE)
	@echo 'A iniciar o processo do irace...'
	Rscript -e "scenario <- irace::readScenario(filename = 'irace/scenario.txt'); irace::irace(scenario = scenario)"

clean:
	@echo 'Limpando o projeto...'
	$(CLEANCMD)

.PHONY: all run irace clean