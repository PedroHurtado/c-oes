#include <iostream>

#include "paint/App.h"
#include "paint/Canvas.h"
#include "paint/ConsoleWriter.h"

// Composition root: aquí se cablean las piezas concretas. Es el único sitio
// que conoce ConsoleWriter y la instancia singleton del Canvas; todo lo demás
// trabaja contra abstracciones.
int main() {
    paint::ConsoleWriter writer;
    paint::App app(paint::Canvas::Instance(), writer);
    app.Run(std::cin);
    return 0;
}
