Attributes {
    lat {
        String units "degrees_north";
        String long_name "Latitude";
        Float32 actual_range 88.0000000, -88.0000000;
        String standard_name "latitude_north";
        String axis "y";
        String coordinate_defines "center";
    }
    lon {
        String units "degrees_east";
        String long_name "Longitude";
        Float32 actual_range 0.00000000, 358.000000;
        String standard_name "longitude_east";
        String axis "x";
        String coordinate_defines "center";
    }
    time {
        String units "days since 1800-1-1 00:00:00";
        String long_name "Time";
        Float64 actual_range 19723.00000000000, 76214.00000000000;
        String delta_t "0000-01-00 00:00:00";
        String avg_period "0000-01-00 00:00:00";
        String prev_avg_period "0000-00-07 00:00:00";
        String standard_name "time";
        String axis "t";
    }
    time_bnds {
        String long_name "Time Boundaries";
    }
    sst {
        String long_name "Monthly Means of Sea Surface Temperature";
        Float32 valid_range -5.00000000, 40.0000000;
        Float32 actual_range -1.79999995, 34.2399979;
        String units "degC";
        Float32 add_offset 0.00000000;
        Float32 scale_factor 0.00999999978;
        Int16 missing_value 32767;
        Int16 precision 2;
        Int16 least_significant_digit 1;
        String var_desc "Sea Surface Temperature";
        String dataset "NOAA Extended Reconstructed SST V3";
        String level_desc "Surface";
        String statistic "Mean";
        String parent_stat "Mean";
        sst {
        }
        time {
            String units "days since 1800-1-1 00:00:00";
            String long_name "Time";
            Float64 actual_range 19723.00000000000, 76214.00000000000;
            String delta_t "0000-01-00 00:00:00";
            String avg_period "0000-01-00 00:00:00";
            String prev_avg_period "0000-00-07 00:00:00";
            String standard_name "time";
            String axis "t";
        }
        lat {
            String units "degrees_north";
            String long_name "Latitude";
            Float32 actual_range 88.0000000, -88.0000000;
            String standard_name "latitude_north";
            String axis "y";
            String coordinate_defines "center";
        }
        lon {
            String units "degrees_east";
            String long_name "Longitude";
            Float32 actual_range 0.00000000, 358.000000;
            String standard_name "longitude_east";
            String axis "x";
            String coordinate_defines "center";
        }
    }
    NC_GLOBAL {
        String title "NOAA Extended Reconstructed SST V3";
        String conventions "CF-1.0";
        String history "created 09/2007 by CAS";
        String comments "The extended reconstructed sea surface temperature (ERSST)\012was constructed using the most recently available \012Comprehensive Ocean-Atmosphere Data Set (COADS) SST data \012and improved statistical methods that allow stable \012reconstruction using sparse data.\012Currently, ERSST version 2 (ERSST.v2) and version 3 (ERSST.v3) are available from NCDC.\012 ERSST.v3 is an improved extended reconstruction over version 2.\012 Most of the improvements are justified by testing with simulated data.\012 The major differences are caused by the improved low-frequency (LF) tuning of ERSST.v3 \012which reduces the SST anomaly damping before 1930 using the optimized parameters.\012 Beginning in 1985, ERSST.v3 is also improved by explicitly including\012 bias-adjusted satellite infrared data from AVHRR.";
        String platform "Model";
        String source "NOAA/NESDIS/National Climatic Data Center";
        String institution "NOAA/NESDIS/National Climatic Data Center";
        String references "http://www.ncdc.noaa.gov/oa/climate/research/sst/ersstv3.php\012http://www.cdc.noaa.gov/cdc/data.noaa.ersst.html";
        String citation "Smith, T.M., R.W. Reynolds, Thomas C. Peterson, and Jay Lawrimore 2007: Improvements to NOAA's Historical Merged Land-Ocean Surface Temperature Analysis (1880-2006). In press. Journal of Climate (ERSSTV3).\012Smith, T.M., and R.W. Reynolds, 2003: Extended Reconstruction of Global Sea Surface Temperatures Based on COADS Data (1854-1997). Journal of Climate, 16, 1495-1510. ERSSTV1\012 Smith, T.M., and R.W. Reynolds, 2004: Improved Extended Reconstruction of SST (1854-1997). Journal of Climate, 17, 2466-2477.";
    }
    DODS_EXTRA {
        String Unlimited_Dimension "time";
    }
}
