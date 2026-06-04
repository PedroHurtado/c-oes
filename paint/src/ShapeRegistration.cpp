#include "paint/ShapeRegistration.h"

#include <istream>
#include <memory>

#include "paint/Circle.h"
#include "paint/Point.h"
#include "paint/ShapeFactory.h"
#include "paint/Square.h"

namespace paint {

void RegisterBuiltinShapes(ShapeFactory& factory) {
    // circle <radio> <color> <x> <y>
    factory.Register("circle", [](std::istream& in) -> std::unique_ptr<IShape> {
        double radius = 0;
        int color = 0;
        Point position;
        in >> radius >> color >> position.x >> position.y;
        return std::make_unique<Circle>(radius, color, position);
    });

    // square <lado> <color> <x> <y>
    factory.Register("square", [](std::istream& in) -> std::unique_ptr<IShape> {
        double side = 0;
        int color = 0;
        Point position;
        in >> side >> color >> position.x >> position.y;
        return std::make_unique<Square>(side, color, position);
    });

    // Añadir una figura nueva (p. ej. "triangle") es otra llamada a Register
    // aquí o en otro .cpp. ShapeFactory no se toca: eso es OCP.
}

}  // namespace paint
