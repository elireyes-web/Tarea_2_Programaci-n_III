# Tarea 2 - Tensor++

Implementación de una biblioteca de tensores en C++ (hasta 3 dimensiones), inspirada en NumPy/PyTorch, desarrollada para el curso CS2013 - Programación III.

## Integrantes

- [Nombre 1]
- [Nombre 2]

## Compilación

Con CMake:

```bash
cmake -S . -B build
cmake --build build
```

El binario se genera dentro de la carpeta `build/`.

**En CLion:** abrir el proyecto y ejecutar la configuración de CMake por defecto (botón ▶ Run).

## Ejecución

```bash
./build/main        # Linux/Mac
build\main.exe       # Windows
```

El programa ejecuta una serie de pruebas de todas las funcionalidades (operadores, `view`, `unsqueeze`, `concat`, `dot`, `matmul`) y finaliza con la simulación de una red neuronal de 1000×20×20 hasta la salida 1000×10, imprimiendo el tamaño del tensor en cada etapa.

## Funcionalidades implementadas

### Creación de tensores
- Constructor principal `Tensor(shape, values)` con validación de forma (1-3 dimensiones) y de coincidencia entre `shape` y cantidad de valores.
- Métodos estáticos: `zeros`, `ones`, `random(min, max)`, `arange(start, end, step)`.

### Gestión de memoria (Rule of Five)
- Constructor de copia (deep copy), constructor de movimiento, asignación por copia, asignación por movimiento y destructor.
- La clase usa un flag interno `owns_data` para distinguir tensores dueños de su memoria de **vistas** que solo la referencian (ver sección de diseño más abajo). Las 5 funciones especiales fueron auditadas para mantener este flag consistente en todos los casos.

### Operadores sobrecargados
- `+`, `-`, `*` (elemento a elemento, tensor-tensor)
- `*` (tensor-escalar, ej. `A * 2.0`)
- `/` (elemento a elemento, no pedido explícitamente pero implementado para completar la aritmética básica)
- Todos lanzan `std::invalid_argument` si las formas son incompatibles y no modifican los operandos originales.
- `+` soporta *broadcasting* limitado: permite sumar un tensor `{1, N}` (bias) a uno `{M, N}`, usado en la simulación de red neuronal.

### Modificación de forma sin copiar datos
- `view(new_shape)`: reinterpreta la forma de un tensor sin copiar sus datos.
- `unsqueeze(dim)`: inserta una dimensión de tamaño 1 en la posición indicada.

### Concatenación
- `Tensor::concat(tensors, dim)`: valida compatibilidad dimensional, reserva memoria nueva y copia los datos de forma controlada.

### Funciones `friend`
- `dot(a, b)`: producto punto entre tensores de igual forma.
- `matmul(a, b)`: multiplicación matricial entre tensores 2D compatibles.

### Aplicación: red neuronal
Simula el forward pass descrito en el enunciado: `view` → `matmul` → suma de bias → `ReLU` → `matmul` → suma de bias → `Sigmoid`, imprimiendo el tamaño del tensor en cada paso.

## Decisiones de diseño

**`view` y `unsqueeze` no usan `std::move`, usan una vista compartida sin ownership.**
El enunciado menciona "move" para estos métodos, pero también exige que "el tensor original permanezca válido". Un `move` real dejaría al tensor original en un estado nulo, lo cual entra en conflicto con ese segundo requisito. En su lugar, ambos métodos crean un nuevo `Tensor` que **apunta al mismo bloque de memoria** que el original, marcado con `owns_data = false`. Solo el tensor original (`owns_data = true`) libera la memoria al destruirse; las vistas nunca lo hacen. Esto satisface "no copiar datos" y "el original permanece válido" simultáneamente.

**Limitación conocida:** no hay conteo de referencias (`shared_ptr`-like). Si el tensor dueño de los datos se destruye o se reasigna mientras una vista derivada de él sigue viva, esa vista queda con un puntero inválido (*dangling pointer*). El diseño actual asume que el programador mantiene vivo el tensor original mientras usa sus vistas — no está validado en tiempo de ejecución.

**`Tensor::random` no está sembrado con `srand()`.**
Actualmente genera la misma secuencia de valores en cada ejecución del programa, ya que no se inicializa la semilla del generador. No afecta la corrección de las operaciones, solo la variabilidad de los valores aleatorios entre corridas.

## Estructura del repositorio

```
├── main.cpp # Implementación completa de la clase Tensor y pruebas
├── CMakeLists.txt # Configuración de build
├── README.md
└── .gitignore
```
