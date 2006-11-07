
# $Id$

# Datasets and their expected output (the information that writeval sends to
# stdout - not the stuff that should be going into the file).

# URI/BEARS:
set bears "http://$host/$test_dir/nph-dods/data/nc/bears.nc"
set bears_ce "bears"

# URI/FNOC
set fnoc1 "http://$host/$test_dir/nph-dods/data/nc/fnoc1.nc"
set fnoc1_ce "u\\\[0:0\\\]\\\[0:9\\\]\\\[0:9\\\]"

# URI/NSCAT Level 3 - here to test grids with the hdf server.
set nscat_l3 "http://$host/$test_dir/nph-dods/data/hdf/S3097057.HDF"
set nscat_l3_ce1 "WVC_Count\\\[200:201\\\]\\\[20:119\\\]"
set nscat_l3_ce2 "Avg_Wind_Vel_U\\\[200:201\\\]\\\[20:119\\\]"
