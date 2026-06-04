#include "paint/ShapeFactory.h"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace paint {

void ShapeFactory::Register(const std::string& name, Builder builder) {
    builders_[name] = std::move(builder);
}

bool ShapeFactory::Knows(const std::string& name) const {
    return builders_.find(name) != builders_.end();
}

std::unique_ptr<IShape> ShapeFactory::Create(const std::string& line) const {
    std::istringstream in(line);
    std::string name;
    in >> name;

    auto it = builders_.find(name);
    if (it == builders_.end())
        throw std::runtime_error("figura desconocida: " + name);

    return it->second(in);
}

}  // namespace paint
