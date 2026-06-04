# Paint de consola — patrones de diseño en C++

Ejercicio del curso: un "Paint" **sin interfaz gráfica** que aplica, sobre el
`Shape` que venimos construyendo desde el día 2, **solo los patrones
estrictamente necesarios** para que el ejemplo sea didáctico:

| Patrón        | Día | Dónde se ve en el código                                  |
|---------------|-----|-----------------------------------------------------------|
| **Singleton** | 3   | [`Canvas`](include/paint/Canvas.h) — un único lienzo      |
| **Prototype** | 3   | [`IShape::Clone`](include/paint/IShape.h) — clonar figuras |
| **Factory**   | 3   | [`ShapeFactory`](include/paint/ShapeFactory.h) — con registro |
| **Command**   | 4   | [`ICommand`](include/paint/ICommand.h) + comandos + [`CommandManager`](include/paint/CommandManager.h) |

No están todos los patrones del curso a propósito: el objetivo es ver estos
cuatro encajando limpiamente, no acumular código.

## Estructura del proyecto

```
paint/
├── CMakeLists.txt          # build principal: biblioteca + ejecutable + tests
├── include/paint/          # cabeceras (.h): la interfaz pública
│   ├── Point.h             #   tipo de valor (posición)
│   ├── IWriter.h           #   puerto de salida (DIP)
│   ├── ConsoleWriter.h
│   ├── IShape.h            #   abstracción de figura (+ Clone = Prototype)
│   ├── Circle.h / Square.h
│   ├── ShapeFactory.h      #   Factory con registro
│   ├── Canvas.h            #   Singleton
│   ├── ICommand.h          #   Command
│   ├── AddShapeCommand.h
│   ├── MoveShapeCommand.h
│   ├── DuplicateShapeCommand.h
│   ├── CommandManager.h    #   historial undo/redo
│   ├── ShapeRegistration.h #   registro de figuras de serie (OCP)
│   └── App.h               #   bucle REPL (lee de cin)
├── src/                    # implementaciones (.cpp)
│   ├── ...                 #   un .cpp por cada cabecera con lógica
│   └── main.cpp            #   composition root
└── tests/                  # pruebas con <cassert> + CTest
    ├── CMakeLists.txt
    └── test_paint.cpp
```

**Por qué `.h` + `.cpp` separados:** la cabecera declara *qué* ofrece cada
clase; el `.cpp` dice *cómo*. Quien usa la clase solo necesita leer el `.h`.
Además, así CMake compila cada `.cpp` por separado y solo recompila lo que
cambia.

**Por qué biblioteca (`paint_core`) + ejecutable (`paint`):** toda la lógica
vive en una biblioteca; `main.cpp` solo la arranca. Los **tests** enlazan con
esa misma biblioteca sin tocar `main`.

## Compilar y ejecutar

Requisitos: CMake ≥ 3.16 y un compilador con C++17 (MSVC, g++ o clang).

```bash
cmake -S . -B build
cmake --build build
```

Ejecutar el programa (lee comandos de la entrada estándar):

```bash
./build/paint            # Linux/macOS
build\Debug\paint.exe    # Windows / MSVC
```

Pasar tests:

```bash
ctest --test-dir build --output-on-failure
```

## Comandos del programa

La entrada se teclea por `cin`, una acción por línea:

```
circle <radio> <color> <x> <y>   añade un círculo
square <lado> <color> <x> <y>    añade un cuadrado
move <indice> <x> <y>            mueve la figura del índice a (x, y)
duplicate <indice>               duplica una figura (usa Prototype)
undo                             deshace la última acción
redo                             rehace la última acción deshecha
print                            lista el lienzo con índices
help                             muestra la ayuda
exit                             termina
```

### Ejemplo de sesión

```
circle 5 2 0 0
square 3 1 10 10
print
move 0 4 4
undo
duplicate 1
```

## Decisiones de diseño (las "buenas prácticas")

- **El cliente nunca hace `new`.** Las figuras nacen en `ShapeFactory`
  (Factory) o se clonan (Prototype). El resto del código depende de `IShape`,
  no de `Circle`/`Square` (DIP).
- **La GUI/REPL no toca el `Canvas` directamente.** Crea `ICommand`s y los
  entrega al `CommandManager`. Por eso `undo`/`redo` funcionan sin strings ni
  casos especiales: cada comando sabe deshacerse a sí mismo.
- **Factory con registro, instanciable.** Añadir una figura nueva es una línea
  en [`ShapeRegistration.cpp`](src/ShapeRegistration.cpp); no se toca
  `ShapeFactory` (OCP). La hacemos **inyectable** (la posee `App`) en lugar de
  100 % estática, que sería un Singleton oculto y difícil de testear.
- **Singleton con honestidad.** `Canvas` es un Singleton de Meyers (las cuatro
  defensas), pero los comandos reciben el `Canvas&` **por inyección** en vez de
  llamar a `Canvas::Instance()` por dentro. Así el patrón se ve, pero los
  comandos siguen siendo testeables. Singleton acopla: lo usamos porque "un
  solo lienzo" es una regla del dominio.
- **La figura no conoce la salida.** `IShape::ToString()` devuelve texto;
  *quién* lo imprime es el `IWriter` (consola, fichero, test). La figura no
  depende del puerto de salida.
- **Propiedad explícita con `unique_ptr` y `move`.** Quién es dueño de cada
  figura está claro en cada momento: el lienzo, o el comando que la tiene
  "guardada" mientras está deshecha.

## Relación con la teoría

- Singleton → `doc/day-03/01_singleton.md`
- Prototype → `doc/day-03/02_prototype.md`
- Factory   → `doc/day-03/03_factory.md`
- Command   → `doc/day-04/06_command.md`
