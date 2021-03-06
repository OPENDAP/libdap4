Attributes {
    lat {
        String long_name "latitude";
        String units "degrees_north";
        Float64 _FillValue 9.999999616903162e+35;
    }
    lon {
        String long_name "longitude";
        String units "degrees_east";
        Float64 _FillValue 9.999999616903162e+35;
    }
    lev {
        String long_name "hybrid level at midpoints (1000*(A+B))";
        String units "level";
        String positive "down";
        String standard_name "atmosphere_hybrid_sigma_pressure_coordinate";
        String formula_terms "a: hyam b: hybm p0: P0 ps: PS";
        Float64 _FillValue 9.999999616903162e+35;
    }
    ilev {
        String long_name "hybrid level at interfaces (1000*(A+B))";
        String units "level";
        String positive "down";
        String standard_name "atmosphere_hybrid_sigma_pressure_coordinate";
        String formula_terms "a: hyai b: hybi p0: P0 ps: PS";
        Float64 _FillValue 9.999999616903162e+35;
    }
    time {
        String long_name "time";
        String units "days since 0000-08-29 00:00:00";
        String calendar "noleap";
        Float64 _FillValue 9.999999616903162e+35;
    }
    hyai {
        String long_name "hybrid A coefficient at layer interfaces";
        Float64 _FillValue 9.999999616903162e+35;
    }
    hybi {
        String long_name "hybrid B coefficient at layer interfaces";
        Float64 _FillValue 9.999999616903162e+35;
    }
    hyam {
        String long_name "hybrid A coefficient at layer midpoints";
        Float64 _FillValue 9.999999616903162e+35;
    }
    hybm {
        String long_name "hybrid B coefficient at layer midpoints";
        Float64 _FillValue 9.999999616903162e+35;
    }
    gw {
        String long_name "gauss weights";
        Float64 _FillValue 9.999999616903162e+35;
    }
    P0 {
        String long_name "reference pressure";
        String units "Pa";
    }
    date_written {
        Int32 string_length 8;
    }
    time_written {
        Int32 string_length 8;
    }
    ntrm {
        String long_name "spectral truncation parameter M";
    }
    ntrn {
        String long_name "spectral truncation parameter N";
    }
    ntrk {
        String long_name "spectral truncation parameter K";
    }
    ndbase {
        String long_name "base day";
    }
    nsbase {
        String long_name "seconds of base day";
    }
    nbdate {
        String long_name "base date (YYYYMMDD)";
    }
    nbsec {
        String long_name "seconds of base date";
    }
    mdt {
        String long_name "timestep";
        String units "s";
    }
    ndcur {
        String long_name "current day (from base day)";
    }
    nscur {
        String long_name "current seconds of current day";
    }
    date {
        String long_name "current date (YYYYMMDD)";
    }
    datesec {
        String long_name "current seconds of current date";
    }
    nsteph {
        String long_name "current timestep";
    }
    U {
        String long_name "Zonal wind";
        String units "m/s";
    }
    V {
        String long_name "Meridional wind";
        String units "m/s";
    }
    T {
        String long_name "Temperature";
        String units "K";
    }
    Q {
        String long_name "Specific humidity";
        String units "kg/kg";
    }
    PS {
        String long_name "Surface pressure";
        String units "Pa";
    }
    PHIS {
        String long_name "surface geopotential";
        String units "M2/S2";
        Float64 _FillValue 1.000000000000000e+36;
        String from_hires "true";
    }
    SGH {
        String long_name "orography standard deviation";
        String units "M";
        Float64 _FillValue 1.000000000000000e+36;
        String from_hires "true";
    }
    LANDM {
        String long_name "land ocean transition mask: ocean (0), continent (1), transition (0-1)";
        String units "none";
        Float64 _FillValue 1.000000000000000e+36;
        String from_hires "true";
    }
    PBLH {
        String long_name "PBL height";
        String units "m";
    }
    TPERT {
        String long_name "Perturbation temperature (eddies in PBL)";
        String units "K";
    }
    QPERT {
        String long_name "Perturbation specific humidity (eddies in PBL)";
        String units "kg/kg";
    }
    CLOUD {
        String long_name "Cloud fraction";
        String units "fraction";
    }
    TS {
        String long_name "Surface temperature";
        String units "K";
    }
    TSICE {
        String long_name "Ice temperature";
        String units "K";
    }
    TS1 {
        String long_name "TS1      subsoil temperature";
        String units "K";
    }
    TS2 {
        String long_name "TS2      subsoil temperature";
        String units "K";
    }
    TS3 {
        String long_name "TS3      subsoil temperature";
        String units "K";
    }
    TS4 {
        String long_name "TS4      subsoil temperature";
        String units "K";
    }
    SNOWHICE {
        String long_name "Water equivalent snow depth";
        String units "m";
    }
    LANDFRAC {
        String long_name "gridbox land fraction";
        String units "FRAC";
        Float64 _FillValue 1.000000000000000e+36;
        String from_hires "true";
    }
    TBOT {
        String long_name "Lowest model level temperature";
        String units "K";
    }
    ICEFRAC {
        String long_name "Fraction of sfc area covered by sea-ice";
        String units "fraction";
    }
    SICTHK {
        String long_name "Sea ice thickness";
        String units "m";
    }
    CLDLIQ {
        String long_name "Grid box averaged liquid condensate amount";
        String units "kg/kg";
    }
    CLDICE {
        String long_name "Grid box averaged ice condensate amount";
        String units "kg/kg";
    }
    LANDM_COSLAT {
        String long_name "land ocean transition mask: ocean (0), continent (1), transition (0-1)";
        String units "none";
        Float64 _FillValue 1.000000000000000e+36;
        String from_hires "true";
    }
    NC_GLOBAL {
        String Conventions "CF-1.0";
        String logname "olson";
        String host "bb0001en";
        String source "Interpolated from:/fs/cgd/data0/olson/inputIC/newICeul.cam2.i.0000-09-01-00000.nc::CAM";
        String case "cam2run";
        String title "Interpolated from:/fs/cgd/data0/olson/inputIC/newICeul.cam2.i.0000-09-01-00000.nc::atm ver atm, eul ver v013, case newICeul";
        String history "\01205/07/03 12:15:34 olson:chinookfe:interpic -t SEP1.T42L26.gaussian.template.nc /fs/cgd/data0/olson/inputIC/newICeul.cam2.i.0000-09-01-00000.nc cami_0000-09-01_64x128_L26_c030507.nc\012definesurf -t /fs/cgd/csm/inputdata/atm/cam1/hrtopo/topo.nc cami_0000-09-01_64x128_L26_c030507.nc\012definesurf -t /fs/cgd/csm/inputdata/atm/cam2/hrtopo/topo.nc cami_0000-09-01_64x128_L26_c030507.nc\012definesurf -t /fs/cgd/csm/inputdata/atm/cam2/hrtopo/topo-usgs-10min.nc cami_0000-09-01_64x128_L26_c030507.nc\012definesurf -t /fs/cgd/csm/inputdata/atm/cam2/hrtopo/topo10min.merged_c030506.nc cami_0000-09-01_64x128_L26_c030507.nc\012definesurf -t /fs/cgd/csm/inputdata/atm/cam2/hrtopo/topo10min.merged_c030506.nc -l cami_0000-09-01_64x128_L26_c030624.nc.new";
        String make_ross "true";
    }
    DODS_EXTRA {
        String Unlimited_Dimension "time";
    }
}
