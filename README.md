# Tarea 2 - Tensor++

Implementacion de una biblioteca basica de tensores en C++ usando memoria dinamica (`double*`).

## Compilacion

Con CMake:

```bash
cmake -S . -B build
cmake --build build
```

Luego ejecutar el binario generado dentro de `build`.

En CLion, abrir el proyecto y ejecutar la configuracion de CMake.

## Funcionalidades

- Constructor principal con validacion de shape y cantidad de valores.
- Tensores predefinidos: `zeros`, `ones`, `random`, `arange`.
- Gestion de memoria: constructor de copia, constructor de movimiento, asignacion por copia, asignacion por movimiento y destructor.
- Operadores: `+`, `-`, `*`, `/`.
- Cambio de dimensiones: `view` y `unsqueeze`.
- Concatenacion con `concat`.
- Funciones amigas: `dot` y `matmul`.
- Ejemplo de red neuronal con `view`, `matmul`, suma de bias, `ReLU` y `Sigmoid`.
 