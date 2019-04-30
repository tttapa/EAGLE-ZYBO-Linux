#pragma once

#define TEMPLATE_GRIDFINDER

#include <AngleTracker.hpp>
#include <GridFinder.hpp>
#include <Line.hpp>
#include <LocationFinder.hpp>
#include <PyMatrix.hpp>
#include <pybind11/stl.h>
#include <sstream>

PYBIND11_MODULE(py_grid_finder, pygridmodule) {
    pybind11::class_<GridFinder>(pygridmodule, "GridFinder")
        .def(pybind11::init<const Mask &>())
        .def("findSquare", &GridFinder::findSquare);

    pybind11::class_<LineResult>(pygridmodule, "LineResult")
        .def("getLineCenter", [](LineResult r) { return r.lineCenter; })
        .def("getWidth", [](LineResult r) { return r.width; })
        .def("getAngle", [](LineResult r) { return r.angle.rad(); })
        .def("__str__", [](LineResult r) {
            std::ostringstream s;
            s << r;
            return s.str();
        });

    pygridmodule.def("intersect", [](LineResult a, LineResult b) {
        Point intersection = Line::intersect(Line(a.lineCenter, a.angle),
                                             Line(b.lineCenter, b.angle));
        return std::make_tuple<int, int>(std::round(intersection.x),
                                         std::round(intersection.y));
    });

    pybind11::class_<Square>(pygridmodule, "Square")
        .def_readonly("lines", &Square::lines)
        .def_readonly("points", &Square::points)
        .def("getAngle", [](Square &s) { return s.getAngle().rad(); })
        .def("__str__", [](const Square &sq) {
            std::ostringstream s;
            s << sq;
            return s.str();
        });

    pybind11::class_<Pixel>(pygridmodule, "Pixel")
        .def_readwrite("x", &Pixel::x)
        .def_readwrite("y", &Pixel::y)
        .def("__str__", [](Pixel p) {
            std::ostringstream s;
            s << p;
            return s.str();
        });

    pybind11::class_<Point>(pygridmodule, "Point")
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y)
        .def("__str__", [](Point p) {
            std::ostringstream s;
            s << p;
            return s.str();
        });

    pybind11::class_<Mask>(pygridmodule, "Mask", pybind11::buffer_protocol())
        .def("__init__",
             [](Mask &m,
                pybind11::array_t<uint8_t, pybind11::array::c_style |
                                               pybind11::array::forcecast>
                    a) {
                 auto info = a.request();
                 if (info.ndim != 3)
                     throw std::runtime_error("3D-array expected");
                 auto shape = info.shape;
                 new (&m) Mask(a.data(), shape[0], shape[1]);
             })
        .def_buffer([](Mask &m) -> pybind11::buffer_info {
            return pybind11::buffer_info(
                m.ptr(),         /* Pointer to buffer */
                sizeof(uint8_t), /* Size of one scalar */
                pybind11::format_descriptor<uint8_t>::
                    format(), /* Python struct-style format descriptor */
                2,            /* Number of dimensions */
                {m.height(), m.width()}, /* Buffer dimensions */
                {sizeof(uint8_t) * (m.width()), sizeof(uint8_t) * (1)}
                /* Strides (in bytes) for each index */
            );
        });

    pybind11::class_<AngleTracker>(pygridmodule, "AngleTracker")
        .def(pybind11::init<>())
        .def("update",
             [](AngleTracker a, double rad) { return a.update(rad).rad(); });

    pybind11::class_<LocationFinder>(pygridmodule, "LocationFinder")
        .def(pybind11::init<std::string>())
        .def(pybind11::init<int>())
        .def("getLocation", &LocationFinder::getLocation)
        .def("getAngle", [](LocationFinder l) { return l.getAngle().rad(); });
}