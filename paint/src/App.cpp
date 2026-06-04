#include "paint/App.h"

#include <istream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "paint/AddShapeCommand.h"
#include "paint/Canvas.h"
#include "paint/DuplicateShapeCommand.h"
#include "paint/IWriter.h"
#include "paint/MoveShapeCommand.h"
#include "paint/ShapeRegistration.h"

namespace paint {

App::App(Canvas& canvas, IWriter& writer) : canvas_(canvas), writer_(writer) {
    RegisterBuiltinShapes(factory_);
}

void App::Execute(std::unique_ptr<ICommand> command) {
    commands_.Execute(std::move(command));
}

void App::Help() const {
    writer_.Write("Comandos disponibles:");
    writer_.Write("  circle <radio> <color> <x> <y>   anade un circulo");
    writer_.Write("  square <lado> <color> <x> <y>    anade un cuadrado");
    writer_.Write("  move <indice> <x> <y>            mueve una figura");
    writer_.Write("  duplicate <indice>               duplica una figura (Prototype)");
    writer_.Write("  undo | redo                      deshace / rehace");
    writer_.Write("  print                            lista el lienzo");
    writer_.Write("  help | exit");
}

void App::Run(std::istream& input) {
    Help();

    std::string line;
    while (std::getline(input, line)) {
        std::istringstream in(line);
        std::string verb;
        if (!(in >> verb)) continue;  // línea en blanco

        try {
            if (verb == "exit" || verb == "quit") {
                break;
            } else if (verb == "help") {
                Help();
            } else if (verb == "print" || verb == "list") {
                canvas_.Print(writer_);
            } else if (verb == "undo") {
                writer_.Write(commands_.Undo() ? "deshecho" : "nada que deshacer");
            } else if (verb == "redo") {
                writer_.Write(commands_.Redo() ? "rehecho" : "nada que rehacer");
            } else if (verb == "move") {
                std::size_t index = 0;
                Point target;
                if (!(in >> index >> target.x >> target.y)) {
                    writer_.Write("uso: move <indice> <x> <y>");
                    continue;
                }
                Execute(std::make_unique<MoveShapeCommand>(canvas_, index, target));
                canvas_.Print(writer_);
            } else if (verb == "duplicate") {
                std::size_t index = 0;
                if (!(in >> index)) {
                    writer_.Write("uso: duplicate <indice>");
                    continue;
                }
                Execute(std::make_unique<DuplicateShapeCommand>(canvas_, index));
                canvas_.Print(writer_);
            } else if (factory_.Knows(verb)) {
                // El verbo es el nombre de una figura registrada: la fábrica
                // parsea la línea completa y construye la figura.
                Execute(std::make_unique<AddShapeCommand>(canvas_, factory_.Create(line)));
                canvas_.Print(writer_);
            } else {
                writer_.Write("comando desconocido: " + verb);
            }
        } catch (const std::exception& ex) {
            writer_.Write(std::string("error: ") + ex.what());
        }
    }
}

}  // namespace paint
