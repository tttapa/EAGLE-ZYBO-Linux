#include <AnglePrinter.hpp>
#include <EulerAngles.hpp>
#include <PyMatrix.hpp>
#include <Quaternion.hpp>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

PYBIND11_MODULE(PyQuaternion, py_quaternion_module) {
    pybind11::class_<Quaternion>(py_quaternion_module, "Quaternion")
        .def(pybind11::init<>())
        .def(pybind11::init<const ColVector<4> &>())
        .def(pybind11::init<real_t, real_t, real_t, real_t>())
        .def("asColVector", pybind11::overload_cast<>(&Quaternion::asColVector))
        .def("asColVector", pybind11::overload_cast<>(&Quaternion::asColVector,
                                                      pybind11::const_))
        .def("normalize", &Quaternion::normalize)
        .def("conjugate", &Quaternion::conjugate)
        .def("rotate", &Quaternion::rotate<1>)
        .def("rotate", &Quaternion::rotate<2>)
        .def("rotate", &Quaternion::rotate<3>)
        .def("rotate", &Quaternion::rotate<4>)
        .def("__getitem__",
             pybind11::overload_cast<size_t>(&Quaternion::operator[]))
        .def("__getitem__", pybind11::overload_cast<size_t>(
                                &Quaternion::operator[], pybind11::const_))
        .def("__setitem__",
             [](Quaternion &q, size_t i, real_t value) { return q[i] = value; })
        .def("__len__", []() { return 4; })
        .def(pybind11::self + pybind11::self)
        .def(pybind11::self - pybind11::self)
        .def(-pybind11::self)
        .def(pybind11::self == pybind11::self)
        .def(pybind11::self != pybind11::self)
        .def("__str__",
             [](const Quaternion &q) {
                 std::ostringstream ss;
                 ss << asEulerAngles(q, AngleFormat::degrees);
                 return ss.str();
             })
        .def("__repr__",
             [](const Quaternion &q) {
                 std::ostringstream ss;
                 ss << "Q(" << q[0] << ", " << q[1] << ", " << q[2] << ", "
                    << q[3] << ")";
                 return ss.str();
             })
        .def_static("identity", &Quaternion::identity)
        .def_static("fromDirection", &Quaternion::fromDirection);

    pybind11::class_<EulerAngles>(py_quaternion_module, "EulerAngles")
        .def(pybind11::init<>())
        .def(pybind11::init<Quaternion>())
        .def(pybind11::init<real_t, real_t, real_t>())
        .def("asColVector", &EulerAngles::asColVector)
        .def("__str__",
             [](const EulerAngles &q) {
                 std::ostringstream ss;
                 ss << asEulerAngles(q, AngleFormat::degrees);
                 return ss.str();
             })
        .def_readwrite("yaw", &EulerAngles::yaw)
        .def_readwrite("pitch", &EulerAngles::pitch)
        .def_readwrite("roll", &EulerAngles::roll)
        .def_static("quat2eul", &EulerAngles::quat2eul)
        .def_static("eul2quat", &EulerAngles::eul2quat);
}