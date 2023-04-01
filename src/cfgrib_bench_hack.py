import cfgrib
import typing as T
import logging
import numpy as np


# Hack the cfgrib function that build lat/lon coordinates, to test the effect of loading 
# the data without the performance bug
def build_geography_coordinates(
    first: cfgrib.abc.Field,
    encode_cf: T.Sequence[str],
    errors: str,
    log: logging.Logger = cfgrib.dataset.LOG,
) -> T.Tuple[T.Tuple[str, ...], T.Tuple[int, ...], T.Dict[str, cfgrib.dataset.Variable]]:
    geo_coord_vars = {}  # type: T.Dict[str, Variable]

    geo_dims = ("y", "x")
    geo_shape = (first["Ny"], first["Nx"])
    geo_coord_vars["latitude"] = cfgrib.dataset.Variable(
        dimensions=("y", "x"),
        data=np.ones(geo_shape),
        attributes=cfgrib.dataset.COORD_ATTRS["latitude"],
    )
    geo_coord_vars["longitude"] = cfgrib.dataset.Variable(
        dimensions=("y", "x"),
        data=np.ones(geo_shape),
        attributes=cfgrib.dataset.COORD_ATTRS["longitude"],
    )
    
    return geo_dims, geo_shape, geo_coord_vars

cfgrib.dataset.build_geography_coordinates = build_geography_coordinates

# cfgrib.
dss = cfgrib.open_datasets(
    "laf2020021715",
        backend_kwargs={"read_keys": ["typeOfLevel", "gridType"]},
        encode_cf=("time", "geography", "vertical"),
)

