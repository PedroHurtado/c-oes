#pragma once

#include <iosfwd>
#include <memory>

#include "paint/CommandManager.h"
#include "paint/ShapeFactory.h"

namespace paint {

class Canvas;
class IWriter;
class ICommand;

/// Bucle de lectura/ejecución (REPL). Traduce cada línea que teclea el usuario
/// en un Command y lo entrega al CommandManager. Es el "composition root":
/// recibe el lienzo y la salida por inyección, así que es testeable.
class App {
public:
    App(Canvas& canvas, IWriter& writer);

    /// Lee líneas de `input` hasta EOF o "exit".
    void Run(std::istream& input);

private:
    void Help() const;
    void Execute(std::unique_ptr<ICommand> command);

    Canvas& canvas_;
    IWriter& writer_;
    ShapeFactory factory_;
    CommandManager commands_;
};

}  // namespace paint
