// Pruebas mínimas con <cassert>. Cada función comprueba un patrón.
// Si alguna aserción falla, el ejecutable aborta con código != 0 y CTest
// marca el test como fallido.
#include <cassert>
#include <cmath>
#include <memory>

#include "paint/AddShapeCommand.h"
#include "paint/Canvas.h"
#include "paint/Circle.h"
#include "paint/CommandManager.h"
#include "paint/DuplicateShapeCommand.h"
#include "paint/MoveShapeCommand.h"
#include "paint/ShapeFactory.h"
#include "paint/ShapeRegistration.h"
#include "paint/Square.h"

using namespace paint;

// Factory: crea la figura correcta a partir de una línea de texto.
static void TestFactoryCreatesCircle() {
    ShapeFactory factory;
    RegisterBuiltinShapes(factory);

    auto shape = factory.Create("circle 5 2 10 20");
    assert(shape != nullptr);
    assert(shape->Color() == 2);
    assert(std::abs(shape->Area() - 3.14159265358979 * 25.0) < 1e-6);
    assert(shape->Position().x == 10 && shape->Position().y == 20);
}

// Prototype: una figura sabe clonarse manteniendo su estado.
static void TestPrototypeClone() {
    Circle original(3.0, 1, Point{4, 5});
    auto copy = original.Clone();

    assert(copy->Color() == 1);
    assert(std::abs(copy->Area() - original.Area()) < 1e-9);
    assert(copy->Position().x == 4 && copy->Position().y == 5);
}

// Command: añadir es reversible (undo/redo) sobre el Canvas singleton.
static void TestAddUndoRedo() {
    Canvas& canvas = Canvas::Instance();
    canvas.Clear();
    CommandManager manager;

    manager.Execute(std::make_unique<AddShapeCommand>(
        canvas, std::make_unique<Square>(2.0, 0, Point{0, 0})));
    assert(canvas.Count() == 1);

    assert(manager.Undo());
    assert(canvas.Count() == 0);

    assert(manager.Redo());
    assert(canvas.Count() == 1);
}

// Command: mover es reversible y restaura la posición previa.
static void TestMoveUndo() {
    Canvas& canvas = Canvas::Instance();
    canvas.Clear();
    canvas.Add(std::make_unique<Circle>(1.0, 0, Point{0, 0}));
    CommandManager manager;

    manager.Execute(std::make_unique<MoveShapeCommand>(canvas, 0, Point{7, 8}));
    assert(canvas.At(0).Position().x == 7 && canvas.At(0).Position().y == 8);

    assert(manager.Undo());
    assert(canvas.At(0).Position().x == 0 && canvas.At(0).Position().y == 0);
}

// Prototype + Command: duplicar clona y es reversible.
static void TestDuplicate() {
    Canvas& canvas = Canvas::Instance();
    canvas.Clear();
    canvas.Add(std::make_unique<Circle>(4.0, 5, Point{1, 1}));
    CommandManager manager;

    manager.Execute(std::make_unique<DuplicateShapeCommand>(canvas, 0));
    assert(canvas.Count() == 2);
    assert(canvas.At(1).Color() == 5);

    assert(manager.Undo());
    assert(canvas.Count() == 1);
}

int main() {
    TestFactoryCreatesCircle();
    TestPrototypeClone();
    TestAddUndoRedo();
    TestMoveUndo();
    TestDuplicate();
    return 0;
}
