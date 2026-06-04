#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "paint/IShape.h"
#include "paint/IWriter.h"

namespace paint {

/// Singleton de Meyers: en el dominio del Paint solo existe un lienzo activo.
/// Las cuatro defensas (ctor privado, copia/movimiento borrados, dtor privado)
/// garantizan que nadie pueda crear un segundo lienzo.
///
/// Recordatorio honesto: Singleton acopla y complica los tests (ver
/// doc/day-03/01_singleton.md §7). Lo usamos porque "un solo lienzo" es una
/// restricción del dominio; aun así, los comandos reciben el Canvas por
/// referencia (inyección) en vez de llamar a Instance() por dentro.
class Canvas {
public:
    static Canvas& Instance();

    void Add(std::unique_ptr<IShape> shape);

    /// Extrae la figura apuntada por `handle` y devuelve su propiedad.
    /// Lo usa, por ejemplo, AddShapeCommand para deshacer un "add".
    std::unique_ptr<IShape> Remove(IShape* handle);

    /// Acceso por índice, validado. Lanza std::out_of_range si no existe.
    IShape& At(std::size_t index);

    std::size_t Count() const;
    void Clear();

    void Print(IWriter& writer) const;

    Canvas(const Canvas&) = delete;
    Canvas& operator=(const Canvas&) = delete;
    Canvas(Canvas&&) = delete;
    Canvas& operator=(Canvas&&) = delete;

private:
    Canvas() = default;
    ~Canvas() = default;

    std::vector<std::unique_ptr<IShape>> shapes_;
};

}  // namespace paint
