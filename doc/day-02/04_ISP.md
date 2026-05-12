# Día 2 — Bloque 4: ISP (Interface Segregation Principle)

> *"Los clientes no deberían verse forzados a depender de interfaces que
> no usan."* — Robert C. Martin

---

## 1. El problema en una frase

Cuando una interfaz mete demasiadas cosas, **todos los que la implementan
cargan con todo**, incluso con métodos que no tienen sentido para ellos.
Y todos los que la usan **dependen de cambios que no les afectan**.

## 2. Definición formal

> Una clase no debe estar obligada a implementar métodos que no necesita.
> Mejor **muchas interfaces pequeñas y específicas** que una grande y
> general.

En C++ "interfaz" se traduce como **clase abstracta con métodos virtuales
puros**. ISP es la guía sobre **cómo dimensionar esas clases abstractas**.

## 3. Bad: la "fat interface"

La interfaz `IMultifuncion` mete impresora, escáner y fax. Y resulta que
tenemos también una impresora simple, que no es ni escáner ni fax.

```cpp
class IMultifuncion {
public:
    virtual ~IMultifuncion() = default;
    virtual void imprimir(const std::string& doc) = 0;
    virtual void escanear() = 0;
    virtual void enviarFax(const std::string& num) = 0;
};

// Una multifunción real: todo bien
class HPMultifuncion : public IMultifuncion {
public:
    void imprimir(const std::string& doc) override { /* ok */ }
    void escanear() override { /* ok */ }
    void enviarFax(const std::string& num) override { /* ok */ }
};

// Una impresora simple: problemas
class HPLaserSimple : public IMultifuncion {
public:
    void imprimir(const std::string& doc) override { /* ok */ }

    void escanear() override {
        throw std::logic_error("No tengo escáner");   // ❌ LSP roto
    }

    void enviarFax(const std::string& num) override {
        throw std::logic_error("No tengo fax");       // ❌ LSP roto
    }
};
```

### ¿Qué duele?

- `HPLaserSimple` **está obligada a implementar métodos que no soporta**.
  Lanzar excepción es violar LSP de paso.
- Quien recibe `IMultifuncion*` no sabe si puede llamar a `escanear()`
  sin que explote.
- Añadir `engrapar()` a la interfaz fuerza a **todas** las impresoras a
  implementarlo (o relanzar excepción), aunque solo una lo soporte.
- La fat interface **acopla todos los clientes**. Si cambias la firma de
  `enviarFax`, recompila toda la torre.

## 4. Good: interfaces segregadas

Cada capacidad va en su propia interfaz pequeña. Cada clase implementa
**solo las que de verdad ofrece**.

```cpp
class IImpresora {
public:
    virtual ~IImpresora() = default;
    virtual void imprimir(const std::string& doc) = 0;
};

class IEscaner {
public:
    virtual ~IEscaner() = default;
    virtual void escanear() = 0;
};

class IFax {
public:
    virtual ~IFax() = default;
    virtual void enviarFax(const std::string& num) = 0;
};

// La impresora simple solo implementa lo que ofrece
class HPLaserSimple : public IImpresora {
public:
    void imprimir(const std::string& doc) override { /* ok */ }
};

// La multifunción combina interfaces
class HPMultifuncion : public IImpresora, public IEscaner, public IFax {
public:
    void imprimir(const std::string& doc) override { /* ok */ }
    void escanear() override { /* ok */ }
    void enviarFax(const std::string& num) override { /* ok */ }
};
```

Y el cliente pide **solo la capacidad que necesita**:

```cpp
// Una función que solo imprime no necesita saber nada más:
void imprimirInforme(IImpresora& imp, const std::string& doc) {
    imp.imprimir(doc);
}

// Una función que escanea y envía por fax pide ambas capacidades:
void escanearYEnviar(IEscaner& es, IFax& fax, const std::string& num) {
    es.escanear();
    fax.enviarFax(num);
}
```

Pasar `HPLaserSimple` a `escanearYEnviar` **ya no compila**. El compilador
te avisa antes de ejecutar nada.

## 5. Qué hemos ganado y qué hemos pagado

**Ganamos:**

- Cada clase implementa **solo lo que de verdad ofrece**. No hay métodos
  que lanzan "no soportado".
- LSP automáticamente cumplido: no hay subtipos que rompan el contrato
  porque cada contrato es pequeño y específico.
- Los clientes **dependen solo de lo que usan**. Menos acoplamiento.
- Cambios localizados: tocar `IFax` no afecta a quien solo usa `IImpresora`.

**Pagamos:**

- Más interfaces, más ficheros.
- Necesitamos **herencia múltiple** en C++ para combinar capacidades. Es
  natural en C++ con interfaces puras, pero hay que hablarlo (no es Java).
- Hay que decidir granularidad: dividir en exceso es ruido.

## 6. La herencia múltiple en C++ aquí no asusta

Si vienen de Java, el alumno puede preocuparse. Conviene aclarar:

- **El "diamante" es un problema cuando heredas múltiplemente de clases
  con estado**. Si heredas solo de **interfaces puras** (todos métodos
  virtuales puros, sin atributos), no hay diamante y no hay problema.
- Java distingue `class`/`interface` para esto mismo. En C++ no existe la
  palabra `interface`; usamos la convención: si todos los métodos son
  virtuales puros y no hay estado, **es una interfaz**.
- `virtual` en la cabecera de la herencia (`class X : virtual public IY`)
  se reserva para los casos en que sí hay diamante con estado, que casi
  no aparecen en patrones de diseño limpios.

## 7. Bad/Good número 2: Trabajador

Una `IEmpleado` que mezcla cosas que solo aplican a algunos:

### Bad

```cpp
class IEmpleado {
public:
    virtual ~IEmpleado() = default;
    virtual void trabajar() = 0;
    virtual void comer()   = 0;
    virtual void dormir()  = 0;
};

class Humano : public IEmpleado {
public:
    void trabajar() override { /* ok */ }
    void comer()   override { /* ok */ }
    void dormir()  override { /* ok */ }
};

class Robot : public IEmpleado {
public:
    void trabajar() override { /* ok */ }
    void comer()   override { /* ❌ no come */ }
    void dormir()  override { /* ❌ no duerme */ }
};
```

### Good

```cpp
class ITrabajador {
public:
    virtual ~ITrabajador() = default;
    virtual void trabajar() = 0;
};

class IAlimentable {
public:
    virtual ~IAlimentable() = default;
    virtual void comer() = 0;
};

class IDescansable {
public:
    virtual ~IDescansable() = default;
    virtual void dormir() = 0;
};

class Humano : public ITrabajador, public IAlimentable, public IDescansable {
public:
    void trabajar() override {}
    void comer()    override {}
    void dormir()   override {}
};

class Robot : public ITrabajador {
public:
    void trabajar() override {}
};
```

El gestor de turnos pide `ITrabajador&`. El comedor pide `IAlimentable&`.
**Cada cliente se queda con la pieza que necesita.**

## 8. Pista para identificar violaciones

ISP se huele cuando ves:

- **Métodos vacíos** en implementaciones (`void escanear() override {}`).
- **Excepciones de "no soportado"** lanzadas desde subclases.
- **`if (capacidadX) hacer(); else nada;`** en el cliente.
- Una **interfaz que crece sin parar** y cada vez tiene más implementaciones
  que solo usan la mitad.

## 9. Conexión con patrones

| Patrón                | Cómo se relaciona con ISP                         |
|-----------------------|---------------------------------------------------|
| **Adapter**           | Adapta una interfaz gorda exponiendo solo lo que el cliente necesita |
| **Facade**            | Esconde una API grande tras una interfaz pequeña y específica |
| **Composite**         | La interfaz común debe ser lo bastante mínima para que tanto hoja como compuesto la cumplan limpiamente |
| **Strategy**          | Strategy es esencialmente "una interfaz pequeñísima con un solo método" — ISP llevado al extremo |

## 10. Mini-ejercicio mental (3 min)

Interfaz `IRepositorio<T>`:

```cpp
template <typename T>
class IRepositorio {
public:
    virtual ~IRepositorio() = default;
    virtual void guardar(const T& obj) = 0;
    virtual T    buscar(int id) const = 0;
    virtual void eliminar(int id) = 0;
    virtual std::vector<T> listar() const = 0;
};
```

Y resulta que vamos a tener un repositorio **de solo lectura** (un caché
remoto) y otro **de solo escritura** (un log de auditoría). ¿Cómo lo
segregas?

(Respuesta breve: separa `IReadOnlyRepo<T>` (buscar, listar) e
`IWriteOnlyRepo<T>` (guardar, eliminar). `IRepositorio<T>` puede seguir
existiendo como combinación de ambas si te conviene, o desaparecer.
Los clientes piden la interfaz mínima que necesiten.)

---

## Resumen del bloque

- ISP = **muchas interfaces pequeñas, no una grande**.
- Síntomas de violación: métodos vacíos, "no soportado" lanzado, métodos
  que no tienen sentido para el implementador.
- En C++ se materializa con **herencia múltiple de interfaces puras**, sin
  los problemas que tendría en otros lenguajes con estado heredado.
- ISP suele aparecer como **consecuencia** de LSP: cuando una subclase no
  puede cumplir todo el contrato, el contrato era demasiado grande.

**Mantra del bloque:**

> *"Una interfaz pequeña es difícil de violar. Una interfaz grande es
> imposible de cumplir."*
