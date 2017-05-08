Attributes {
    lon {
        String standard_name "longitude";
        String long_name "longitude";
        String units "degrees_east";
        String axis "X";
        String bounds "lon_bnds";
        String original_units "degrees_east";
    }
    lon_bnds {
    }
    lat {
        String standard_name "latitude";
        String long_name "latitude";
        String units "degrees_north";
        String axis "Y";
        String bounds "lat_bnds";
        String original_units "degrees_north";
    }
    lat_bnds {
    }
    time {
        String standard_name "time";
        String long_name "time";
        String units "days since 2001-1-1";
        String axis "T";
        String calendar "360_day";
        String bounds "time_bnds";
        String original_units "seconds since 2001-1-1";
    }
    time_bnds {
    }
    tos {
        String standard_name "sea_surface_temperature";
        String long_name "Sea Surface Temperature";
        String units "K";
        String cell_methods "time: mean (interval: 30 minutes)";
        Float32 _FillValue 1.00000002e+20;
        Float32 missing_value 1.00000002e+20;
        String original_name "sosstsst";
        String original_units "degC";
        String history " At   16:37:23 on 01/11/2005: CMOR altered the data in the following ways: added 2.73150E+02 to yield output units;  Cyclical dimension was output starting at a different lon;";
        tos {
        }
        time {
            String standard_name "time";
            String long_name "time";
            String units "days since 2001-1-1";
            String axis "T";
            String calendar "360_day";
            String bounds "time_bnds";
            String original_units "seconds since 2001-1-1";
        }
        lat {
            String standard_name "latitude";
            String long_name "latitude";
            String units "degrees_north";
            String axis "Y";
            String bounds "lat_bnds";
            String original_units "degrees_north";
        }
        lon {
            String standard_name "longitude";
            String long_name "longitude";
            String units "degrees_east";
            String axis "X";
            String bounds "lon_bnds";
            String original_units "degrees_east";
        }
    }
    NC_GLOBAL {
        String title "IPSL  model output prepared for IPCC Fourth Assessment SRES A2 experiment";
        String institution "IPSL (Institut Pierre Simon Laplace, Paris, France)";
        String source "IPSL-CM4_v1 (2003) : atmosphere : LMDZ (IPSL-CM4_IPCC, 96x71x19) ; ocean ORCA2 (ipsl_cm4_v1_8, 2x2L31); sea ice LIM (ipsl_cm4_v";
        String contact "Sebastien Denvil, sebastien.denvil@ipsl.jussieu.fr";
        String project_id "IPCC Fourth Assessment";
        String table_id "Table O1 (13 November 2004)";
        String experiment_id "SRES A2 experiment";
        Int32 realization 1;
        Float32 cmor_version 0.959999979;
        String Conventions "CF-1.0";
        String history "YYYY/MM/JJ: data generated; YYYY/MM/JJ+1 data transformed  At 16:37:23 on 01/11/2005, CMOR rewrote data to comply with CF standards and IPCC Fourth Assessment requirements";
        String references "Dufresne et al, Journal of Climate, 2015, vol XX, p 136";
        String comment "Test drive";
    }
    DODS_EXTRA {
        String Unlimited_Dimension "time";
    }
}
