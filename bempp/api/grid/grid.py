"""The basic grid class"""
import collections as _collections

import numba as _numba
import numpy as _np

EDGES_ID = 2
VERTICES_ID = 1

_EDGE_LOCAL = _np.array([[0, 1], [2, 0], [1, 2]])


class Grid(object):
    """The Grid class."""

    def __init__(self, vertices, elements, domain_indices=None):
        """Create a grid from a vertices and an elements array."""
        self._vertices = None
        self._elements = None
        self._domain_indices = None
        self._edges = None
        self._element_edges = None
        self._edge_adjacency = None
        self._vertex_adjacency = None
        self._element_neighbors = None
        self._vertex_on_boundary = None
        self._edge_on_boundary = None
        self._edge_neighbors = None

        self._volumes = None
        self._normals = None
        self._jacobians = None
        self._jacobian_inverse_transposed = None
        self._diameters = None
        self._integration_elements = None
        self._centroids = None

        self._device_interfaces = {}

        self._normalize_and_assign_input(vertices, elements, domain_indices)
        self._enumerate_edges()

        self._get_element_adjacency_for_edges_and_vertices()
        self._compute_geometric_quantities()

        self._compute_boundary_information()

        self._compute_edge_neighbors()

        self._grid_data = GridData(
            self._vertices,
            self._elements,
            self._volumes,
            self._normals,
            self._jacobians,
            self._jacobian_inverse_transposed,
            self._diameters,
            self._integration_elements,
            self._centroids,
            self._domain_indices
        )

    @property
    def vertex_adjacency(self):
        """
        Vertex adjacency information.

        Returns a matrix with 4 rows. Each column has the entries e0,
        e1, ind0, ind1, which means that element e0 is connected to
        element e1 via local vertex index ind0 in e0 and ind1 in e1.
        Only returnes connectivity via a single vertex. For
        connectivity via edges see edge_adjacency.

        """
        return self._vertex_adjacency

    @property
    def edge_adjacency(self):
        """
        Edge adjacency information.

        Returns a matrix with 6 rows. Each column has the entries e0,
        e1, v00, v01, v11, v12, which means that element e0 is
        connected to element e1. Vertex v00 in element e0 is
        identical to vertex v11 in element e1, and vertex v01 in
        element 0 is identical to vertex v12 in element e1.
        """
        return self._edge_adjacency

    @property
    def element_neighbors(self):
        """
        Return list of element neighbors.

        For element i the value of element_neighbors[i] is a set
        that contains all elements different from i that share at
        least one vertex with element i.
        """
        return self._element_neighbors

    @property
    def number_of_vertices(self):
        """Number of vertices."""
        return self._vertices.shape[1]

    @property
    def number_of_edges(self):
        """Number of edges."""
        return self._edges.shape[1]

    @property
    def number_of_elements(self):
        """Return number of elements."""
        return self._elements.shape[1]

    @property
    def vertices(self):
        """Return vertices."""
        return self._vertices

    @property
    def elements(self):
        """Return elements."""
        return self._elements

    @property
    def edges(self):
        """Return edges."""
        return self._edges

    @property
    def centroids(self):
        """Return the centroids of the elements."""
        return self._centroids

    @property
    def domain_indices(self):
        """Return domain indices."""
        return self._domain_indices

    @property
    def element_edges(self):
        """
        Returns an array of edge indices for each element.

        element_edges[i, j] is the index of the ith edge
        in the jth element.

        """
        return self._element_edges

    @property
    def device_interfaces(self):
        """Return the dictionary of device interfaces for the grid."""
        return self._device_interfaces

    @property
    def as_array(self):
        """
        Converts the grid to an array.

        For a grid with N elements returns a 1d array with
        9 * N entries. The three nodes for element with index e
        can be found in [9 * e, 9 * (e + 1)].

        """
        return self.vertices.T[self.elements.flatten(order="F"), :].flatten(order="C")

    @property
    def bounding_box(self):
        """
        Return the bounding box for the grid.

        The bounding box is a 3x2 array box such that
        box[:, 0] contains (xmin, ymin, zmin) and box[:, 1]
        contains (xmax, ymax, zmax).

        """

        box = _np.empty((3, 2), dtype="float64")
        box[:, 0] = _np.min(self.vertices, axis=1)
        box[:, 1] = _np.max(self.vertices, axis=1)

        return box

    @property
    def volumes(self):
        """Return element volumes."""
        return self._volumes

    @property
    def diameters(self):
        """Return element diameters."""
        return self._diameters

    @property
    def maximum_element_diameter(self):
        """Return the maximum element diameter."""
        return _np.max(self.diameters)

    @property
    def minimum_element_diameter(self):
        """Return the maximum element diameter."""
        return _np.min(self.diameters)

    @property
    def normals(self):
        """Return normals."""
        return self._normals

    @property
    def jacobians(self):
        """Return Jacobians."""
        return self._jacobians

    @property
    def integration_elements(self):
        """Return integration elements."""
        return self._integration_elements

    @property
    def jacobian_inverse_transposed(self):
        """Return the jacobian inverse transposed."""
        return self._jacobian_inverse_transposed

    @property
    def vertex_on_boundary(self):
        """Return vertex boundary information."""
        return self._vertex_on_boundary

    @property
    def edge_on_boundary(self):
        """Return edge boundary information."""
        return self._edge_on_boundary

    @property
    def edge_neighbors(self):
        """Return for each edge the list of neighboring elements.."""
        return self._edge_neighbors

    @property
    def data(self):
        """Return Numba container with all relevant grid data."""
        return self._grid_data

    def entity_count(self, codim):
        """Return the number of entities of given codimension."""

        if codim == 0:
            return self.number_of_elements
        if codim == 1:
            return self.number_of_edges
        if codim == 2:
            return self.number_of_vertices

        raise ValueError("codim must be one of 0, 1, or 2.")

    def plot(self):
        """Plot the grid."""
        from bempp.api.external.viewers import visualize

        visualize(self)

    def get_element(self, index):
        """Return element with a given index."""
        return Element(self, index)

    def entity_iterator(self, codim):
        """Return an iterator for a given codim."""

        def element_iterator():
            """Iterate over elements."""
            for index in range(self.number_of_elements):
                yield Element(self, index)

        def vertex_iterator():
            """Iterate over vertices."""
            for index in range(self.number_of_vertices):
                yield Vertex(self, index)

        def edge_iterator():
            """Iterate over edges."""
            for index in range(self.number_of_edges):
                yield Edge(self, index)

        if codim not in [0, 1, 2]:
            raise ValueError("codim must be one of 0, 1, or 2.")

        if codim == 0:
            iterator = element_iterator()
        elif codim == 1:
            iterator = edge_iterator()
        elif codim == 2:
            iterator = vertex_iterator()

        return iterator

    def refine(self):
        """Return a new grid with all elements refined."""

        new_number_of_vertices = self.number_of_edges + self.number_of_vertices

        new_vertices = _np.empty(
            (3, new_number_of_vertices), dtype="float64", order="F"
        )

        new_vertices[:, : self.number_of_vertices] = self.vertices

        # Each edge midpoint forms a new vertex.
        new_vertices[:, self.number_of_vertices :] = 0.5 * (
            self.vertices[:, self.edges[0, :]] + self.vertices[:, self.edges[1, :]]
        )

        new_elements = _np.empty(
            (3, 4 * self.number_of_elements), order="F", dtype="uint32"
        )

        new_domain_indices = _np.repeat(self.domain_indices, 4)

        for index, elem in enumerate(self.elements.T):
            vertex0 = elem[0]
            vertex1 = elem[1]
            vertex2 = elem[2]
            vertex01 = self.element_edges[0, index] + self.number_of_vertices
            vertex20 = self.element_edges[1, index] + self.number_of_vertices
            vertex12 = self.element_edges[2, index] + self.number_of_vertices

            new_elements[:, 4 * index] = [vertex0, vertex01, vertex20]

            new_elements[:, 4 * index + 1] = [vertex01, vertex1, vertex12]

            new_elements[:, 4 * index + 2] = [vertex12, vertex2, vertex20]

            new_elements[:, 4 * index + 3] = [vertex01, vertex12, vertex20]

        return Grid(new_vertices, new_elements, new_domain_indices)

    def push_to_device(self, device_interface, precision):
        """
        Copy serialized grid onto device.

        The property as_array is used to create a representation
        of the grid, which is pushed onto a given device.
        The method returns a DeviceGridInterface object. If
        the grid with the given precision is already in the context, 
        an existing DeviceGridInterface instance is returned to avoid
        multiple copies of the grid in the same context.
        """
        from bempp.core.device_grid_interface import DeviceGridInterface

        context = device_interface.context

        if (context, precision) in self.device_interfaces.keys():
            return self.device_interfaces[(context, precision)]

        interface = DeviceGridInterface(self, device_interface, precision)
        self.device_interfaces[(device_interface.context, precision)] = interface
        return interface

    def _normalize_and_assign_input(self, vertices, elements, domain_indices):
        """Convert input into the right form."""
        from bempp.api.utils.helpers import align_array

        if domain_indices is None:
            domain_indices = _np.zeros(elements.shape[1], dtype="uint32")

        self._vertices = align_array(vertices, "float64", "F")
        self._elements = align_array(elements, "uint32", "F")
        self._domain_indices = align_array(domain_indices, "uint32", "F")

    def _enumerate_edges(self):
        """
        Enumerate all edges in a given grid.

        Assigns a tuple (edges, element_edges) to
        self._edges and self._element_edges.
        element_edges is an array a such that a[i, j] is the
        index of the ith edge in the jth elements, and edges
        is a 2 x nedges array such that the jth column stores the
        two nodes associated with the jth edge.

        """
        not_found = -1

        edges = []
        edge_tuple_to_index = {}

        number_of_elements = self._elements.shape[1]
        element_edges = _np.zeros((3, number_of_elements), dtype="int32")

        number_of_edges = 0

        for elem_index, elem in enumerate(self._elements.T):
            for local_index in range(3):
                edge_tuple = _vertices_from_edge_index(elem, local_index)
                edge_index = edge_tuple_to_index.get(edge_tuple, not_found)
                if edge_index == not_found:
                    edge_index = number_of_edges
                    edge_tuple_to_index[edge_tuple] = edge_index
                    edges.append(edge_tuple)
                    number_of_edges += 1
                element_edges[local_index, elem_index] = edge_index

        self._edges = _np.array(edges, dtype="int32").T
        self._element_edges = element_edges

    def _get_element_adjacency_for_edges_and_vertices(self):
        """
        The array edge_adjacency has 6 rows, such that for index j the
        element edge_adjacency[0, j] is connected with element
        edge_adjacency[1, j] via the vertices edge_adjacency[2:4, j]
        in the first element and the vertices edge_adjacency[4:6, j]
        in the second element. The vertex numbers here are local
        numbers (0, 1 or 2).

        The array vertex_adjacency has 4 rows, such that for index j the
        element vertex_adjacency[0, j] is connected with
        vertex_adjacency[1, j] via the vertex vertex_adjacency[2, j]
        in the first element and the vertex vertex_adjacency[3, j]
        in the second element. The vertex numbers here are local numbers
        (0, 1 or 2).

        The list element_neighbors is defined such that element_neighbors[i]
        contains the element indices of all elements which share at least one
        vertex with element i.

        """
        elem_to_elem_matrix = get_element_to_element_matrix(
            self._vertices, self._elements
        )

        elements1, elements2, nvertices = _get_element_to_element_vertex_count(
            elem_to_elem_matrix
        )

        vertex_connected_elements1, vertex_connected_elements2 = _element_filter(
            elements1, elements2, nvertices, VERTICES_ID
        )

        edge_connected_elements1, edge_connected_elements2 = _element_filter(
            elements1, elements2, nvertices, EDGES_ID
        )

        self._vertex_adjacency = _find_vertex_adjacency(
            self._elements, vertex_connected_elements1, vertex_connected_elements2
        )

        self._edge_adjacency = _find_edge_adjacency(
            self._elements, edge_connected_elements1, edge_connected_elements2
        )

        self._element_neighbors = _get_element_neighbors(elem_to_elem_matrix)

    def _compute_geometric_quantities(self):
        """Compute geometric quantities for the grid."""

        element_vertices = self.vertices.T[self.elements.flatten(order="F")]
        indexptr = 3 * _np.arange(self.number_of_elements)
        indices = _np.repeat(indexptr, 2) + _np.tile([1, 2], self.number_of_elements)

        centroids = (
            1.0
            / 3
            * _np.sum(
                _np.reshape(element_vertices, (self.number_of_elements, 3, 3)), axis=1
            )
        )

        jacobians = (element_vertices - _np.repeat(element_vertices[::3], 3, axis=0))[
            indices
        ]

        normal_directions = _np.cross(jacobians[::2], jacobians[1::2], axis=1)
        normal_direction_norms = _np.linalg.norm(normal_directions, axis=1)
        normals = normal_directions / _np.expand_dims(normal_direction_norms, 1)

        volumes = 0.5 * normal_direction_norms

        jacobian_diff = jacobians[::2] - jacobians[1::2]
        diff_norms = _np.linalg.norm(jacobian_diff, axis=1)
        jac_vector_norms = _np.linalg.norm(jacobians, axis=1)

        diameters = (
            jac_vector_norms[::2]
            * jac_vector_norms[1::2]
            * diff_norms
            / normal_direction_norms
        )

        self._volumes = volumes
        self._normals = normals
        self._jacobians = _np.swapaxes(
            _np.reshape(jacobians, (self.number_of_elements, 2, 3)), 1, 2
        )
        self._diameters = diameters
        self._centroids = centroids

        jac_transpose_jac = _np.empty((self.number_of_elements, 2, 2), dtype="float64")
        for index in range(self.number_of_elements):
            jac_transpose_jac[index] = self.jacobians[index].T.dot(
                self.jacobians[index]
            )
        self._integration_elements = _np.sqrt(_np.linalg.det(jac_transpose_jac))

        jac_transpose_jac_inv = _np.linalg.inv(jac_transpose_jac)

        self._jacobian_inverse_transposed = _np.empty(
            (self.number_of_elements, 3, 2), dtype="float64"
        )

        for index in range(self.number_of_elements):
            self._jacobian_inverse_transposed[index] = self.jacobians[index].dot(
                jac_transpose_jac_inv[index]
            )

    def _compute_boundary_information(self):
        """
        Return a boolean array with boundary information.

        Computes arr0, arr1 such that arr0[j] is True if
        vertex j lies on the boundary and arr1[i] is True if edge
        i lies on the boundary.
        """
        from scipy.sparse import csr_matrix

        element_edges = self.element_edges

        number_of_elements = self.number_of_elements
        number_of_edges = self.number_of_edges
        number_of_vertices = self.number_of_vertices
        edge_indices = _np.ravel(element_edges, order="F")
        repeated_element_indices = _np.repeat(_np.arange(number_of_elements), 3)
        data = _np.ones(3 * number_of_elements, dtype="uint32")

        element_to_edge = csr_matrix(
            (data, (repeated_element_indices, edge_indices)),
            shape=(number_of_elements, number_of_edges),
        )

        edge_to_edge = element_to_edge.T.dot(element_to_edge)
        arr1 = edge_to_edge.diagonal() == 1
        arr0 = _np.zeros(number_of_vertices, dtype=_np.bool)

        for boundary_edge_index in _np.flatnonzero(arr1):
            arr0[self.edges[:, boundary_edge_index]] = True

        self._vertex_on_boundary = arr0
        self._edge_on_boundary = arr1

    def _compute_edge_neighbors(self):
        """Get the neighbors of each edge."""
        self._edge_neighbors = [[] for _ in range(self.number_of_edges)]

        for element_index in range(self.number_of_elements):
            for local_index in range(3):
                self._edge_neighbors[
                    self.element_edges[local_index, element_index]
                ].append(element_index)


@_numba.jitclass(
    [
        ("vertices", _numba.float64[:, :]),
        ("elements", _numba.uint32[:, :]),
        ("volumes", _numba.float64[:]),
        ("normals", _numba.float64[:, :]),
        ("jacobians", _numba.float64[:, :, :]),
        ("jac_inv_trans", _numba.float64[:, :, :]),
        ("diameters", _numba.float64[:]),
        ("integration_elements", _numba.float64[:]),
        ("centroids", _numba.float64[:, :]),
        ("domain_indices", _numba.uint32[:])
    ]
)
class GridData(object):
    """A Numba container class for the grid data."""

    def __init__(
        self,
        vertices,
        elements,
        volumes,
        normals,
        jacobians,
        jac_inv_trans,
        diameters,
        integration_elements,
        centroids,
        domain_indices
    ):

        self.vertices = vertices
        self.elements = elements
        self.volumes = volumes
        self.normals = normals
        self.jacobians = jacobians
        self.jac_inv_trans = jac_inv_trans
        self.diameters = diameters
        self.integration_elements = integration_elements
        self.centroids = centroids
        self.domain_indices = domain_indices


class ElementGeometry(object):
    """Provides geometry information for an element."""

    def __init__(self, grid, index):
        """Initialize geometry wth a 3x3 array of corners."""

        self._grid = grid
        self._index = index

    @property
    def corners(self):
        """Return corners."""
        return self._grid.vertices[:, self._grid.elements[:, self._index]]

    @property
    def jacobian(self):
        """Return jacobian."""
        return self._grid.jacobians[self._index]

    @property
    def integration_element(self):
        """Return integration element."""
        return self._grid.integration_elements[self._index]

    @property
    def jacobian_inverse_transposed(self):
        """Return Jacobian inverse transposed."""
        return self._grid.jacobian_inverse_transposed[self._index]

    @property
    def normal(self):
        """Return normal."""
        return self._grid.normals[self._index]

    @property
    def volume(self):
        """Return volume."""
        return self._grid.volumes[self._index]

    @property
    def diameter(self):
        """Return the diameter of the circumcircle."""
        return self._grid.diameters[self._index]

    @property
    def centroid(self):
        """Return the centroid of the element."""
        return self._grid.centroids[self._index]

    def local2global(self, points):
        """Map points in local coordinates to global."""
        return _np.expand_dims(self.corners[:, 0], 1) + self.jacobian @ points


class Element(object):
    """Provides a view onto an element of the grid."""

    def __init__(self, grid, index):

        self._grid = grid
        self._index = index

    @property
    def index(self):
        """Index of the element."""
        return self._index

    @property
    def grid(self):
        """Associated grid."""
        return self._grid

    @property
    def geometry(self):
        """Return geometry."""
        grid = self._grid
        return ElementGeometry(grid, self.index)

    @property
    def domain_index(self):
        """Return the domain index."""
        return self._grid.domain_indices[self.index]

    @property
    def neighbors(self):
        """
        Return a list of neighboring elements.

        Two elements are neighbors if they share at least one vertex.
        """
        return [
            Element(self._grid, index)
            for index in self.grid.element_neighbors[self.index]
        ]

    def sub_entity_iterator(self, codim):
        """Return iterator over subentitites."""

        def edge_iterator():
            """Iterate over edges."""
            for index in self._grid.element_edges[:, self.index]:
                yield Edge(self._grid, index)

        def vertex_iterator():
            """Iterate over vertices."""
            for index in self._grid.elements[:, self.index]:
                yield Vertex(self._grid, index)

        if codim not in [1, 2]:
            raise ValueError("codim must be 1 (for edges) or 2 (for vertices)")

        if codim == 1:
            iterator = edge_iterator()

        if codim == 2:
            iterator = vertex_iterator()

        return iterator

    def __eq__(self, other):
        """Equality comparison of elements."""

        if isinstance(other, Element):
            if other.grid == self.grid and other.index == self.index:
                return True
        return False


VertexGeometry = _collections.namedtuple("VertexGeometry", "corners")


class Vertex(object):
    """Provides a view onto a vertex of the grid."""

    def __init__(self, grid, index):

        self._grid = grid
        self._index = index

    @property
    def index(self):
        """Index of the vertex."""
        return self._index

    @property
    def geometry(self):
        """Return geometry."""
        return VertexGeometry(self._grid.vertices[:, self.index].reshape(3, 1))


class EdgeGeometry(object):
    """Implementation of a geometry for edges."""

    def __init__(self, corners):

        self._corners = corners
        self._volume = _np.linalg.norm(corners[:, 1] - corners[:, 0])

    @property
    def corners(self):
        """Return the corners."""
        return self._corners

    @property
    def volume(self):
        """Return length of the edge."""
        return self._volume


class Edge(object):
    """Provides a view onto an edge of the grid."""

    def __init__(self, grid, index):
        """Describes an edge."""

        self._grid = grid
        self._index = index

    @property
    def index(self):
        """Index of the edge."""
        return self._index

    @property
    def geometry(self):
        """Return geometry."""
        grid = self._grid
        return EdgeGeometry(grid.vertices[:, grid.edges[:, self.index]])


def get_vertex_to_element_matrix(vertices, elements):
    """Return the sparse matrix mapping vertices to elements."""
    from scipy.sparse import csr_matrix

    number_of_elements = elements.shape[1]
    number_of_vertices = vertices.shape[1]
    vertex_indices = _np.ravel(elements, order="F")
    vertex_element_indices = _np.repeat(_np.arange(number_of_elements), 3)
    data = _np.ones(len(vertex_indices), dtype="uint32")

    return csr_matrix(
        (data, (vertex_indices, vertex_element_indices)),
        shape=(number_of_vertices, number_of_elements),
        dtype="uint32",
    )


def get_element_to_element_matrix(vertices, elements):
    """
    Return element to element matrix.

    If entry (i,j) has the value n > 0, element i
    and element j are connected via n vertices.

    """
    vertex_to_element = get_vertex_to_element_matrix(vertices, elements)
    return vertex_to_element.T.dot(vertex_to_element)


@_numba.njit(cache=True, locals={"index": _numba.types.int32})
def _compare_array_to_value(array, val):
    """
    Return i such that array[i] == val.

    If val not found return -1
    """
    for index, elem in enumerate(array):
        if elem == val:
            return index
    return -1


@_numba.njit(
    cache=True,
    locals={
        "index1": _numba.types.int32,
        "index2": _numba.types.int32,
        "full_index1": _numba.types.int32,
    },
)
def _find_first_common_array_index_pair_from_position(array1, array2, start=0):
    """
    Return first index pair (i, j) such that array1[i] = array2[j].

    Assumes that one index pair satisfying the equality
    always exists. Method checks in array1 from position start
    onwards.
    """
    for index1 in range(len(array1[start:])):
        full_index1 = index1 + start
        index2 = _compare_array_to_value(array2, array1[full_index1])
        if index2 != -1:
            return (full_index1, index2)
    raise ValueError("Could not find a common index pair.")


@_numba.njit(cache=True, locals={"offset": _numba.types.int32})
def _find_two_common_array_index_pairs(array1, array2):
    """
    Return two index pairs (i, j) such that array1[i] = array2[j]
    """
    offset = 0
    index_pairs = _np.empty((2, 2), dtype=_np.int32)
    index_pairs[:, 0] = _find_first_common_array_index_pair_from_position(
        array1, array2, offset
    )
    offset = index_pairs[0, 0] + 1  # Next search starts behind found pair
    index_pairs[:, 1] = _find_first_common_array_index_pair_from_position(
        array1, array2, offset
    )
    return index_pairs


@_numba.njit(cache=True)
def _get_shared_vertex_information_for_two_elements(elements, elem0, elem1):
    """
    Return tuple (i, j)

    The tuple has the property elements[i, elem0] == elements[j, elem1]
    """
    i, j = _find_first_common_array_index_pair_from_position(
        elements[:, elem0], elements[:, elem1]
    )
    return (i, j)


@_numba.njit(cache=True)
def _get_shared_edge_information_for_two_elements(elements, elem0, elem1):
    """
    Return 2x2 array of int32 indices

    Each column in the return indices as a pair (i, j) such that
    elements[i, elem0] = elements[j, elem1]

    """
    index_pairs = _find_two_common_array_index_pairs(
        elements[:, elem0], elements[:, elem1]
    )
    return index_pairs


@_numba.njit(cache=True)
def _find_vertex_adjacency(elements, test_indices, trial_indices):
    """
    Return for element pairs the vertex adjacency.

    The return array vertex_adjacency has 4 rows, such that for index j
    the element vertex_adjacency[0, j] is connected with
    vertex_adjacency[1, j] via the vertex vertex_adjacency[2, j] in
    the first element and the vertex vertex_adjacency[3, j] in the
    second element. The vertex numbers here are local
    numbers (0, 1 or 2).

    """

    number_of_indices = len(test_indices)
    adjacency = _np.zeros((4, number_of_indices), dtype=_np.int32)

    for index in range(number_of_indices):
        # Now find the position of the shared vertex
        test_index = test_indices[index]
        trial_index = trial_indices[index]
        i, j = _get_shared_vertex_information_for_two_elements(
            elements, test_index, trial_index
        )
        adjacency[:, index] = (test_index, trial_index, i, j)

    return adjacency


@_numba.njit(cache=True)
def _find_edge_adjacency(elements, elem0_indices, elem1_indices):
    """
    Return for element pairs the edge adjacency

    The return array edge_adjacency has 6 rows, such that for index
    j the element edge_adjacency[0, j] is connected with
    edge_adjacency[1, j] via the two vertices edge_adjacency[2:4, j]
    in the first element and the vertices edge_adjacency[4:6, j]
    in the second element. The vertex numbers here are local
    numbers (0, 1 or 2).

    """

    number_of_indices = len(elem0_indices)

    adjacency = _np.zeros((6, number_of_indices), dtype=_np.int32)

    for index in range(number_of_indices):
        elem0 = elem0_indices[index]
        elem1 = elem1_indices[index]
        index_pairs = _get_shared_edge_information_for_two_elements(
            elements, elem0, elem1
        )
        adjacency[0, index] = elem0
        adjacency[1, index] = elem1
        adjacency[2:, index] = index_pairs.flatten()

    return adjacency


def _get_element_to_element_vertex_count(element_to_element_matrix):
    """
    Return a tuple of arrays (elements1, elements2, nvertices).

    The element elements1[i] is connected with elements2[i] via
    nvertices[i] vertices.

    """
    coo_matrix = element_to_element_matrix.tocoo()
    elements1 = coo_matrix.row
    elements2 = coo_matrix.col
    nvertices = coo_matrix.data

    return (elements1, elements2, nvertices)


def _element_filter(elements1, elements2, nvertices, filter_type):
    """
    Return element pairs according to a filter condition.

    Takes an array (elements1, elements2, nvertices)
    such that elements1[i] and elements2[i] are connected
    via nvertices[i] vertices and returns a tuple
    (new_elem1, new_elem2) of all element pairs connected via
    vertices (filter_type=VERTICES) or edges (filter_type=EDGES).

    """
    # Elements connected via edges share two vertices
    filtered_indices = _np.argwhere(nvertices == filter_type).flatten()
    return (elements1[filtered_indices], elements2[filtered_indices])


@_numba.njit(cache=True)
def _sort_values(val1, val2):
    """Return a tuple with the input values sorted."""
    if val1 > val2:
        val1, val2 = val2, val1
    return val1, val2


@_numba.njit(cache=True)
def _vertices_from_edge_index(element, local_index):
    """
    Returns the vertices associated with an edge.

    Element is 3-tupel with the vertex indices.
    Sorts the returned vertices in ascending order.

    """
    vertex0, vertex1 = element[_EDGE_LOCAL[local_index]]
    return _sort_values(vertex0, vertex1)


def _get_element_neighbors(element_to_element_matrix):
    """Return a list of neighbors for each element."""

    number_of_elements = element_to_element_matrix.shape[0]
    indices = element_to_element_matrix.indices
    indptr = element_to_element_matrix.indptr
    neighbors = [[] for _ in range(number_of_elements)]

    for element_index in range(number_of_elements):
        neighbors[element_index] = set(
            indices[indptr[element_index] : indptr[element_index + 1]]
        )
        # We don't want the element itself in the neighbors set.
        neighbors[element_index].remove(element_index)

    return neighbors