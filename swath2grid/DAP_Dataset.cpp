/******************************************************************************
 * $Id: TRMM_Dataset.cpp 2011-07-19 16:24:00Z $
 *
 * Project:  The Open Geospatial Consortium (OGC) Web Coverage Service (WCS)
 * 			 for Earth Observation: Open Source Reference Implementation
 * Purpose:  DAP_Dataset implementation for NOAA GOES data
 * Author:   Yuanzheng Shao, yshao3@gmu.edu
 *
 ******************************************************************************
 * Copyright (c) 2011, Liping Di <ldi@gmu.edu>, Yuanzheng Shao <yshao3@gmu.edu>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include <cstdlib>

#include "DAP_Dataset.h"

#include "Array.h"

#include "ce_functions.h"

using namespace libdap;
using namespace std;

#define GOES_TIME_DEBUG FALSE

namespace libdap {

DAP_Dataset::DAP_Dataset()
{
}

/************************************************************************/
/*                           ~DAP_Dataset()                         */
/************************************************************************/

/**
 * \brief Destroy an open DAP_Dataset object.
 *
 * This is the accepted method of closing a DAP_Dataset dataset and
 * deallocating all resources associated with it.
 */

DAP_Dataset::~DAP_Dataset()
{
}

/************************************************************************/
/*                           DAP_Dataset()                          */
/************************************************************************/

/**
 * \brief Create an DAP_Dataset object.
 *
 * This is the accepted method of creating a DAP_Dataset object and
 * allocating all resources associated with it.
 *
 * @param id The coverage identifier.
 *
 * @param rBandList The field list selected for this coverage. For TRMM
 * daily data, the user could specify multiple days range in request.
 * Each day is seemed as one field.
 *
 * @return A DAP_Dataset object.
 */

DAP_Dataset::DAP_Dataset(const string& id, vector<int> &rBandList) :
        AbstractDataset(id, rBandList)
{
    md_MissingValue = 0;
    mb_GeoTransformSet = FALSE;
}

/**
 * @brief Initialize a DAP Dataset using Array objects already read.
 *
 *
 */

DAP_Dataset::DAP_Dataset(Array *src, Array *lat, Array *lon) :
        AbstractDataset(), m_src(src), m_lat(lat), m_lon(lon)
{
    // Read this from the 'missing_value' or '_FillValue' attributes
    string missing_value = m_src->get_attr_table().get_attr("missing_value");
    if (missing_value.empty())
        missing_value = m_src->get_attr_table().get_attr("_FillValue");

    if (!missing_value.empty())
        md_MissingValue = atof(missing_value.c_str());
    else
        md_MissingValue = 0;

    mb_GeoTransformSet = FALSE;
}

/************************************************************************/
/*                           InitialDataset()                           */
/************************************************************************/

/**
 * \brief Initialize the GOES dataset with NetCDF format.

 * This method is the implementation for initializing a GOES dataset with NetCDF format.
 * Within this method, SetNativeCRS(), SetGeoTransform() and SetGDALDataset()
 * will be called to initialize an GOES dataset.
 *
 * @note To use this, call this method and then access the GDALDataset that
 * contains the reprojected array using maptr_DS.get().
 *
 * @param isSimple the WCS request type.  When user executing a DescribeCoverage
 * request, isSimple is set to 1, and for GetCoverage, is set to 0.
 *
 * @return CE_None on success or CE_Failure on failure.
 */

CPLErr DAP_Dataset::InitialDataset(const int isSimple)
{
#if 0
    vector<string> strSet;
    unsigned int n = CsvburstCpp(ms_CoverageID, strSet, ':');
    if (n != 4)
    {
        SetWCS_ErrorLocator("DAP_Dataset::InitialDataset()");
        WCS_Error(CE_Failure, OGC_WCS_NoApplicableCode, "Incorrect coverage ID.");
        return CE_Failure;
    }

    ms_DataTypeName = strSet[0] + ":" + strSet[1];
    ms_SrcFilename = StrTrims(strSet[2], " \'\"");
    ms_DatasetName = strSet[3];

    m_ncLatDataSetName = "NETCDF:" + ms_SrcFilename + ":latitude";
    m_ncLonDataSetName = "NETCDF:" + ms_SrcFilename + ":longitude";
    m_ncCoverageIDName = strSet[1] + ":" + strSet[2] + ":" + strSet[3];

    GDALDataset* pSrc = (GDALDataset*) GDALOpenShared(m_ncCoverageIDName.c_str(), GA_ReadOnly);
    if (pSrc == NULL)
    {
        SetWCS_ErrorLocator("DAP_Dataset::InitialDataset()");
        WCS_Error(CE_Failure, OGC_WCS_NoApplicableCode, "Failed to open file \"%s\".", ms_SrcFilename.c_str());
        return CE_Failure;
    }

    ms_NativeFormat = GDALGetDriverShortName(pSrc->GetDriver());

    //setmetalist
    SetMetaDataList(pSrc);// This doesn't do anything for DAP

    // We always have one band - could maybe expand this to more than one
    // later on so that several Grids would be returned.

    //set noValue
    unsigned int nBandCount = pSrc->GetRasterCount();
    if (nBandCount < 1)
    {
        GDALClose(pSrc);
        SetWCS_ErrorLocator("DAP_Dataset::InitialDataset()");
        WCS_Error(CE_Failure, OGC_WCS_NoApplicableCode, "The GOES file does not contain any raster band.");
        return CE_Failure;
    }

    maptr_DS.reset(pSrc);
#endif

    // FIXME Start rewriting code here - see notes.

    // NB: maptr_DS is not valid since it's (re)set above in code not
    // used anymore. SeGDALDataset() frees the original dataset
    // and makes a new virtual dataset. It then calls RectifyGOESDataSet
    // which does the remapping and resets maptr_DS once again.

    // Might break that operation out so the remap is a separate call
    if (CE_None != SetNativeCRS() || CE_None != SetGeoTransform())
        throw Error("Could not set the dataset native CRS or the GeoTransform.");

    if (CE_None != SetGDALDataset(isSimple)) {
        GDALClose(maptr_DS.release());
        return CE_Failure;
    }

    return CE_None;
}

/************************************************************************/
/*                            SetNativeCRS()                            */
/************************************************************************/

/**
 * \brief Set the Native CRS for a GOES dataset.
 *
 * The method will set the CRS for a GOES dataset as an native CRS.
 *
 * Since the original GOES data adopt satellite CRS to recored its value,
 * like MODIS swath data, each data point has its corresponding latitude
 * and longitude value, those coordinates could be fetched in another two fields.
 *
 * The native CRS for GOES Imager and Sounder data is assigned to EPSG:4326 if
 * both the latitude and longitude are existed.
 *
 * @return CE_None on success or CE_Failure on failure.
 */

CPLErr DAP_Dataset::SetNativeCRS()
{
#if 0
    // This has no effect since mo_NativeCRS is set to WGS84 below
    if (CE_None == AbstractDataset::SetNativeCRS())
    return CE_None;
#endif

#if 0
    // These values have already been read by the constraint evaluator
    GDALDataset* hLatDS = (GDALDataset*) GDALOpen(m_ncLatDataSetName.c_str(), GA_ReadOnly);
    GDALDataset* hLonDS = (GDALDataset*) GDALOpen(m_ncLonDataSetName.c_str(), GA_ReadOnly);

    if(hLatDS == NULL) {
        m_ncLatDataSetName = "NETCDF:\"" + ms_SrcFilename + "\":lat";
        hLatDS = (GDALDataset*) GDALOpen(m_ncLatDataSetName.c_str(), GA_ReadOnly);
    }

    if(hLonDS == NULL) {
        m_ncLonDataSetName = "NETCDF:\"" + ms_SrcFilename + "\":lon";
        hLonDS = (GDALDataset*) GDALOpen(m_ncLonDataSetName.c_str(), GA_ReadOnly);
    }

    if (hLatDS == NULL || hLonDS == NULL)
    {
        SetWCS_ErrorLocator("DAP_Dataset::SetNativeCRS()");
        WCS_Error(CE_Failure, OGC_WCS_NoApplicableCode, "Failed to open latitude/longitude sub-dataset.");
        return CE_Failure;
    }
#endif

    mo_NativeCRS.SetWellKnownGeogCS("WGS84");

#if 0
    GDALClose(hLatDS);
    GDALClose(hLonDS);
#endif

    //return CE_None;
}

/************************************************************************/
/*                           SetGeoTransform()                          */
/************************************************************************/

/**
 * \brief Set the affine GeoTransform matrix for a GOES data.
 *
 * The method will set a GeoTransform matrix for a GOES data
 * by parsing the coordinates values existed in longitude and latitude field.
 *
 * The CRS for the bounding box is EPSG:4326.
 *
 * @return CE_None on success or CE_Failure on failure.
 */

CPLErr DAP_Dataset::SetGeoTransform()
{
#if 0
    // This call has no effect since the md_GeoTransform array is
    // set below.
    if (CE_None == AbstractDataset::SetGeoTransform())
    return CE_None;
#endif
#if 0
    if (CE_None != SetGeoBBoxAndGCPs(maptr_DS.get()))
    return CE_Failure;
#endif

    // Assume the array is two dimensional
    Array::Dim_iter i = m_src->dim_begin();
    int nXSize = m_src->dimension_size(i, true);
    int nYSize = m_src->dimension_size(i + 1, true);

    // TODO The nXSize and nYSize can be passed into SetGeoBBoxAndGCPs
    SetGeoBBoxAndGCPs();

    double resX, resY;
    if (mdSrcGeoMinX > mdSrcGeoMaxX && mdSrcGeoMinX > 0 && mdSrcGeoMaxX < 0)
        resX = (360 + mdSrcGeoMaxX - mdSrcGeoMinX) / (nXSize - 1);
    else
        resX = (mdSrcGeoMaxX - mdSrcGeoMinX) / (nXSize - 1);

    resY = (mdSrcGeoMaxY - mdSrcGeoMinY) / (nYSize - 1);

    double res = MIN(resX, resY);

    if (mdSrcGeoMinX > mdSrcGeoMaxX && mdSrcGeoMinX > 0 && mdSrcGeoMaxX < 0)
        mi_RectifiedImageXSize = (int) ((360 + mdSrcGeoMaxX - mdSrcGeoMinX) / res) + 1;
    else
        mi_RectifiedImageXSize = (int) ((mdSrcGeoMaxX - mdSrcGeoMinX) / res) + 1;

    mi_RectifiedImageYSize = (int) fabs((mdSrcGeoMaxY - mdSrcGeoMinY) / res) + 1;

    md_Geotransform[0] = mdSrcGeoMinX;
    md_Geotransform[1] = res;
    md_Geotransform[2] = 0;
    md_Geotransform[3] = mdSrcGeoMaxY;
    md_Geotransform[4] = 0;
    md_Geotransform[5] = -res;
    mb_GeoTransformSet = TRUE;

    return CE_None;
}

#if 0
CPLErr DAP_Dataset::setResampleStandard(GDALDataset* hSrcDS, int& xRSValue, int& yRSValue)
{
    // FIXME
    // Since DAP_Dataset can do all of this w/o GDAL, replace this with
    // two functions. Not sure about the RESAMPLE_STANDARD
    static int RESAMPLE_STANDARD = 500;
    int nXSize = hSrcDS->GetRasterXSize();
    int nYSize = hSrcDS->GetRasterYSize();

    xRSValue = int(nXSize / RESAMPLE_STANDARD) + 2;
    yRSValue = int(nYSize / RESAMPLE_STANDARD) + 2;

    return CE_None;
}
#endif
#if 0
void DAP_Dataset::setResampleStandard(Array* hSrcDS, int& xRSValue, int& yRSValue)
{
    // FIXME
    // Since DAP_Dataset can do all of this w/o GDAL, replace this with
    // two functions. Not sure about the RESAMPLE_STANDARD
    static int RESAMPLE_STANDARD = 500;

    Array::Dim_iter i = m_src->dim_begin();
    int nXSize = m_src->dimension_size(i, true);
    int nYSize = m_src->dimension_size(i + 1, true);

#if 0
    int nXSize = hSrcDS->GetRasterXSize();
    int nYSize = hSrcDS->GetRasterYSize();
#endif
    xRSValue = int(nXSize / RESAMPLE_STANDARD) + 2;
    yRSValue = int(nYSize / RESAMPLE_STANDARD) + 2;
#if 0
    return CE_None;
#endif
}
#endif

/************************************************************************/
/*                         SetGeoBBoxAndGCPs()                          */
/************************************************************************/

/**
 * \brief Set the native geographical bounding box and GCP array for a GOES data.
 *
 * The method will set the native geographical bounding box
 * by comparing the coordinates values existed in longitude and latitude field.
 *
 * @param poVDS The GDAL dataset returned by calling GDALOpen() method.
 *
 * @return CE_None on success or CE_Failure on failure.
 */

void DAP_Dataset::SetGeoBBoxAndGCPs()
{
    // FIXME
#if 0
    // 'VDS' is a misnomer since it's not the virtual dataset yet. This code is just
    // working with the raster band's GDAL dataset object
    GDALDataset* hLatDS = (GDALDataset*) GDALOpen(m_ncLatDataSetName.c_str(), GA_ReadOnly);
    GDALDataset* hLonDS = (GDALDataset*) GDALOpen(m_ncLonDataSetName.c_str(), GA_ReadOnly);

    int nXSize = poVDS->GetRasterXSize();
    int nYSize = poVDS->GetRasterYSize();
#endif

    // Assume the array is two dimensional
    Array::Dim_iter i = m_src->dim_begin();
    int nXSize = m_src->dimension_size(i, true);
    int nYSize = m_src->dimension_size(i + 1, true);

    mi_GoesSrcImageXSize = nXSize;
    mi_GoesSrcImageYSize = nYSize;

    // reuse the Dim_iter for both lat and lon arrays
    i = m_lat->dim_begin();
    int nLatXSize = m_lat->dimension_size(i, true);
    int nLatYSize = m_lat->dimension_size(i + 1, true);
    i = m_lon->dim_begin();
    int nLonXSize = m_lon->dimension_size(i, true);
    int nLonYSize = m_lon->dimension_size(i + 1, true);

    if (nXSize != nLatXSize || nLatXSize != nLonXSize || nYSize != nLatYSize || nLatYSize != nLonYSize)
        throw Error("The size of latitude/longitude and data field does not match.");
#if 0
    {
        GDALClose(hLatDS);
        GDALClose(hLonDS);

        SetWCS_ErrorLocator("DAP_Dataset::SetGeoBBoxAndGCPs()");
        WCS_Error(CE_Failure, OGC_WCS_NoApplicableCode, );

        return CE_Failure;
    }
#endif
#if 0
    /*
     *	Re-sample Standards:
     *	Height | Width
     *	(0, 500)		every other one pixel
     *	[500, 1000)		every other two pixels
     *	[1000,1500)		every other three pixels
     *	[1500,2000)		every other four pixels
     *	... ...
     */

    int xSpace = 1;
    int ySpace = 1;
    //setResampleStandard(poVDS, xSpace, ySpace);

    // TODO understand how GMU picked this value.
    // xSpace and ySpace are the stride values for sampling in
    // the x and y dimensions.
    const int RESAMPLE_STANDARD = 500;

    xSpace = int(nXSize / RESAMPLE_STANDARD) + 2;
    ySpace = int(nYSize / RESAMPLE_STANDARD) + 2;
#endif
    int nGCPs = 0;
    GDAL_GCP gdalCGP;

#if 0
    GDALRasterBand *poBandLat = hLatDS->GetRasterBand(1);
    GDALRasterBand *poBandLon = hLonDS->GetRasterBand(1);
    GDALDataType eDT = poBandLat->GetRasterDataType();
    void *dataLat = NULL;
    void *dataLon = NULL;
#endif

    double *dataLat = extract_double_array(m_lat);
    double *dataLon = extract_double_array(m_lon);

    try {

        mdSrcGeoMinX = 360;
        mdSrcGeoMaxX = -360;
        mdSrcGeoMinY = 90;
        mdSrcGeoMaxY = -90;

        // Sample every fourth row and column
        int xSpace = 4;
        int ySpace = 4;

#if 0
        switch (eDT)
        {
            case GDT_Float32: //For GOES Imager and Sounder data
            {
                dataLat = (float *) CPLMalloc(nXSize * sizeof(float));
                dataLon = (float *) CPLMalloc(nXSize * sizeof(float));
#endif
        for (int iLine = 0; iLine < nYSize - ySpace; iLine += ySpace) {
#if 0
            // This catches the case where the stride steps over the
            // end of the array.
            // TODO Could write a better loop ...
            if (iLine >= nYSize)
            iLine = nYSize - 1;
#endif
#if 0
            poBandLat->RasterIO(GF_Read, 0, iLine, nXSize, 1, dataLat, nXSize, 1, GDT_Float32, 0, 0);
            poBandLon->RasterIO(GF_Read, 0, iLine, nXSize, 1, dataLon, nXSize, 1, GDT_Float32, 0, 0);
#endif
            for (int iPixel = 0; iPixel < nXSize - xSpace; iPixel += xSpace) {
#if 0
                // TODO same as above
                if (iPixel >= nXSize)
                iPixel = nXSize - 1;
#endif
#if 0
                double x = *((float *) dataLon + iPixel);
                double y = *((float *) dataLat + iPixel);
#endif
                double x = *(dataLon + (iLine * nYSize) + iPixel);
                double y = *(dataLat + (iLine * nYSize) + iPixel);

                if (isValidLongitude(x) && isValidLatitude(y)) {
                    char pChr[64];
                    snprintf(pChr, 64, "%d", ++nGCPs);
                    GDALInitGCPs(1, &gdalCGP);
                    gdalCGP.pszId = strdup(pChr);
                    gdalCGP.pszInfo = strdup("");
                    gdalCGP.dfGCPLine = iLine;
                    gdalCGP.dfGCPPixel = iPixel;
                    gdalCGP.dfGCPX = x;
                    gdalCGP.dfGCPY = y;
                    gdalCGP.dfGCPZ = 0;
                    m_gdalGCPs.push_back(gdalCGP);

                    mdSrcGeoMinX = MIN(mdSrcGeoMinX, gdalCGP.dfGCPX);
                    mdSrcGeoMaxX = MAX(mdSrcGeoMaxX, gdalCGP.dfGCPX);
                    mdSrcGeoMinY = MIN(mdSrcGeoMinY, gdalCGP.dfGCPY);
                    mdSrcGeoMaxY = MAX(mdSrcGeoMaxY, gdalCGP.dfGCPY);
                }
            }
        }
    }
    catch (...) {
        delete[] dataLat;
        delete[] dataLon;
    }

    delete[] dataLat;
    delete[] dataLon;

#if 0
    VSIFree((float *) dataLat);
    VSIFree((float *) dataLon);
}

break;

default:
break;
}

GDALClose(hLatDS);
GDALClose(hLonDS);

return CE_None;
#endif
}

/************************************************************************/
/*                           SetGDALDataset()                           */
/************************************************************************/

/**
 * \brief Set the GDALDataset object to GOES Imager and Sounder dataset.
 *
 * This method is used to set the GOES Imager and Sounder dataset based on GDAL
 * class VRTDataset.
 *
 * @param isSimple the WCS request type.  When user executing a DescribeCoverage
 * request, isSimple is set to 1, and for GetCoverage, is set to 0.
 *
 * @return CE_None on success or CE_Failure on failure.
 */

CPLErr DAP_Dataset::SetGDALDataset(const int isSimple)
{
    // There's only one band for now... and it's data are in m_src.
#if 0
    for(int i = 1; i <= maptr_DS->GetRasterCount(); ++i)
    mv_BandList.push_back(i);
#endif

    // NB: mi_RectifiedImageXSize & Y are set in SetGeoTransform()
    VRTDataset *poVDS = (VRTDataset *) VRTCreate(mi_RectifiedImageXSize, mi_RectifiedImageYSize);
    if (poVDS == NULL)
        throw Error("Failed to create VRT DataSet.");

    VRTSourcedRasterBand *poVRTBand = NULL;
    // The band is always double values for now...
#if 0
    GDALRasterBand *poSrcBand = NULL;
#endif
    GDALDataType eBandType;

#if 0
    for (unsigned int i = 0; i < mv_BandList.size(); i++)
    {
#endif
#if 0
    poSrcBand = maptr_DS->GetRasterBand(mv_BandList[i]);
    eBandType = poSrcBand->GetRasterDataType();
#endif
    int i = 0;
    eBandType = GDT_Float64;
    poVDS->AddBand(eBandType, NULL);
    poVRTBand = (VRTSourcedRasterBand *) poVDS->GetRasterBand(i + 1);
    poVRTBand->SetNoDataValue(md_MissingValue);

    double *data = extract_double_array(m_src);
    //GDALRasterBand srcBand;
    if (CE_None != poVRTBand->RasterIO(GF_Write, 0, 0, mi_RectifiedImageXSize, mi_RectifiedImageYSize, data,
                    mi_RectifiedImageXSize, mi_RectifiedImageYSize, eBandType, 0, 0)) {
        GDALClose((GDALDatasetH) poVDS);
        throw Error("Failed to satellite data band to VRT DataSet.");
    }

    delete[] data;

    /*
     CPLErr GDALRasterBand::RasterIO (   GDALRWFlag  eRWFlag,
     int     nXOff,
     int     nYOff,
     int     nXSize,
     int     nYSize,
     void *  pData,
     int     nBufXSize,
     int     nBufYSize,
     GDALDataType    eBufType,
     int     nPixelSpace,
     int     nLineSpace
     )  */

#if 0
    if (CE_None
            != poVRTBand->AddSimpleSource(&srcBand, 0, 0, mi_RectifiedImageXSize, mi_RectifiedImageYSize, 0, 0,
                    mi_RectifiedImageXSize, mi_RectifiedImageYSize, NULL, md_MissingValue)) {
        GDALClose((GDALDatasetH) poVDS);
        throw Error("Failed to satellite data band to VRT DataSet.");
    }
#endif
#if 0
}
#endif
    //set GCPs for this VRTDataset
    if (CE_None != SetGCPGeoRef4VRTDataset(poVDS)) {
        GDALClose((GDALDatasetH) poVDS);
        throw Error("Could not georeference the virtual dataset.");
    }

#if 0
    GDALClose(maptr_DS.release());
#endif

    maptr_DS.reset(poVDS);

    if (isSimple)
        return CE_None;

    return RectifyGOESDataSet();
}

/************************************************************************/
/*                       SetGCPGeoRef4VRTDataset()                      */
/************************************************************************/

/**
 * \brief Set the GCP array for the VRT dataset.
 *
 * This method is used to set the GCP array to created VRT dataset based on GDAL
 * method SetGCPs().
 *
 * @param poVDS The VRT dataset.
 *
 * @return CE_None on success or CE_Failure on failure.
 */

CPLErr DAP_Dataset::SetGCPGeoRef4VRTDataset(GDALDataset* poVDS)
{
    char* psTargetSRS;
    mo_NativeCRS.exportToWkt(&psTargetSRS);

#if (__GNUC__ >=4 && __GNUC_MINOR__ > 1)
    if (CE_None != poVDS->SetGCPs(m_gdalGCPs.size(), (GDAL_GCP*) (m_gdalGCPs.data()), psTargetSRS)) {
        OGRFree(psTargetSRS);
        throw Error("Failed to set GCPs.");
    }
#else
    {
        if(CE_None!=poVDS->SetGCPs(m_gdalGCPs.size(), (GDAL_GCP*)&m_gdalGCPs[0], psTargetSRS))
        {
            OGRFree( psTargetSRS );
            throw Error("Failed to set GCPs.");
        }
    }
#endif

    OGRFree(psTargetSRS);

    return CE_None;
}

/************************************************************************/
/*                        SetMetaDataList()                             */
/************************************************************************/

/**
 * \brief Set the metadata list for this coverage.
 *
 * The method will set the metadata list for the coverage based on its
 * corresponding GDALDataset object.
 *
 * @param hSrc the GDALDataset object corresponding to coverage.
 *
 * @return CE_None on success or CE_Failure on failure.
 */

CPLErr DAP_Dataset::SetMetaDataList(GDALDataset* hSrcDS)
{
    // TODO Remove
#if 0
    mv_MetaDataList.push_back("Product_Description=The data was created by GMU WCS from NOAA GOES satellite data.");
    mv_MetaDataList.push_back("unit=GVAR");
    mv_MetaDataList.push_back("FillValue=0");
    ms_FieldQuantityDef = "GVAR";
    ms_AllowRanges = "0 65535";
    ms_CoveragePlatform = "GOES-11";
    ms_CoverageInstrument = "GOES-11";
    ms_CoverageSensor = "Imager";
#endif

    return CE_None;
}

/************************************************************************/
/*                          GetGeoMinMax()                              */
/************************************************************************/

/**
 * \brief Get the min/max coordinates of laitutude and longitude.
 *
 * The method will fetch the min/max coordinates of laitutude and longitude.
 *
 * @param geoMinMax an existing four double buffer into which the
 * native geographical bounding box values will be placed.
 *
 * @return CE_None on success or CE_Failure on failure.
 */

CPLErr DAP_Dataset::GetGeoMinMax(double geoMinMax[])
{
    if (!mb_GeoTransformSet)
        return CE_Failure;

    geoMinMax[0] = mdSrcGeoMinX;
    geoMinMax[2] = mdSrcGeoMinY;
    geoMinMax[1] = mdSrcGeoMaxX;
    geoMinMax[3] = mdSrcGeoMaxY;

    return CE_None;
}

/************************************************************************/
/*                          RectifyGOESDataSet()                        */
/************************************************************************/

/**
 * \brief Convert the GOES dataset from satellite CRS project to grid CRS.
 *
 * The method will convert the GOES dataset from satellite CRS project to
 * grid CRS based on GDAL API GDALReprojectImage;
 *
 * @return CE_None on success or CE_Failure on failure.
 */

CPLErr DAP_Dataset::RectifyGOESDataSet()
{
    char *pszDstWKT;
    mo_NativeCRS.exportToWkt(&pszDstWKT);

    GDALDriverH poDriver = GDALGetDriverByName("MEM");
    GDALDataset* rectDataSet = (GDALDataset*) GDALCreate(poDriver, "", mi_RectifiedImageXSize, mi_RectifiedImageYSize,
            maptr_DS->GetRasterCount(), maptr_DS->GetRasterBand(1)->GetRasterDataType(), NULL);
    if (NULL == rectDataSet) {
        GDALClose(poDriver);
        OGRFree(pszDstWKT);
        throw Error("Failed to create \"MEM\" dataSet.");
    }

    rectDataSet->SetProjection(pszDstWKT);
    rectDataSet->SetGeoTransform(md_Geotransform);

    // FIXME Magic value of 0.125
    if (CE_None != GDALReprojectImage(maptr_DS.get(), NULL, rectDataSet, pszDstWKT, GRA_NearestNeighbour, 0, 0.125, NULL,
                    NULL, NULL)) {
        GDALClose(rectDataSet);
        GDALClose(poDriver);
        OGRFree(pszDstWKT);
        throw Error("Failed to re-project GOES data from satellite GCP CRS to geographical CRS.");
    }

    OGRFree(pszDstWKT);
    GDALClose(maptr_DS.release());

    maptr_DS.reset(rectDataSet);

    return CE_None;
}

} // namespace libdap
