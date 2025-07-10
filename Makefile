# Nombre del ejecutable
TARGET = main1

# Archivos fuente
SRC = src/main.cpp src/model_loader.cpp src/shader_loader.cpp src/texture_loader.cpp glad/glad.c

# Cabeceras y directorios de inclusión
INCLUDES = -Iglad -Iinclude -Isrc

# Compilador
CXX = g++
CXXFLAGS = -std=c++17 $(INCLUDES) `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`
GLFLAGS = -lglfw -ldl -lGL -lm

# Regla por defecto
all: $(TARGET)

# Cómo compilar
$(TARGET): $(SRC)
	$(CXX) -o $(TARGET) $(SRC) $(CXXFLAGS) $(LDFLAGS) $(GLFLAGS)

# Limpiar archivos generados
clean:
	rm -f $(TARGET)
