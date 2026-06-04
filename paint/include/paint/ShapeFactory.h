#pragma once

#include <functional>
#include <istream>
#include <map>
#include <memory>
#include <string>

#include "paint/IShape.h"

namespace paint {

/// Factory con registro. Añadir una figura nueva consiste en registrar un
/// "builder" (OCP): no se toca esta clase.
///
/// Nota de diseño: la fábrica es instanciable e inyectable (la app posee la
/// suya). Evitamos a propósito la fábrica 100% estática, que de facto es un
/// Singleton oculto y dificulta los tests (ver doc/day-03/03_factory.md §8).
class ShapeFactory {
public:
    /// Un builder recibe el resto de la línea ya tokenizada y construye la
    /// figura leyendo de ella sus parámetros.
    using Builder = std::function<std::unique_ptr<IShape>(std::istream&)>;

    void Register(const std::string& name, Builder builder);
    bool Knows(const std::string& name) const;

    /// Crea una figura a partir de una línea con formato:
    ///   "<nombre> <param1> <param2> ..."
    /// Lanza std::runtime_error si el nombre no está registrado.
    std::unique_ptr<IShape> Create(const std::string& line) const;

private:
    std::map<std::string, Builder> builders_;
};

}  // namespace paint
