# Visualizador 3D - Proyecto de Computación Gráfica

Este proyecto carga y renderiza un modelo 3D escaneado (formato `.obj`) con su textura correspondiente en una escena con OpenGL moderno. Se usaron técnicas de iluminación básica, proyección perspectiva y mapeo de textura.

---

## 📂 Estructura del Proyecto

```

.
├── glad/                  # Loader de funciones OpenGL moderno
├── include/               # Librerías externas (tinyobjloader, stb\_image)
├── shaders/               # Shaders GLSL para vértices y fragmentos
├── src/                   # Código fuente C++
│   ├── main.cpp           # Programa principal
│   ├── model\_loader.\*     # Carga de modelo OBJ
│   ├── shader\_loader.\*    # Compilación de shaders
│   └── texture\_loader.\*   # Carga de textura desde .jpg
├── star-war/              # Archivos escaneados (obj, mtl, textura jpg)
├── Makefile               # Para compilar en Linux
└── .gitignore             # Archivos a ignorar

````

---

## ⚙️ Instrucciones de Compilación

### ✅ Requisitos

- OpenGL 3.3+
- CMake o Make (Linux)
- GLFW
- GLM
- OpenCV (solo si se usará para RA más adelante)
- `pkg-config` (Linux)

### 💻 Linux (probado en Ubuntu)

1. Instala dependencias:

```bash
sudo apt update
sudo apt install libglfw3-dev libglm-dev libgl1-mesa-dev
````

2. Compila el proyecto:

```bash
make
```

3. Ejecuta:

```bash
./main1
```

---

### 🪟 Windows (VS Code con MSYS2 o MinGW)

1. Instala [MSYS2](https://www.msys2.org/)

2. Instala dependencias:

```bash
pacman -S mingw-w64-x86_64-glfw mingw-w64-x86_64-glm
```

3. Asegúrate de que `glad`, `stb_image.h` y `tiny_obj_loader.h` estén incluidos en las rutas correctas.

4. Compila con:

```bash
g++ src/*.cpp glad/glad.c -o main1 -Iinclude -Isrc -Iglad -lglfw3 -lopengl32 -lgdi32
```

---

## 🔁 Flujo del `main.cpp`

```cpp
1. Inicializa GLFW + crea ventana
2. Carga GLAD
3. Habilita el depth test (3D realista)

4. Carga:
   - Modelo `.obj`
   - Textura `.jpg`
   - Shaders GLSL

5. Crea y configura VAO/VBO

6. Calcula la matriz MVP:
   - `model`: rotación + escala
   - `view`: posición de cámara
   - `projection`: perspectiva

7. Pasa `mvp` al shader

8. En el bucle:
   - Limpia el frame
   - Dibuja el modelo
   - Aplica la textura

9. Libera recursos y termina
```

---

## ✨ Estado actual

* ✅ Renderizado básico del modelo OBJ
* ✅ Aplicación de textura escaneada
* 🚧 En progreso: integración con realidad aumentada
* 🚧 Próximos pasos: detección de patrón con OpenCV, animación y gestos

---

## 🙋‍♀️ Autores

* Andrea Cuela + Melany Cahuana + Rushell Zavalaga
* Universidad Nacional de San Agustín de Arequipa
* Curso: Computación Gráfica

---

## 📦 Dependencias externas

* [GLAD](https://glad.dav1d.de/)
* [GLFW](https://www.glfw.org/)
* [GLM](https://github.com/g-truc/glm)
* [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)
* [stb\_image](https://github.com/nothings/stb)

---

## 🛑 Notas

* Este proyecto no usa herramientas automáticas de modelado ni generación de código.
* El modelo fue escaneado localmente y procesado manualmente.

```


