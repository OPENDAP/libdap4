
# $Id$

# Datasets and their expected output (the information that writeval sends to
# stdout - not the stuff that should be going into the file).

# URI/BEARS:
set bears "http://$host/$test_dir/nph-dods/data/nc/bears.nc"
set bears_ce "bears"
set bears_dds "Dataset {
    Grid {
     ARRAY:
        String bears\[i = 2\]\[j = 3\]\[l = 3\];
     MAPS:
        Int32 i\[i = 2\];
        Float64 j\[j = 3\];
        Int32 l\[l = 3\];
    } bears;
    Grid {
     ARRAY:
        Int32 order\[i = 2\]\[j = 3\];
     MAPS:
        Int32 i\[i = 2\];
        Float64 j\[j = 3\];
    } order;
    Grid {
     ARRAY:
        Int32 shot\[i = 2\]\[j = 3\];
     MAPS:
        Int32 i\[i = 2\];
        Float64 j\[j = 3\];
    } shot;
    Grid {
     ARRAY:
        Float64 aloan\[i = 2\]\[j = 3\];
     MAPS:
        Int32 i\[i = 2\];
        Float64 j\[j = 3\];
    } aloan;
    Grid {
     ARRAY:
        Float64 cross\[i = 2\]\[j = 3\];
     MAPS:
        Int32 i\[i = 2\];
        Float64 j\[j = 3\];
    } cross;
    Int32 i\[i = 2\];
    Float64 j\[j = 3\];
    Int32 l\[l = 3\];
} bears;"

# URI/FNOC
set fnoc1 "http://$host/$test_dir/nph-dods/data/nc/fnoc1.nc"
set fnoc1_ce "u\\\[0:0\\\]\\\[0:9\\\]\\\[0:9\\\]"
set fnoc1_dds "Dataset {
    Int32 u\[time_a = 16\]\[lat = 17\]\[lon = 21\];
    Int32 v\[time_a = 16\]\[lat = 17\]\[lon = 21\];
    Float64 lat\[lat = 17\];
    Float64 lon\[lon = 21\];
    Float64 time\[time = 16\];
} fnoc1;"

# URI/DSP:
set dsp_1 "http://$host/$test_dir/nph-dods/data/dsp/east.coast.pvu"
set dsp_1_ce "dsp_band_1\\\[20:30\\\]\\\[20:30\\\]"
set dsp_1_dds "Dataset {
    Byte dsp_band_1\\\[line = 512\\\]\\\[pixel = 512\\\];
} east.coast.pvu;"

# URI/MatLab:
set nscat_s2 "http://$host/$test_dir/nph-dods/data/mat/NSCAT_S2.mat"
set nscat_s2_ce "NSCAT_S2\\\[75:75\\\]\\\[0:5\\\]"
set nscat_s2_dds "Dataset {
    Float64 NSCAT_S2\[NSCAT_S2_row = 153\]\[NSCAT_S2_column = 843\];
} NSCAT_S2;"

# URI/NSCAT:
set nscat_hdf "http://$host/$test_dir/nph-dods/data/hdf/S2000415.HDF"
set nscat_hdf_ce "WVC_Lat\\\[200:201\\\]\\\[20:21\\\]"
set nscat_hdf_dds "Dataset {
    Int32 WVC_Lat\[row = 458\]\[WVC = 24\];
    UInt32 WVC_Lon\[row = 458\]\[WVC = 24\];
    UInt32 Num_Sigma0\[row = 458\]\[WVC = 24\];
    UInt32 Num_Beam_12\[row = 458\]\[WVC = 24\];
    UInt32 Num_Beam_34\[row = 458\]\[WVC = 24\];
    UInt32 Num_Beam_56\[row = 458\]\[WVC = 24\];
    UInt32 Num_Beam_78\[row = 458\]\[WVC = 24\];
    UInt32 WVC_Quality_Flag\[row = 458\]\[WVC = 24\];
    UInt32 Mean_Wind\[row = 458\]\[WVC = 24\];
    UInt32 Wind_Speed\[row = 458\]\[WVC = 24\]\[position = 4\];
    UInt32 Wind_Dir\[row = 458\]\[WVC = 24\]\[position = 4\];
    UInt32 Error_Speed\[row = 458\]\[WVC = 24\]\[position = 4\];
    UInt32 Error_Dir\[row = 458\]\[WVC = 24\]\[position = 4\];
    Int32 MLE_Likelihood\[row = 458\]\[WVC = 24\]\[position = 4\];
    UInt32 Num_Ambigs\[row = 458\]\[WVC = 24\];
    Sequence {
        Structure {
            Int32 begin__0;
        } begin;
    } SwathIndex;
    Sequence {
        Structure {
            String Mean_Time__0;
        } Mean_Time;
        Structure {
            UInt32 Low_Wind_Speed_Flag__0;
        } Low_Wind_Speed_Flag;
        Structure {
            UInt32 High_Wind_Speed_Flag__0;
        } High_Wind_Speed_Flag;
    } NSCAT%20L2;
} S2000415%2eHDF;"

# URI/NSCAT Level 3 - here to test grids with the hdf server.
set nscat_l3 "http://$host/$test_dir/nph-dods/data/hdf/S3097057.HDF"
set nscat_l3_ce1 "WVC_Count\\\[200:201\\\]\\\[20:119\\\]"
set nscat_l3_ce2 "Avg_Wind_Vel_U\\\[200:201\\\]\\\[20:119\\\]"
set nscat_l3_dds "Dataset {
    Grid {
     ARRAY:
        UInt32 WVC_Count\[row = 300\]\[column = 720\];
     MAPS:
        Float64 row\[300\];
        Float64 column\[720\];
    } WVC_Count;
    Grid {
     ARRAY:
        UInt32 Map_Day_Fraction\[row = 300\]\[column = 720\];
     MAPS:
        Float64 row\[300\];
        Float64 column\[720\];
    } Map_Day_Fraction;
    Grid {
     ARRAY:
        UInt32 Avg_Sigma0_Count\[row = 300\]\[column = 720\];
     MAPS:
        Float64 row\[300\];
        Float64 column\[720\];
    } Avg_Sigma0_Count;
    Grid {
     ARRAY:
        Int32 Avg_Wind_Vel_U\[row = 300\]\[column = 720\];
     MAPS:
        Float64 row\[300\];
        Float64 column\[720\];
    } Avg_Wind_Vel_U;
    Grid {
     ARRAY:
        Int32 Avg_Wind_Vel_V\[row = 300\]\[column = 720\];
     MAPS:
        Float64 row\[300\];
        Float64 column\[720\];
    } Avg_Wind_Vel_V;
    Grid {
     ARRAY:
        UInt32 Avg_Wind_Speed\[row = 300\]\[column = 720\];
     MAPS:
        Float64 row\[300\];
        Float64 column\[720\];
    } Avg_Wind_Speed;
    Grid {
     ARRAY:
        UInt32 RMS_Wind_Speed\[row = 300\]\[column = 720\];
     MAPS:
        Float64 row\[300\];
        Float64 column\[720\];
    } RMS_Wind_Speed;
    Grid {
     ARRAY:
        UInt32 Map_Day_Fraction_StdDev\[row = 300\]\[column = 720\];
     MAPS:
        Float64 row\[300\];
        Float64 column\[720\];
    } Map_Day_Fraction_StdDev;
    Grid {
     ARRAY:
        UInt32 Wind_Vel_U_StdDev\[row = 300\]\[column = 720\];
     MAPS:
        Float64 row\[300\];
        Float64 column\[720\];
    } Wind_Vel_U_StdDev;
    Grid {
     ARRAY:
        UInt32 Wind_Vel_V_StdDev\[row = 300\]\[column = 720\];
     MAPS:
        Float64 row\[300\];
        Float64 column\[720\];
    } Wind_Vel_V_StdDev;
    Sequence {
        Structure {
            String bin_meth__0;
        } bin_meth;
        Structure {
            String grid_origin__0;
        } grid_origin;
        Structure {
            Float64 hsize__0;
        } hsize;
        Structure {
            Int32 max_east__0;
        } max_east;
        Structure {
            Int32 max_north__0;
        } max_north;
        Structure {
            Int32 max_south__0;
        } max_south;
        Structure {
            Int32 max_west__0;
        } max_west;
        Structure {
            String registration__0;
        } registration;
        Structure {
            Float64 vsize__0;
        } vsize;
    } Lat%2fLon;
} S3097057%2eHDF;"

# URI/JGOFS:
set jg_test "http://$host/$test_dir/nph-dods/test"
set jg_test_ce "year,month,lat,lon,sal&lat>37.5&lat<38.0"
set jg_test_dds "Dataset {
    Sequence {
        String leg;
        String year;
        String month;
        Sequence {
            String station;
            String lat;
            String lon;
            Sequence {
                String press;
                String temp;
                String sal;
                String o2;
                String sigth;
            } Level_2;
        } Level_1;
    } Level_0;
} test;"

# FF server tests. These also test asking for stuff when the variable names
# contain spaces. 7/26/2001 jhrg

set ff_test1 "http://$host/$test_dir/nph-dods/data/ff/jplpath.dat"
set ff_test1_ce1 "year"
set ff_test1_ce2 "JPL_Pathfinder.year"
set ff_test1_dds "Dataset {
    Sequence {
        Int32 year;
        Int32 day;
        Int32 hours;
        Int32 minutes;
        Int32 seconds;
        String DODS_URL;
    } JPL_Pathfinder;
} jplpath.dat;
"

set ff_test2 "http://$host/$test_dir/nph-dods/data/ff/jplpath_space.dat"
set ff_test2_ce1 "year"
set ff_test2_ce2 "JPL%20Pathfinder.year"
set ff_test2_ce3 "JPL Pathfinder.year"
set ff_test2_dds "Dataset {
    Sequence {
        Int32 year;
        Int32 day;
        Int32 hours;
        Int32 minutes;
        Int32 seconds;
        String DODS_URL;
    } JPL%20Pathfinder;
} jplpath_space.dat;
"