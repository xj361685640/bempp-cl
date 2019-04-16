"""Definition of a piecewise constant function space."""

# pylint: disable=unused-argument

import numpy as _np
import numba as _numba

from .space import _FunctionSpace, _SpaceData


class P0DiscontinuousFunctionSpace(_FunctionSpace):
    """A space of piecewise constant functions."""

    def __init__(self, grid):
        """Initialize with a given grid."""
        from scipy.sparse import identity

        shapeset = "p0_discontinuous"

        global_dof_count = grid.entity_count(0)

        local2global_map = _np.expand_dims(
            _np.arange(global_dof_count, dtype="uint32"), 1
        )

        local_multipliers = _np.ones((global_dof_count, 1), dtype="float64")

        support = _np.full(grid.number_of_elements, True, dtype=bool)

        codomain_dimension = 1
        order = 0
        identifier = "p0_discontinuous"

        localised_space = self

        color_map = _np.zeros(grid.entity_count(0), dtype="uint32")

        map_to_localised_space = identity(
            grid.number_of_elements, dtype="float64", format="csr"
        )

        space_data = _SpaceData(
            grid,
            codomain_dimension,
            global_dof_count,
            order,
            shapeset,
            local2global_map,
            local_multipliers,
            identifier,
            support,
            localised_space,
            color_map,
            map_to_localised_space,
        )

        super().__init__(space_data)

    @property
    def numba_evaluate(self):
        """Return numba method that evaluates the basis."""
        return _numba_evaluate

    @property
    def numba_surface_gradient(self):
        """Return numba method that evaluates the surface gradient."""
        return _numba_surface_gradient

    def evaluate(self, element, local_coordinates):
        """Evaluate the basis on an element."""
        return _numba_evaluate(
            element.index,
            self.shapeset.evaluate,
            local_coordinates,
            self.grid.data,
            self.local_multipliers,
        )

    def surface_gradient(self, element, local_coordinates):
        """Return the surface gradient."""
        return _numba_surface_gradient(
            element.index,
            self.shapeset.gradient,
            local_coordinates,
            self.grid.data,
            self.local_multipliers,
        )


@_numba.njit
def _numba_evaluate(
    element_index, shapeset_evaluate, local_coordinates, grid_data, local_multipliers
):
    """Evaluate the basis on an element."""
    return shapeset_evaluate(local_coordinates)


@_numba.njit
def _numba_surface_gradient(
    element_index, shapeset_gradient, local_coordinates, grid_data, local_multipliers
):
    """Evaluate the surface gradient."""
    return _np.zeros((1, 3, 1, local_coordinates.shape[1]), dtype=_np.float64)