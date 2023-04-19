import cfgrib
import typing as T
import logging
import numpy as np
import timeit

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

start = timeit.default_timer()
# cfgrib.
dss = cfgrib.open_datasets(
    "laf2020021715",
        backend_kwargs={"read_keys": ["typeOfLevel", "gridType"]},
        encode_cf=("time", "geography", "vertical"),
)
for ds in dss:
    ds.load()
stop = timeit.default_timer()

total = 0
for ds in dss:
    for f in ds:
        total += np.prod(ds[f].data.shape)*ds[f].data.itemsize
print('Time: ', stop - start) 
print('Size: ', total/(1024.*1024.*1024.))
print('Bandwidth: ', total/(1024.*1024.*1024.) / (stop-start), " GB/s")

