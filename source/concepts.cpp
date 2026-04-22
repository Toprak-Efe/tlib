#include <tlib/control/spatial.hpp>
#include <tlib/control/filters/butterworth.hpp>
#include <tlib/control/estimators/differentiate.hpp>
#include <tlib/control/estimators/integrate.hpp>

void initialization_test() {
    EulerIntegrator<Wrench> ei;
    TrapezoidalIntegrator<Wrench> ti;
}
