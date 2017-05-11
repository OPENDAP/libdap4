Attributes {
    lat {
        String long_name "latitude";
        String units "degrees_north";
        String axis "Y";
        String standard_name "latitude";
        String bounds "lat_bnds";
    }
    lon {
        String long_name "longitude";
        String units "degrees_east";
        String axis "X";
        String standard_name "longitude";
        String bounds "lon_bnds";
    }
    area {
        String long_name "Surface area";
        String units "meter2";
        area {
        }
        lat {
            String long_name "latitude";
            String units "degrees_north";
            String axis "Y";
            String standard_name "latitude";
            String bounds "lat_bnds";
        }
        lon {
            String long_name "longitude";
            String units "degrees_east";
            String axis "X";
            String standard_name "longitude";
            String bounds "lon_bnds";
        }
    }
    lat_bnds {
    }
    lon_bnds {
    }
    msk_rgn {
        String long_name "Mask region";
        String units "bool";
        msk_rgn {
        }
        lat {
            String long_name "latitude";
            String units "degrees_north";
            String axis "Y";
            String standard_name "latitude";
            String bounds "lat_bnds";
        }
        lon {
            String long_name "longitude";
            String units "degrees_east";
            String axis "X";
            String standard_name "longitude";
            String bounds "lon_bnds";
        }
    }
    plev {
        String long_name "pressure";
        String units "Pa";
        String standard_name "air_pressure";
        String positive "down";
        String axis "Z";
    }
    time {
        String calendar "noleap";
        String standard_name "time";
        String axis "T";
        String units "days since 0000-1-1";
        String bounds "time_bnds";
        String long_name "time";
    }
    pr {
        String comment "Created using NCL code CCSM_atmm_2cf.ncl on\012 machine eagle163s";
        Float32 missing_value 1.00000002e+20;
        Float32 _FillValue 1.00000002e+20;
        String cell_methods "time: mean (interval: 1 month)";
        String history "(PRECC+PRECL)*r[h2o]";
        String original_units "m-1 s-1";
        String original_name "PRECC, PRECL";
        String standard_name "precipitation_flux";
        String units "kg m-2 s-1";
        String long_name "precipitation_flux";
        String cell_method "time: mean";
        pr {
        }
        time {
            String calendar "noleap";
            String standard_name "time";
            String axis "T";
            String units "days since 0000-1-1";
            String bounds "time_bnds";
            String long_name "time";
        }
        lat {
            String long_name "latitude";
            String units "degrees_north";
            String axis "Y";
            String standard_name "latitude";
            String bounds "lat_bnds";
        }
        lon {
            String long_name "longitude";
            String units "degrees_east";
            String axis "X";
            String standard_name "longitude";
            String bounds "lon_bnds";
        }
    }
    tas {
        String comment "Created using NCL code CCSM_atmm_2cf.ncl on\012 machine eagle163s";
        Float32 missing_value 1.00000002e+20;
        Float32 _FillValue 1.00000002e+20;
        String cell_methods "time: mean (interval: 1 month)";
        String history "Added height coordinate";
        String coordinates "height";
        String original_units "K";
        String original_name "TREFHT";
        String standard_name "air_temperature";
        String units "K";
        String long_name "air_temperature";
        String cell_method "time: mean";
        tas {
        }
        time {
            String calendar "noleap";
            String standard_name "time";
            String axis "T";
            String units "days since 0000-1-1";
            String bounds "time_bnds";
            String long_name "time";
        }
        lat {
            String long_name "latitude";
            String units "degrees_north";
            String axis "Y";
            String standard_name "latitude";
            String bounds "lat_bnds";
        }
        lon {
            String long_name "longitude";
            String units "degrees_east";
            String axis "X";
            String standard_name "longitude";
            String bounds "lon_bnds";
        }
    }
    time_bnds {
    }
    ua {
        String comment "Created using NCL code CCSM_atmm_2cf.ncl on\012 machine eagle163s";
        Float32 missing_value 1.00000002e+20;
        String cell_methods "time: mean (interval: 1 month)";
        String long_name "eastward_wind";
        String history "Interpolated U with NCL 'vinth2p_ecmwf'";
        String units "m s-1";
        String original_units "m s-1";
        String original_name "U";
        String standard_name "eastward_wind";
        Float32 _FillValue 1.00000002e+20;
        ua {
        }
        time {
            String calendar "noleap";
            String standard_name "time";
            String axis "T";
            String units "days since 0000-1-1";
            String bounds "time_bnds";
            String long_name "time";
        }
        plev {
            String long_name "pressure";
            String units "Pa";
            String standard_name "air_pressure";
            String positive "down";
            String axis "Z";
        }
        lat {
            String long_name "latitude";
            String units "degrees_north";
            String axis "Y";
            String standard_name "latitude";
            String bounds "lat_bnds";
        }
        lon {
            String long_name "longitude";
            String units "degrees_east";
            String axis "X";
            String standard_name "longitude";
            String bounds "lon_bnds";
        }
    }
    NC_GLOBAL {
        String CVS_Id "$Id$";
        String creation_date "";
        String prg_ID "Source file unknown Version unknown Date unknown";
        String cmd_ln "bds -x 256 -y 128 -m 23 -o /data/zender/data/dst_T85.nc";
        String history "Tue Oct 25 15:08:51 2005: ncks -O -x -v va -m sresa1b_ncar_ccsm3_0_run1_200001.nc sresa1b_ncar_ccsm3_0_run1_200001.nc\012Tue Oct 25 15:07:21 2005: ncks -d time,0 sresa1b_ncar_ccsm3_0_run1_200001_201912.nc sresa1b_ncar_ccsm3_0_run1_200001.nc\012Tue Oct 25 13:29:43 2005: ncks -d time,0,239 sresa1b_ncar_ccsm3_0_run1_200001_209912.nc /var/www/html/tmp/sresa1b_ncar_ccsm3_0_run1_200001_201912.nc\012Thu Oct 20 10:47:50 2005: ncks -A -v va /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/sresa1b_ncar_ccsm3_0_run1_va_200001_209912.nc /data/brownmc/sresa1b/atm/mo/tas/ncar_ccsm3_0/run1/sresa1b_ncar_ccsm3_0_run1_200001_209912.nc\012Wed Oct 19 14:55:04 2005: ncks -F -d time,01,1200 /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/sresa1b_ncar_ccsm3_0_run1_va_200001_209912.nc /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/sresa1b_ncar_ccsm3_0_run1_va_200001_209912.nc\012Wed Oct 19 14:53:28 2005: ncrcat /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/foo_05_1200.nc /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/foo_1192_1196.nc /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/sresa1b_ncar_ccsm3_0_run1_va_200001_209912.nc\012Wed Oct 19 14:50:38 2005: ncks -F -d time,05,1200 /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/va_A1.SRESA1B_1.CCSM.atmm.2000-01_cat_2099-12.nc /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/foo_05_1200.nc\012Wed Oct 19 14:49:45 2005: ncrcat /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/va_A1.SRESA1B_1.CCSM.atmm.2000-01_cat_2079-12.nc /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/va_A1.SRESA1B_1.CCSM.atmm.2080-01_cat_2099-12.nc /data/brownmc/sresa1b/atm/mo/va/ncar_ccsm3_0/run1/va_A1.SRESA1B_1.CCSM.atmm.2000-01_cat_2099-12.nc\012Created from CCSM3 case b30.040a\012 by wgstrand@ucar.edu\012 on Wed Nov 17 14:12:57 EST 2004\012 \012 For all data, added IPCC requested metadata";
        String table_id "Table A1";
        String title "model output prepared for IPCC AR4";
        String institution "NCAR (National Center for Atmospheric \012Research, Boulder, CO, USA)";
        String source "CCSM3.0, version beta19 (2004): \012atmosphere: CAM3.0, T85L26;\012ocean     : POP1.4.3 (modified), gx1v3\012sea ice   : CSIM5.0, T85;\012land      : CLM3.0, gx1v3";
        String contact "ccsm@ucar.edu";
        String project_id "IPCC Fourth Assessment";
        String Conventions "CF-1.0";
        String references "Collins, W.D., et al., 2005:\012 The Community Climate System Model, Version 3\012 Journal of Climate\012 \012 Main website: http://www.ccsm.ucar.edu";
        String acknowledgment " Any use of CCSM data should acknowledge the contribution\012 of the CCSM project and CCSM sponsor agencies with the \012 following citation:\012 'This research uses data provided by the Community Climate\012 System Model project (www.ccsm.ucar.edu), supported by the\012 Directorate for Geosciences of the National Science Foundation\012 and the Office of Biological and Environmental Research of\012 the U.S. Department of Energy.'\012In addition, the words 'Community Climate System Model' and\012 'CCSM' should be included as metadata for webpages referencing\012 work using CCSM data or as keywords provided to journal or book\012publishers of your manuscripts.\012Users of CCSM data accept the responsibility of emailing\012 citations of publications of research using CCSM data to\012 ccsm@ucar.edu.\012Any redistribution of CCSM data must include this data\012 acknowledgement statement.";
        Int32 realization 1;
        String experiment_id "720 ppm stabilization experiment (SRESA1B)";
        String comment "This simulation was initiated from year 2000 of \012 CCSM3 model run b30.030a and executed on \012 hardware cheetah.ccs.ornl.gov. The input external forcings are\012ozone forcing    : A1B.ozone.128x64_L18_1991-2100_c040528.nc\012aerosol optics   : AerosolOptics_c040105.nc\012aerosol MMR      : AerosolMass_V_128x256_clim_c031022.nc\012carbon scaling   : carbonscaling_A1B_1990-2100_c040609.nc\012solar forcing    : Fixed at 1366.5 W m-2\012GHGs             : ghg_ipcc_A1B_1870-2100_c040521.nc\012GHG loss rates   : noaamisc.r8.nc\012volcanic forcing : none\012DMS emissions    : DMS_emissions_128x256_clim_c040122.nc\012oxidants         : oxid_128x256_L26_clim_c040112.nc\012SOx emissions    : SOx_emissions_A1B_128x256_L2_1990-2100_c040608.nc\012 Physical constants used for derived data:\012 Lv (latent heat of evaporation): 2.501e6 J kg-1\012 Lf (latent heat of fusion     ): 3.337e5 J kg-1\012 r[h2o] (density of water      ): 1000 kg m-3\012 g2kg   (grams to kilograms    ): 1000 g kg-1\012 \012 Integrations were performed by NCAR and CRIEPI with support\012 and facilities provided by NSF, DOE, MEXT and ESC/JAMSTEC.";
        String model_name_english "NCAR CCSM";
    }
    DODS_EXTRA {
        String Unlimited_Dimension "time";
    }
}
