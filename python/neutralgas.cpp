#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/neutralgas/GasType.h"
#include "hermes/neutralgas/NeutralGasAbstract.h"
#include "hermes/neutralgas/RingModel.h"

#include "hermes/neutralgas/ProfileAbstract.h"
#include "hermes/neutralgas/Nakanishi06.h"

namespace py = pybind11;

namespace hermes { namespace neutralgas {

void init(py::module &m) {
	py::module subm = m.def_submodule("neutralgas");
	subm.doc() = "neutral gas package";

	py::enum_<GasType>(subm, "GasType").value("HI", GasType::HI).value("H2", GasType::H2);

	// neutral gas density models
	// RingData
	py::class_<RingData, std::shared_ptr<RingData>>(subm, "RingData")
	    .def(py::init<GasType>())
		.def(py::init<GasType, const std::string &>())
	    .def("getHIColumnDensityInRing", &RingData::getHIColumnDensityInRing)
	    .def("getCOIntensityInRing", &RingData::getCOIntensityInRing);
	py::class_<Ring, std::shared_ptr<Ring>>(subm, "Ring")
	    .def(py::init<std::size_t, const std::shared_ptr<RingData>, QLength, QLength>())
	    .def("getBoundaries", &Ring::getBoundaries)
	    .def("isInside", &Ring::isInside)
	    // .def("getHIColumnDensity", &Ring::getHIColumnDensity)
	    // .def("getH2ColumnDensity", &Ring::getH2ColumnDensity);
		.def("getColumnDensity", &Ring::getColumnDensity);
	// NOLINTNEXTLINE(bugprone-unused-raii)

	// NeutralGasAbstract
	py::class_<NeutralGasAbstract, std::shared_ptr<NeutralGasAbstract>>(subm, "NeutralGasAbstract");

	// RingModel
	py::class_<RingModel, std::shared_ptr<RingModel>, NeutralGasAbstract>(subm, "RingModel")
	    .def(py::init<GasType>(), py::arg("ring_type"))
	    .def(py::init([](GasType gas, std::array<double, 12> arr) -> std::shared_ptr<RingModel> {
		    return std::make_shared<RingModel>(gas, arr);
	    }))
	    .def("getEnabledRings", &RingModel::getEnabledRings)
	    .def("setEnabledRings", &RingModel::setEnabledRings)
	    .def("disableRingNo", &RingModel::disableRingNo)
	    .def("enableRingNo", &RingModel::enableRingNo)
	    .def("isRingEnabled", &RingModel::isRingEnabled)
	    .def("getGasType", &RingModel::getGasType)
	    .def("getRingNumber", &RingModel::getRingNumber)
		.def("getAbundanceFractions", &RingModel::getAbundanceFractions)
	    .def("__getitem__",
	         [](const RingModel &r, std::size_t i) -> std::shared_ptr<Ring> {
		         if (i >= r.size()) throw py::index_error();
		         return r[i];
	         })
	    .def("__len__", &RingModel::size)
	    .def(
	        "__iter__", [](const RingModel &r) { return py::make_iterator(r.begin(), r.end()); },
	        py::keep_alive<0, 1>() /* Essential: keep object alive while iterator exists */);

	py::class_<ProfileAbstract, std::shared_ptr<ProfileAbstract>>(subm, "ProfileAbstract");

	// Nakanishi06
	py::class_<Nakanishi06, std::shared_ptr<Nakanishi06>, ProfileAbstract>(subm, "Nakanishi06")
	    .def(py::init<>())
	    // .def("getHIDensity", &Nakanishi06::getHIDensity)
	    // .def("getH2Density", &Nakanishi06::getH2Density)
	    .def(
			"getPDensity",
			static_cast<QPDensity (Nakanishi06::*)(GasType, const Vector3QLength&) const>(&Nakanishi06::getPDensity));

				
}

}}  // namespace hermes::neutralgas
