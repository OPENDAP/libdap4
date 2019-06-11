Attributes {
    HDF5_GLOBAL {
        String HDFEOSVersion "HDFEOS_V2.18";
        String StructMetadata_0 "GROUP=SwathStructure
END_GROUP=SwathStructure
GROUP=GridStructure
	GROUP=GRID_1
		GridName=\"ascending\"
		XDim=360
		YDim=180
		UpperLeftPointMtrs=(-179030000.000000,89030000.000000)
		LowerRightMtrs=(180030000.000000,-90030000.000000)
		Projection=GCTP_GEO
		GROUP=Dimension
			OBJECT=Dimension_1
				DimensionName=\"StdPressureLev\"
				Size=24
			END_OBJECT=Dimension_1
			OBJECT=Dimension_2
				DimensionName=\"H2OPressureLev\"
				Size=12
			END_OBJECT=Dimension_2
			OBJECT=Dimension_3
				DimensionName=\"H2OPressureLay\"
				Size=12
			END_OBJECT=Dimension_3
			OBJECT=Dimension_4
				DimensionName=\"CoarseCloudLayer\"
				Size=3
			END_OBJECT=Dimension_4
			OBJECT=Dimension_5
				DimensionName=\"FineCloudLayer\"
				Size=12
			END_OBJECT=Dimension_5
			OBJECT=Dimension_6
				DimensionName=\"EmisFreqIR\"
				Size=4
			END_OBJECT=Dimension_6
		END_GROUP=Dimension
		GROUP=DataField
			OBJECT=DataField_1
				DataFieldName=\"TotalCounts_A\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_1
			OBJECT=DataField_2
				DataFieldName=\"SurfPres_Forecast_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_2
			OBJECT=DataField_3
				DataFieldName=\"SurfPres_Forecast_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_3
			OBJECT=DataField_4
				DataFieldName=\"SurfPres_Forecast_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_4
			OBJECT=DataField_5
				DataFieldName=\"SurfPres_Forecast_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_5
			OBJECT=DataField_6
				DataFieldName=\"SurfPres_Forecast_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_6
			OBJECT=DataField_7
				DataFieldName=\"SurfSkinTemp_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_7
			OBJECT=DataField_8
				DataFieldName=\"SurfSkinTemp_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_8
			OBJECT=DataField_9
				DataFieldName=\"SurfSkinTemp_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_9
			OBJECT=DataField_10
				DataFieldName=\"SurfSkinTemp_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_10
			OBJECT=DataField_11
				DataFieldName=\"SurfSkinTemp_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_11
			OBJECT=DataField_12
				DataFieldName=\"SurfSkinTemp_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_12
			OBJECT=DataField_13
				DataFieldName=\"EmisIR_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_13
			OBJECT=DataField_14
				DataFieldName=\"EmisIR_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_14
			OBJECT=DataField_15
				DataFieldName=\"EmisIR_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_15
			OBJECT=DataField_16
				DataFieldName=\"EmisIR_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_16
			OBJECT=DataField_17
				DataFieldName=\"EmisIR_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_17
			OBJECT=DataField_18
				DataFieldName=\"EmisIR_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_18
			OBJECT=DataField_19
				DataFieldName=\"Temperature_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_19
			OBJECT=DataField_20
				DataFieldName=\"Temperature_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_20
			OBJECT=DataField_21
				DataFieldName=\"Temperature_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_21
			OBJECT=DataField_22
				DataFieldName=\"Temperature_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_22
			OBJECT=DataField_23
				DataFieldName=\"Temperature_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_23
			OBJECT=DataField_24
				DataFieldName=\"Temperature_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_24
			OBJECT=DataField_25
				DataFieldName=\"SurfAirTemp_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_25
			OBJECT=DataField_26
				DataFieldName=\"SurfAirTemp_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_26
			OBJECT=DataField_27
				DataFieldName=\"SurfAirTemp_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_27
			OBJECT=DataField_28
				DataFieldName=\"SurfAirTemp_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_28
			OBJECT=DataField_29
				DataFieldName=\"SurfAirTemp_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_29
			OBJECT=DataField_30
				DataFieldName=\"SurfAirTemp_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_30
			OBJECT=DataField_31
				DataFieldName=\"TropPres_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_31
			OBJECT=DataField_32
				DataFieldName=\"TropPres_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_32
			OBJECT=DataField_33
				DataFieldName=\"TropPres_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_33
			OBJECT=DataField_34
				DataFieldName=\"TropPres_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_34
			OBJECT=DataField_35
				DataFieldName=\"TropPres_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_35
			OBJECT=DataField_36
				DataFieldName=\"TropTemp_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_36
			OBJECT=DataField_37
				DataFieldName=\"TropTemp_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_37
			OBJECT=DataField_38
				DataFieldName=\"TropTemp_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_38
			OBJECT=DataField_39
				DataFieldName=\"TropTemp_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_39
			OBJECT=DataField_40
				DataFieldName=\"TropTemp_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_40
			OBJECT=DataField_41
				DataFieldName=\"TotH2OVap_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_41
			OBJECT=DataField_42
				DataFieldName=\"TotH2OVap_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_42
			OBJECT=DataField_43
				DataFieldName=\"TotH2OVap_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_43
			OBJECT=DataField_44
				DataFieldName=\"TotH2OVap_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_44
			OBJECT=DataField_45
				DataFieldName=\"TotH2OVap_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_45
			OBJECT=DataField_46
				DataFieldName=\"TotH2OVap_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_46
			OBJECT=DataField_47
				DataFieldName=\"H2O_MMR_Lyr_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_47
			OBJECT=DataField_48
				DataFieldName=\"H2O_MMR_Lyr_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_48
			OBJECT=DataField_49
				DataFieldName=\"H2O_MMR_Lyr_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_49
			OBJECT=DataField_50
				DataFieldName=\"H2O_MMR_Lyr_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_50
			OBJECT=DataField_51
				DataFieldName=\"H2O_MMR_Lyr_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_51
			OBJECT=DataField_52
				DataFieldName=\"H2O_MMR_Lyr_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_52
			OBJECT=DataField_53
				DataFieldName=\"H2O_MMR_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_53
			OBJECT=DataField_54
				DataFieldName=\"H2O_MMR_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_54
			OBJECT=DataField_55
				DataFieldName=\"H2O_MMR_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_55
			OBJECT=DataField_56
				DataFieldName=\"H2O_MMR_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_56
			OBJECT=DataField_57
				DataFieldName=\"H2O_MMR_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_57
			OBJECT=DataField_58
				DataFieldName=\"H2O_MMR_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_58
			OBJECT=DataField_59
				DataFieldName=\"H2O_MMR_Surf_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_59
			OBJECT=DataField_60
				DataFieldName=\"H2O_MMR_Surf_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_60
			OBJECT=DataField_61
				DataFieldName=\"H2O_MMR_Surf_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_61
			OBJECT=DataField_62
				DataFieldName=\"H2O_MMR_Surf_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_62
			OBJECT=DataField_63
				DataFieldName=\"H2O_MMR_Surf_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_63
			OBJECT=DataField_64
				DataFieldName=\"H2O_MMR_Surf_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_64
			OBJECT=DataField_65
				DataFieldName=\"RelHum_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_65
			OBJECT=DataField_66
				DataFieldName=\"RelHum_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_66
			OBJECT=DataField_67
				DataFieldName=\"RelHum_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_67
			OBJECT=DataField_68
				DataFieldName=\"RelHum_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_68
			OBJECT=DataField_69
				DataFieldName=\"RelHum_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_69
			OBJECT=DataField_70
				DataFieldName=\"RelHumSurf_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_70
			OBJECT=DataField_71
				DataFieldName=\"RelHumSurf_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_71
			OBJECT=DataField_72
				DataFieldName=\"RelHumSurf_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_72
			OBJECT=DataField_73
				DataFieldName=\"RelHumSurf_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_73
			OBJECT=DataField_74
				DataFieldName=\"RelHumSurf_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_74
			OBJECT=DataField_75
				DataFieldName=\"RelHum_liquid_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_75
			OBJECT=DataField_76
				DataFieldName=\"RelHum_liquid_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_76
			OBJECT=DataField_77
				DataFieldName=\"RelHum_liquid_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_77
			OBJECT=DataField_78
				DataFieldName=\"RelHum_liquid_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_78
			OBJECT=DataField_79
				DataFieldName=\"RelHum_liquid_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_79
			OBJECT=DataField_80
				DataFieldName=\"RelHumSurf_liquid_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_80
			OBJECT=DataField_81
				DataFieldName=\"RelHumSurf_liquid_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_81
			OBJECT=DataField_82
				DataFieldName=\"RelHumSurf_liquid_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_82
			OBJECT=DataField_83
				DataFieldName=\"RelHumSurf_liquid_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_83
			OBJECT=DataField_84
				DataFieldName=\"RelHumSurf_liquid_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_84
			OBJECT=DataField_85
				DataFieldName=\"TropHeight_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_85
			OBJECT=DataField_86
				DataFieldName=\"TropHeight_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_86
			OBJECT=DataField_87
				DataFieldName=\"TropHeight_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_87
			OBJECT=DataField_88
				DataFieldName=\"TropHeight_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_88
			OBJECT=DataField_89
				DataFieldName=\"TropHeight_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_89
			OBJECT=DataField_90
				DataFieldName=\"GPHeight_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_90
			OBJECT=DataField_91
				DataFieldName=\"GPHeight_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_91
			OBJECT=DataField_92
				DataFieldName=\"GPHeight_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_92
			OBJECT=DataField_93
				DataFieldName=\"GPHeight_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_93
			OBJECT=DataField_94
				DataFieldName=\"GPHeight_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_94
			OBJECT=DataField_95
				DataFieldName=\"CloudFrc_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_95
			OBJECT=DataField_96
				DataFieldName=\"CloudFrc_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_96
			OBJECT=DataField_97
				DataFieldName=\"CloudFrc_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_97
			OBJECT=DataField_98
				DataFieldName=\"CloudFrc_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_98
			OBJECT=DataField_99
				DataFieldName=\"CloudFrc_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_99
			OBJECT=DataField_100
				DataFieldName=\"CloudFrc_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_100
			OBJECT=DataField_101
				DataFieldName=\"CloudTopPres_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_101
			OBJECT=DataField_102
				DataFieldName=\"CloudTopPres_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_102
			OBJECT=DataField_103
				DataFieldName=\"CloudTopPres_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_103
			OBJECT=DataField_104
				DataFieldName=\"CloudTopPres_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_104
			OBJECT=DataField_105
				DataFieldName=\"CloudTopPres_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_105
			OBJECT=DataField_106
				DataFieldName=\"CloudTopPres_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_106
			OBJECT=DataField_107
				DataFieldName=\"CloudTopTemp_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_107
			OBJECT=DataField_108
				DataFieldName=\"CloudTopTemp_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_108
			OBJECT=DataField_109
				DataFieldName=\"CloudTopTemp_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_109
			OBJECT=DataField_110
				DataFieldName=\"CloudTopTemp_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_110
			OBJECT=DataField_111
				DataFieldName=\"CloudTopTemp_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_111
			OBJECT=DataField_112
				DataFieldName=\"CloudTopTemp_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_112
			OBJECT=DataField_113
				DataFieldName=\"FineCloudFrc_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_113
			OBJECT=DataField_114
				DataFieldName=\"FineCloudFrc_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_114
			OBJECT=DataField_115
				DataFieldName=\"FineCloudFrc_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_115
			OBJECT=DataField_116
				DataFieldName=\"FineCloudFrc_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_116
			OBJECT=DataField_117
				DataFieldName=\"FineCloudFrc_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_117
			OBJECT=DataField_118
				DataFieldName=\"CoarseCloudFrc_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_118
			OBJECT=DataField_119
				DataFieldName=\"CoarseCloudFrc_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_119
			OBJECT=DataField_120
				DataFieldName=\"CoarseCloudFrc_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_120
			OBJECT=DataField_121
				DataFieldName=\"CoarseCloudFrc_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_121
			OBJECT=DataField_122
				DataFieldName=\"CoarseCloudFrc_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_122
			OBJECT=DataField_123
				DataFieldName=\"CoarseCloudPres_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_123
			OBJECT=DataField_124
				DataFieldName=\"CoarseCloudPres_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_124
			OBJECT=DataField_125
				DataFieldName=\"CoarseCloudPres_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_125
			OBJECT=DataField_126
				DataFieldName=\"CoarseCloudPres_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_126
			OBJECT=DataField_127
				DataFieldName=\"CoarseCloudPres_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_127
			OBJECT=DataField_128
				DataFieldName=\"CoarseCloudTemp_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_128
			OBJECT=DataField_129
				DataFieldName=\"CoarseCloudTemp_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_129
			OBJECT=DataField_130
				DataFieldName=\"CoarseCloudTemp_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_130
			OBJECT=DataField_131
				DataFieldName=\"CoarseCloudTemp_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_131
			OBJECT=DataField_132
				DataFieldName=\"CoarseCloudTemp_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_132
			OBJECT=DataField_133
				DataFieldName=\"TotO3_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_133
			OBJECT=DataField_134
				DataFieldName=\"TotO3_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_134
			OBJECT=DataField_135
				DataFieldName=\"TotO3_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_135
			OBJECT=DataField_136
				DataFieldName=\"TotO3_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_136
			OBJECT=DataField_137
				DataFieldName=\"TotO3_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_137
			OBJECT=DataField_138
				DataFieldName=\"TotO3_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_138
			OBJECT=DataField_139
				DataFieldName=\"O3_VMR_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_139
			OBJECT=DataField_140
				DataFieldName=\"O3_VMR_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_140
			OBJECT=DataField_141
				DataFieldName=\"O3_VMR_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_141
			OBJECT=DataField_142
				DataFieldName=\"O3_VMR_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_142
			OBJECT=DataField_143
				DataFieldName=\"O3_VMR_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_143
			OBJECT=DataField_144
				DataFieldName=\"O3_VMR_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_144
			OBJECT=DataField_145
				DataFieldName=\"TotCO_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_145
			OBJECT=DataField_146
				DataFieldName=\"TotCO_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_146
			OBJECT=DataField_147
				DataFieldName=\"TotCO_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_147
			OBJECT=DataField_148
				DataFieldName=\"TotCO_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_148
			OBJECT=DataField_149
				DataFieldName=\"TotCO_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_149
			OBJECT=DataField_150
				DataFieldName=\"CO_VMR_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_150
			OBJECT=DataField_151
				DataFieldName=\"CO_VMR_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_151
			OBJECT=DataField_152
				DataFieldName=\"CO_VMR_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
	";
        String StructMetadata_1 "			CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_152
			OBJECT=DataField_153
				DataFieldName=\"CO_VMR_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_153
			OBJECT=DataField_154
				DataFieldName=\"CO_VMR_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_154
			OBJECT=DataField_155
				DataFieldName=\"CO_VMR_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_155
			OBJECT=DataField_156
				DataFieldName=\"TotCH4_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_156
			OBJECT=DataField_157
				DataFieldName=\"TotCH4_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_157
			OBJECT=DataField_158
				DataFieldName=\"TotCH4_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_158
			OBJECT=DataField_159
				DataFieldName=\"TotCH4_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_159
			OBJECT=DataField_160
				DataFieldName=\"TotCH4_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_160
			OBJECT=DataField_161
				DataFieldName=\"CH4_VMR_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_161
			OBJECT=DataField_162
				DataFieldName=\"CH4_VMR_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_162
			OBJECT=DataField_163
				DataFieldName=\"CH4_VMR_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_163
			OBJECT=DataField_164
				DataFieldName=\"CH4_VMR_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_164
			OBJECT=DataField_165
				DataFieldName=\"CH4_VMR_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_165
			OBJECT=DataField_166
				DataFieldName=\"CH4_VMR_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_166
			OBJECT=DataField_167
				DataFieldName=\"OLR_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_167
			OBJECT=DataField_168
				DataFieldName=\"OLR_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_168
			OBJECT=DataField_169
				DataFieldName=\"OLR_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_169
			OBJECT=DataField_170
				DataFieldName=\"OLR_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_170
			OBJECT=DataField_171
				DataFieldName=\"OLR_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_171
			OBJECT=DataField_172
				DataFieldName=\"OLR_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_172
			OBJECT=DataField_173
				DataFieldName=\"ClrOLR_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_173
			OBJECT=DataField_174
				DataFieldName=\"ClrOLR_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_174
			OBJECT=DataField_175
				DataFieldName=\"ClrOLR_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_175
			OBJECT=DataField_176
				DataFieldName=\"ClrOLR_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_176
			OBJECT=DataField_177
				DataFieldName=\"ClrOLR_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_177
			OBJECT=DataField_178
				DataFieldName=\"ClrOLR_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_178
		END_GROUP=DataField
		GROUP=MergedFields
		END_GROUP=MergedFields
	END_GROUP=GRID_1
	GROUP=GRID_2
		GridName=\"descending\"
		XDim=360
		YDim=180
		UpperLeftPointMtrs=(-179030000.000000,89030000.000000)
		LowerRightMtrs=(180030000.000000,-90030000.000000)
		Projection=GCTP_GEO
		GROUP=Dimension
			OBJECT=Dimension_1
				DimensionName=\"StdPressureLev\"
				Size=24
			END_OBJECT=Dimension_1
			OBJECT=Dimension_2
				DimensionName=\"H2OPressureLev\"
				Size=12
			END_OBJECT=Dimension_2
			OBJECT=Dimension_3
				DimensionName=\"H2OPressureLay\"
				Size=12
			END_OBJECT=Dimension_3
			OBJECT=Dimension_4
				DimensionName=\"CoarseCloudLayer\"
				Size=3
			END_OBJECT=Dimension_4
			OBJECT=Dimension_5
				DimensionName=\"FineCloudLayer\"
				Size=12
			END_OBJECT=Dimension_5
			OBJECT=Dimension_6
				DimensionName=\"EmisFreqIR\"
				Size=4
			END_OBJECT=Dimension_6
		END_GROUP=Dimension
		GROUP=DataField
			OBJECT=DataField_1
				DataFieldName=\"TotalCounts_D\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_1
			OBJECT=DataField_2
				DataFieldName=\"SurfPres_Forecast_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_2
			OBJECT=DataField_3
				DataFieldName=\"SurfPres_Forecast_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_3
			OBJECT=DataField_4
				DataFieldName=\"SurfPres_Forecast_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_4
			OBJECT=DataField_5
				DataFieldName=\"SurfPres_Forecast_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_5
			OBJECT=DataField_6
				DataFieldName=\"SurfPres_Forecast_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_6
			OBJECT=DataField_7
				DataFieldName=\"SurfSkinTemp_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_7
			OBJECT=DataField_8
				DataFieldName=\"SurfSkinTemp_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_8
			OBJECT=DataField_9
				DataFieldName=\"SurfSkinTemp_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_9
			OBJECT=DataField_10
				DataFieldName=\"SurfSkinTemp_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_10
			OBJECT=DataField_11
				DataFieldName=\"SurfSkinTemp_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_11
			OBJECT=DataField_12
				DataFieldName=\"SurfSkinTemp_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_12
			OBJECT=DataField_13
				DataFieldName=\"EmisIR_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_13
			OBJECT=DataField_14
				DataFieldName=\"EmisIR_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_14
			OBJECT=DataField_15
				DataFieldName=\"EmisIR_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_15
			OBJECT=DataField_16
				DataFieldName=\"EmisIR_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_16
			OBJECT=DataField_17
				DataFieldName=\"EmisIR_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_17
			OBJECT=DataField_18
				DataFieldName=\"EmisIR_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_18
			OBJECT=DataField_19
				DataFieldName=\"Temperature_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_19
			OBJECT=DataField_20
				DataFieldName=\"Temperature_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_20
			OBJECT=DataField_21
				DataFieldName=\"Temperature_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_21
			OBJECT=DataField_22
				DataFieldName=\"Temperature_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_22
			OBJECT=DataField_23
				DataFieldName=\"Temperature_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_23
			OBJECT=DataField_24
				DataFieldName=\"Temperature_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_24
			OBJECT=DataField_25
				DataFieldName=\"SurfAirTemp_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_25
			OBJECT=DataField_26
				DataFieldName=\"SurfAirTemp_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_26
			OBJECT=DataField_27
				DataFieldName=\"SurfAirTemp_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_27
			OBJECT=DataField_28
				DataFieldName=\"SurfAirTemp_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_28
			OBJECT=DataField_29
				DataFieldName=\"SurfAirTemp_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_29
			OBJECT=DataField_30
				DataFieldName=\"SurfAirTemp_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_30
			OBJECT=DataField_31
				DataFieldName=\"TropPres_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_31
			OBJECT=DataField_32
				DataFieldName=\"TropPres_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_32
			OBJECT=DataField_33
				DataFieldName=\"TropPres_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_33
			OBJECT=DataField_34
				DataFieldName=\"TropPres_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_34
			OBJECT=DataField_35
				DataFieldName=\"TropPres_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_35
			OBJECT=DataField_36
				DataFieldName=\"TropTemp_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_36
			OBJECT=DataField_37
				DataFieldName=\"TropTemp_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_37
			OBJECT=DataField_38
				DataFieldName=\"TropTemp_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_38
			OBJECT=DataField_39
				DataFieldName=\"TropTemp_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_39
			OBJECT=DataField_40
				DataFieldName=\"TropTemp_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_40
			OBJECT=DataField_41
				DataFieldName=\"TotH2OVap_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_41
			OBJECT=DataField_42
				DataFieldName=\"TotH2OVap_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_42
			OBJECT=DataField_43
				DataFieldName=\"TotH2OVap_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_43
			OBJECT=DataField_44
				DataFieldName=\"TotH2OVap_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_44
			OBJECT=DataField_45
				DataFieldName=\"TotH2OVap_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_45
			OBJECT=DataField_46
				DataFieldName=\"TotH2OVap_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_46
			OBJECT=DataField_47
				DataFieldName=\"H2O_MMR_Lyr_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_47
			OBJECT=DataField_48
				DataFieldName=\"H2O_MMR_Lyr_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_48
			OBJECT=DataField_49
				DataFieldName=\"H2O_MMR_Lyr_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_49
			OBJECT=DataField_50
				DataFieldName=\"H2O_MMR_Lyr_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_50
			OBJECT=DataField_51
				DataFieldName=\"H2O_MMR_Lyr_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_51
			OBJECT=DataField_52
				DataFieldName=\"H2O_MMR_Lyr_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_52
			OBJECT=DataField_53
				DataFieldName=\"H2O_MMR_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_53
			OBJECT=DataField_54
				DataFieldName=\"H2O_MMR_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_54
			OBJECT=DataField_55
				DataFieldName=\"H2O_MMR_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_55
			OBJECT=DataField_56
				DataFieldName=\"H2O_MMR_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_56
			OBJECT=DataField_57
				DataFieldName=\"H2O_MMR_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_57
			OBJECT=DataField_58
				DataFieldName=\"H2O_MMR_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_58
			OBJECT=DataField_59
				DataFieldName=\"H2O_MMR_Surf_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_59
			OBJECT=DataField_60
				DataFieldName=\"H2O_MMR_Surf_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_60
			OBJECT=DataField_61
				DataFieldName=\"H2O_MMR_Surf_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_61
			OBJECT=DataField_62
				DataFieldName=\"H2O_MMR_Surf_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_62
			OBJECT=DataField_63
				DataFieldName=\"H2O_MMR_Surf_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_63
			OBJECT=DataField_64
				DataFieldName=\"H2O_MMR_Surf_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_64
			OBJECT=DataField_65
				DataFieldName=\"RelHum_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_65
			OBJECT=DataField_66
				DataFieldName=\"RelHum_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_66
			OBJECT=DataField_67
				DataFieldName=\"RelHum_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_67
			OBJECT=DataField_68
				DataFieldName=\"RelHum_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_68
			OBJECT=DataField_69
				DataFieldName=\"RelHum_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_69
			OBJECT=DataField_70
				DataFieldName=\"RelHumSurf_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_70
			OBJECT=DataField_71
				DataFieldName=\"RelHumSurf_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_71
			OBJECT=DataField_72
				DataFieldName=\"RelHumSurf_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_72
			OBJECT=DataField_73
				DataFieldName=\"RelHumSurf_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_73
			OBJECT=DataField_74
				DataFieldName=\"RelHumSurf_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_74
			OBJECT=DataField_75
				DataFieldName=\"RelHum_liquid_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_75
			OBJECT=DataField_76
				DataFieldName=\"RelHum_liquid_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_76
			OBJECT=DataField_77
				DataFieldName=\"RelHum_liquid_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_77
			OBJECT=DataField_78
				DataFieldName=\"RelHum_liquid_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_78
			OBJECT=DataField_79
				DataFieldName=\"RelHum_liquid_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_79
			OBJECT=DataField_80
				DataFieldName=\"RelHumSurf_liquid_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_80
			OBJECT=DataField_81
				DataFieldName=\"RelHumSurf_liquid_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_81
			OBJECT=DataField_82
				DataFieldName=\"RelHumSurf_liquid_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_82
			OBJECT=DataField_83
				DataFieldName=\"RelHumSurf_liquid_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_83
			OBJECT=DataField_84
				DataFieldName=\"RelHumSurf_liquid_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_84
			OBJECT=DataField_85
				DataFieldName=\"TropHeight_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_85
			OBJECT=DataField_86
				DataFieldName=\"TropHeight_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_86
			OBJECT=DataField_87
				DataFieldName=\"TropHeight_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_87
			OBJECT=DataField_88
				DataFieldName=\"TropHeight_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_88
			OBJECT=DataField_89
				DataFieldName=\"TropHeight_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_89
			OBJECT=DataField_90
				DataFieldName=\"GPHeight_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_90
			OBJECT=DataField_91
				DataFieldName=\"GPHeight_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_91
			OBJECT=DataField_92
				DataFieldName=\"GPHeight_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_92
			OBJECT=DataField_93
				DataFieldName=\"GPHeight_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_93
			OBJECT=DataField_94
				DataFieldName=\"GPHeight_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_94
			OBJECT=DataField_95
				DataFieldName=\"CloudFrc_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_95
			OBJECT=DataField_96
				DataFieldName=\"CloudFrc_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_96
			OBJECT=DataField_97
				DataFieldName=\"CloudFrc_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_97
			OBJECT=DataField_98
				DataFieldName=\"CloudFrc_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_98
			OBJECT=DataField_99
				DataFieldName=\"CloudFrc_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_99
			OBJECT=DataField_100
				DataFieldName=\"CloudFrc_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_100
			OBJECT=DataField_101
				DataFieldName=\"CloudTopPres_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_101
			OBJECT=DataField_102
				DataFieldName=\"CloudTopPres_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_102
			OBJECT=DataField_103
				DataFieldName=\"CloudTopPres_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_103
			OBJECT=DataField_104
				DataFieldName=\"CloudTopPres_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_104
			OBJECT=DataField_105
				DataFieldName=\"CloudTopPres_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_105
			OBJECT=DataField_106
				DataFieldName=\"CloudTopPres_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_106
			OBJECT=DataField_107
				DataFieldName=\"CloudTopTemp_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_107
			OBJECT=DataField_108
				DataFieldName=\"CloudTopTemp_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_108
			OBJECT=DataField_109
				DataFieldName=\"CloudTopTemp_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_109
			OBJECT=DataField_110
				DataFieldName=\"CloudTopTemp_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_110
			OBJECT=DataField_111
				DataFieldName=\"CloudTopTemp_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_111
			OBJECT=DataField_112
				DataFieldName=\"CloudTopTemp_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_112
			OBJECT=DataField_113
				DataFieldName=\"FineCloudFrc_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_113
			OBJECT=DataField_114
				DataFieldName=\"FineCloudFrc_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_114
			OBJECT=DataField_115
				DataFieldName=\"FineCloudFrc_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_115
			OBJECT=DataField_116
				DataFieldName=\"FineCloudFrc_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_116
			OBJECT=DataField_117
				DataFieldName=\"FineCloudFrc_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_117
			OBJECT=DataField_118
				DataFieldName=\"CoarseCloudFrc_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_118
			OBJECT=DataField_119
				DataFieldName=\"CoarseCloudFrc_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_119
			OBJECT=DataField_120
				DataFieldName=\"CoarseCloudFrc_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_120
			OBJECT=DataField_121
				DataFieldName=\"CoarseCloudFrc_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_121
			OBJECT=DataField_122
				DataFieldName=\"CoarseCloudFrc_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_122
			OBJECT=DataField_123
				DataFieldName=\"CoarseCloudPres_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_123
			OBJECT=DataField_124
				DataFieldName=\"CoarseCloudPres_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_124
			OBJECT=DataField_125
				DataFieldName=\"CoarseCloudPres_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_125
			OBJECT=DataField_126
				DataFieldName=\"CoarseCloudPres_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
		";
        String StructMetadata_2 "	END_OBJECT=DataField_126
			OBJECT=DataField_127
				DataFieldName=\"CoarseCloudPres_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_127
			OBJECT=DataField_128
				DataFieldName=\"CoarseCloudTemp_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_128
			OBJECT=DataField_129
				DataFieldName=\"CoarseCloudTemp_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_129
			OBJECT=DataField_130
				DataFieldName=\"CoarseCloudTemp_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_130
			OBJECT=DataField_131
				DataFieldName=\"CoarseCloudTemp_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_131
			OBJECT=DataField_132
				DataFieldName=\"CoarseCloudTemp_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_132
			OBJECT=DataField_133
				DataFieldName=\"TotO3_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_133
			OBJECT=DataField_134
				DataFieldName=\"TotO3_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_134
			OBJECT=DataField_135
				DataFieldName=\"TotO3_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_135
			OBJECT=DataField_136
				DataFieldName=\"TotO3_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_136
			OBJECT=DataField_137
				DataFieldName=\"TotO3_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_137
			OBJECT=DataField_138
				DataFieldName=\"TotO3_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_138
			OBJECT=DataField_139
				DataFieldName=\"O3_VMR_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_139
			OBJECT=DataField_140
				DataFieldName=\"O3_VMR_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_140
			OBJECT=DataField_141
				DataFieldName=\"O3_VMR_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_141
			OBJECT=DataField_142
				DataFieldName=\"O3_VMR_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_142
			OBJECT=DataField_143
				DataFieldName=\"O3_VMR_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_143
			OBJECT=DataField_144
				DataFieldName=\"O3_VMR_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_144
			OBJECT=DataField_145
				DataFieldName=\"TotCO_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_145
			OBJECT=DataField_146
				DataFieldName=\"TotCO_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_146
			OBJECT=DataField_147
				DataFieldName=\"TotCO_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_147
			OBJECT=DataField_148
				DataFieldName=\"TotCO_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_148
			OBJECT=DataField_149
				DataFieldName=\"TotCO_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_149
			OBJECT=DataField_150
				DataFieldName=\"CO_VMR_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_150
			OBJECT=DataField_151
				DataFieldName=\"CO_VMR_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_151
			OBJECT=DataField_152
				DataFieldName=\"CO_VMR_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_152
			OBJECT=DataField_153
				DataFieldName=\"CO_VMR_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_153
			OBJECT=DataField_154
				DataFieldName=\"CO_VMR_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_154
			OBJECT=DataField_155
				DataFieldName=\"CO_VMR_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_155
			OBJECT=DataField_156
				DataFieldName=\"TotCH4_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_156
			OBJECT=DataField_157
				DataFieldName=\"TotCH4_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_157
			OBJECT=DataField_158
				DataFieldName=\"TotCH4_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_158
			OBJECT=DataField_159
				DataFieldName=\"TotCH4_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_159
			OBJECT=DataField_160
				DataFieldName=\"TotCH4_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_160
			OBJECT=DataField_161
				DataFieldName=\"CH4_VMR_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_161
			OBJECT=DataField_162
				DataFieldName=\"CH4_VMR_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_162
			OBJECT=DataField_163
				DataFieldName=\"CH4_VMR_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_163
			OBJECT=DataField_164
				DataFieldName=\"CH4_VMR_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_164
			OBJECT=DataField_165
				DataFieldName=\"CH4_VMR_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_165
			OBJECT=DataField_166
				DataFieldName=\"CH4_VMR_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_166
			OBJECT=DataField_167
				DataFieldName=\"OLR_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_167
			OBJECT=DataField_168
				DataFieldName=\"OLR_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_168
			OBJECT=DataField_169
				DataFieldName=\"OLR_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_169
			OBJECT=DataField_170
				DataFieldName=\"OLR_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_170
			OBJECT=DataField_171
				DataFieldName=\"OLR_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_171
			OBJECT=DataField_172
				DataFieldName=\"OLR_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_172
			OBJECT=DataField_173
				DataFieldName=\"ClrOLR_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_173
			OBJECT=DataField_174
				DataFieldName=\"ClrOLR_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_174
			OBJECT=DataField_175
				DataFieldName=\"ClrOLR_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_175
			OBJECT=DataField_176
				DataFieldName=\"ClrOLR_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_176
			OBJECT=DataField_177
				DataFieldName=\"ClrOLR_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_177
			OBJECT=DataField_178
				DataFieldName=\"ClrOLR_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_178
		END_GROUP=DataField
		GROUP=MergedFields
		END_GROUP=MergedFields
	END_GROUP=GRID_2
	GROUP=GRID_3
		GridName=\"ascending_TqJoint\"
		XDim=360
		YDim=180
		UpperLeftPointMtrs=(-179030000.000000,89030000.000000)
		LowerRightMtrs=(180030000.000000,-90030000.000000)
		Projection=GCTP_GEO
		GROUP=Dimension
			OBJECT=Dimension_1
				DimensionName=\"StdPressureLev\"
				Size=24
			END_OBJECT=Dimension_1
			OBJECT=Dimension_2
				DimensionName=\"H2OPressureLev\"
				Size=12
			END_OBJECT=Dimension_2
			OBJECT=Dimension_3
				DimensionName=\"H2OPressureLay\"
				Size=12
			END_OBJECT=Dimension_3
			OBJECT=Dimension_4
				DimensionName=\"CoarseCloudLayer\"
				Size=3
			END_OBJECT=Dimension_4
			OBJECT=Dimension_5
				DimensionName=\"FineCloudLayer\"
				Size=12
			END_OBJECT=Dimension_5
			OBJECT=Dimension_6
				DimensionName=\"EmisFreqIR\"
				Size=4
			END_OBJECT=Dimension_6
		END_GROUP=Dimension
		GROUP=DataField
			OBJECT=DataField_1
				DataFieldName=\"TotalCounts_TqJ_A\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_1
			OBJECT=DataField_2
				DataFieldName=\"SurfPres_Forecast_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_2
			OBJECT=DataField_3
				DataFieldName=\"SurfPres_Forecast_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_3
			OBJECT=DataField_4
				DataFieldName=\"SurfPres_Forecast_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_4
			OBJECT=DataField_5
				DataFieldName=\"SurfPres_Forecast_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_5
			OBJECT=DataField_6
				DataFieldName=\"SurfPres_Forecast_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_6
			OBJECT=DataField_7
				DataFieldName=\"SurfSkinTemp_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_7
			OBJECT=DataField_8
				DataFieldName=\"SurfSkinTemp_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_8
			OBJECT=DataField_9
				DataFieldName=\"SurfSkinTemp_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_9
			OBJECT=DataField_10
				DataFieldName=\"SurfSkinTemp_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_10
			OBJECT=DataField_11
				DataFieldName=\"SurfSkinTemp_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_11
			OBJECT=DataField_12
				DataFieldName=\"SurfSkinTemp_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_12
			OBJECT=DataField_13
				DataFieldName=\"EmisIR_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_13
			OBJECT=DataField_14
				DataFieldName=\"EmisIR_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_14
			OBJECT=DataField_15
				DataFieldName=\"EmisIR_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_15
			OBJECT=DataField_16
				DataFieldName=\"EmisIR_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_16
			OBJECT=DataField_17
				DataFieldName=\"EmisIR_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_17
			OBJECT=DataField_18
				DataFieldName=\"EmisIR_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_18
			OBJECT=DataField_19
				DataFieldName=\"Temperature_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_19
			OBJECT=DataField_20
				DataFieldName=\"Temperature_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_20
			OBJECT=DataField_21
				DataFieldName=\"Temperature_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_21
			OBJECT=DataField_22
				DataFieldName=\"Temperature_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_22
			OBJECT=DataField_23
				DataFieldName=\"Temperature_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_23
			OBJECT=DataField_24
				DataFieldName=\"Temperature_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_24
			OBJECT=DataField_25
				DataFieldName=\"SurfAirTemp_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_25
			OBJECT=DataField_26
				DataFieldName=\"SurfAirTemp_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_26
			OBJECT=DataField_27
				DataFieldName=\"SurfAirTemp_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_27
			OBJECT=DataField_28
				DataFieldName=\"SurfAirTemp_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_28
			OBJECT=DataField_29
				DataFieldName=\"SurfAirTemp_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_29
			OBJECT=DataField_30
				DataFieldName=\"SurfAirTemp_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_30
			OBJECT=DataField_31
				DataFieldName=\"TropPres_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_31
			OBJECT=DataField_32
				DataFieldName=\"TropPres_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_32
			OBJECT=DataField_33
				DataFieldName=\"TropPres_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_33
			OBJECT=DataField_34
				DataFieldName=\"TropPres_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_34
			OBJECT=DataField_35
				DataFieldName=\"TropPres_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_35
			OBJECT=DataField_36
				DataFieldName=\"TropTemp_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_36
			OBJECT=DataField_37
				DataFieldName=\"TropTemp_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_37
			OBJECT=DataField_38
				DataFieldName=\"TropTemp_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_38
			OBJECT=DataField_39
				DataFieldName=\"TropTemp_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_39
			OBJECT=DataField_40
				DataFieldName=\"TropTemp_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_40
			OBJECT=DataField_41
				DataFieldName=\"TotH2OVap_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_41
			OBJECT=DataField_42
				DataFieldName=\"TotH2OVap_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_42
			OBJECT=DataField_43
				DataFieldName=\"TotH2OVap_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_43
			OBJECT=DataField_44
				DataFieldName=\"TotH2OVap_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_44
			OBJECT=DataField_45
				DataFieldName=\"TotH2OVap_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_45
			OBJECT=DataField_46
				DataFieldName=\"TotH2OVap_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_46
			OBJECT=DataField_47
				DataFieldName=\"H2O_MMR_Lyr_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_47
			OBJECT=DataField_48
				DataFieldName=\"H2O_MMR_Lyr_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_48
			OBJECT=DataField_49
				DataFieldName=\"H2O_MMR_Lyr_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_49
			OBJECT=DataField_50
				DataFieldName=\"H2O_MMR_Lyr_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_50
			OBJECT=DataField_51
				DataFieldName=\"H2O_MMR_Lyr_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_51
			OBJECT=DataField_52
				DataFieldName=\"H2O_MMR_Lyr_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_52
			OBJECT=DataField_53
				DataFieldName=\"H2O_MMR_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_53
			OBJECT=DataField_54
				DataFieldName=\"H2O_MMR_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_54
			OBJECT=DataField_55
				DataFieldName=\"H2O_MMR_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_55
			OBJECT=DataField_56
				DataFieldName=\"H2O_MMR_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_56
			OBJECT=DataField_57
				DataFieldName=\"H2O_MMR_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_57
			OBJECT=DataField_58
				DataFieldName=\"H2O_MMR_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_58
			OBJECT=DataField_59
				DataFieldName=\"H2O_MMR_Surf_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_59
			OBJECT=DataField_60
				DataFieldName=\"H2O_MMR_Surf_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_60
			OBJECT=DataField_61
				DataFieldName=\"H2O_MMR_Surf_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_61
			OBJECT=DataField_62
				DataFieldName=\"H2O_MMR_Surf_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_62
			OBJECT=DataField_63
				DataFieldName=\"H2O_MMR_Surf_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_63
			OBJECT=DataField_64
				DataFieldName=\"H2O_MMR_Surf_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_64
			OBJECT=DataField_65
				DataFieldName=\"RelHum_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_65
			OBJECT=DataField_66
				DataFieldName=\"RelHum_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_66
			OBJECT=DataField_67
				DataFieldName=\"RelHum_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_67
			OBJECT=DataField_68
				DataFieldName=\"RelHum_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_68
			OBJECT=DataField_69
				DataFieldName=\"RelHum_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_69
			OBJECT=DataField_70
				DataFieldName=\"RelHumSurf_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_70
			OBJECT=DataField_71
				DataFieldName=\"RelHumSurf_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_71
			OBJECT=DataField_72
				DataFieldName=\"RelHumSurf_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_72
			OBJECT=DataField_73
				DataFieldName=\"RelHumSurf_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_73
			OBJECT=DataField_74
				DataFieldName=\"RelHumSurf_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_74
			OBJECT=DataField_75
				DataFieldName=\"RelHum_liquid_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_75
			OBJECT=DataField_76
				DataFieldName=\"RelHum_liquid_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_76
			OBJECT=DataField_77
				DataFieldName=\"RelHum_liquid_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_77
			OBJECT=DataField_78
				DataFieldName=\"RelHum_liquid_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_78
			OBJECT=DataField_79
				DataFieldName=\"RelHum_liquid_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_79
			OBJECT=DataField_80
				DataFieldName=\"RelHumSurf_liquid_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_80
			OBJECT=DataField_81
				DataFieldName=\"RelHumSurf_liquid_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_81
			OBJECT=DataField_82
				DataFieldName=\"RelHumSurf_liquid_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_82
			OBJECT=DataField_83
				DataFieldName=\"RelHumSurf_liquid_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_83
			OBJECT=DataField_84
				DataFieldName=\"RelHumSurf_liquid_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_84
			OBJECT=DataField_85
				DataFieldName=\"TropHeight_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_85
			OBJECT=DataField_86
				DataFieldName=\"TropHeight_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_86
			OBJECT=DataField_87
				DataFieldName=\"TropHeight_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_87
			OBJECT=DataField_88
				DataFieldName=\"TropHeight_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_88
			OBJECT=DataField_89
				DataFieldName=\"TropHeight_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_89
			OBJECT=DataField_90
				DataFieldName=\"GPHeight_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_90
			OBJECT=DataField_91
				DataFieldName=\"GPHeight_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_91
			OBJECT=DataField_92
				DataFieldName=\"GPHeight_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_92
			OBJECT=DataField_93
				DataFieldName=\"GPHeight_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_93
			OBJECT=DataField_94
				DataFieldName=\"GPHeight_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_94
			OBJECT=DataField_95
				DataFieldName=\"CloudFrc_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_95
			OBJECT=DataField_96
				DataFieldName=\"CloudFrc_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_96
			OBJECT=DataField_97
				DataFieldName=\"CloudFrc_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_97
			OBJECT=DataField_98
				DataFieldName=\"CloudFrc_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_98
			OBJECT=DataField_99
				DataFieldName=\"CloudFrc_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLA";
        String StructMetadata_3 "TE
				DeflateLevel=5
			END_OBJECT=DataField_99
			OBJECT=DataField_100
				DataFieldName=\"CloudFrc_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_100
			OBJECT=DataField_101
				DataFieldName=\"CloudTopPres_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_101
			OBJECT=DataField_102
				DataFieldName=\"CloudTopPres_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_102
			OBJECT=DataField_103
				DataFieldName=\"CloudTopPres_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_103
			OBJECT=DataField_104
				DataFieldName=\"CloudTopPres_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_104
			OBJECT=DataField_105
				DataFieldName=\"CloudTopPres_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_105
			OBJECT=DataField_106
				DataFieldName=\"CloudTopPres_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_106
			OBJECT=DataField_107
				DataFieldName=\"CloudTopTemp_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_107
			OBJECT=DataField_108
				DataFieldName=\"CloudTopTemp_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_108
			OBJECT=DataField_109
				DataFieldName=\"CloudTopTemp_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_109
			OBJECT=DataField_110
				DataFieldName=\"CloudTopTemp_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_110
			OBJECT=DataField_111
				DataFieldName=\"CloudTopTemp_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_111
			OBJECT=DataField_112
				DataFieldName=\"CloudTopTemp_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_112
			OBJECT=DataField_113
				DataFieldName=\"FineCloudFrc_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_113
			OBJECT=DataField_114
				DataFieldName=\"FineCloudFrc_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_114
			OBJECT=DataField_115
				DataFieldName=\"FineCloudFrc_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_115
			OBJECT=DataField_116
				DataFieldName=\"FineCloudFrc_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_116
			OBJECT=DataField_117
				DataFieldName=\"FineCloudFrc_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_117
			OBJECT=DataField_118
				DataFieldName=\"CoarseCloudFrc_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_118
			OBJECT=DataField_119
				DataFieldName=\"CoarseCloudFrc_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_119
			OBJECT=DataField_120
				DataFieldName=\"CoarseCloudFrc_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_120
			OBJECT=DataField_121
				DataFieldName=\"CoarseCloudFrc_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_121
			OBJECT=DataField_122
				DataFieldName=\"CoarseCloudFrc_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_122
			OBJECT=DataField_123
				DataFieldName=\"CoarseCloudPres_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_123
			OBJECT=DataField_124
				DataFieldName=\"CoarseCloudPres_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_124
			OBJECT=DataField_125
				DataFieldName=\"CoarseCloudPres_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_125
			OBJECT=DataField_126
				DataFieldName=\"CoarseCloudPres_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_126
			OBJECT=DataField_127
				DataFieldName=\"CoarseCloudPres_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_127
			OBJECT=DataField_128
				DataFieldName=\"CoarseCloudTemp_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_128
			OBJECT=DataField_129
				DataFieldName=\"CoarseCloudTemp_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_129
			OBJECT=DataField_130
				DataFieldName=\"CoarseCloudTemp_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_130
			OBJECT=DataField_131
				DataFieldName=\"CoarseCloudTemp_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_131
			OBJECT=DataField_132
				DataFieldName=\"CoarseCloudTemp_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_132
			OBJECT=DataField_133
				DataFieldName=\"TotO3_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_133
			OBJECT=DataField_134
				DataFieldName=\"TotO3_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_134
			OBJECT=DataField_135
				DataFieldName=\"TotO3_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_135
			OBJECT=DataField_136
				DataFieldName=\"TotO3_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_136
			OBJECT=DataField_137
				DataFieldName=\"TotO3_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_137
			OBJECT=DataField_138
				DataFieldName=\"TotO3_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_138
			OBJECT=DataField_139
				DataFieldName=\"O3_VMR_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_139
			OBJECT=DataField_140
				DataFieldName=\"O3_VMR_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_140
			OBJECT=DataField_141
				DataFieldName=\"O3_VMR_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_141
			OBJECT=DataField_142
				DataFieldName=\"O3_VMR_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_142
			OBJECT=DataField_143
				DataFieldName=\"O3_VMR_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_143
			OBJECT=DataField_144
				DataFieldName=\"O3_VMR_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_144
			OBJECT=DataField_145
				DataFieldName=\"TotCO_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_145
			OBJECT=DataField_146
				DataFieldName=\"TotCO_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_146
			OBJECT=DataField_147
				DataFieldName=\"TotCO_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_147
			OBJECT=DataField_148
				DataFieldName=\"TotCO_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_148
			OBJECT=DataField_149
				DataFieldName=\"TotCO_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_149
			OBJECT=DataField_150
				DataFieldName=\"CO_VMR_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_150
			OBJECT=DataField_151
				DataFieldName=\"CO_VMR_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_151
			OBJECT=DataField_152
				DataFieldName=\"CO_VMR_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_152
			OBJECT=DataField_153
				DataFieldName=\"CO_VMR_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_153
			OBJECT=DataField_154
				DataFieldName=\"CO_VMR_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_154
			OBJECT=DataField_155
				DataFieldName=\"CO_VMR_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_155
			OBJECT=DataField_156
				DataFieldName=\"TotCH4_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_156
			OBJECT=DataField_157
				DataFieldName=\"TotCH4_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_157
			OBJECT=DataField_158
				DataFieldName=\"TotCH4_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_158
			OBJECT=DataField_159
				DataFieldName=\"TotCH4_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_159
			OBJECT=DataField_160
				DataFieldName=\"TotCH4_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_160
			OBJECT=DataField_161
				DataFieldName=\"CH4_VMR_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_161
			OBJECT=DataField_162
				DataFieldName=\"CH4_VMR_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_162
			OBJECT=DataField_163
				DataFieldName=\"CH4_VMR_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_163
			OBJECT=DataField_164
				DataFieldName=\"CH4_VMR_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_164
			OBJECT=DataField_165
				DataFieldName=\"CH4_VMR_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_165
			OBJECT=DataField_166
				DataFieldName=\"CH4_VMR_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_166
			OBJECT=DataField_167
				DataFieldName=\"OLR_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_167
			OBJECT=DataField_168
				DataFieldName=\"OLR_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_168
			OBJECT=DataField_169
				DataFieldName=\"OLR_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_169
			OBJECT=DataField_170
				DataFieldName=\"OLR_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_170
			OBJECT=DataField_171
				DataFieldName=\"OLR_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_171
			OBJECT=DataField_172
				DataFieldName=\"OLR_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_172
			OBJECT=DataField_173
				DataFieldName=\"ClrOLR_TqJ_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_173
			OBJECT=DataField_174
				DataFieldName=\"ClrOLR_TqJ_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_174
			OBJECT=DataField_175
				DataFieldName=\"ClrOLR_TqJ_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_175
			OBJECT=DataField_176
				DataFieldName=\"ClrOLR_TqJ_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_176
			OBJECT=DataField_177
				DataFieldName=\"ClrOLR_TqJ_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_177
			OBJECT=DataField_178
				DataFieldName=\"ClrOLR_TqJ_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_178
		END_GROUP=DataField
		GROUP=MergedFields
		END_GROUP=MergedFields
	END_GROUP=GRID_3
	GROUP=GRID_4
		GridName=\"descending_TqJoint\"
		XDim=360
		YDim=180
		UpperLeftPointMtrs=(-179030000.000000,89030000.000000)
		LowerRightMtrs=(180030000.000000,-90030000.000000)
		Projection=GCTP_GEO
		GROUP=Dimension
			OBJECT=Dimension_1
				DimensionName=\"StdPressureLev\"
				Size=24
			END_OBJECT=Dimension_1
			OBJECT=Dimension_2
				DimensionName=\"H2OPressureLev\"
				Size=12
			END_OBJECT=Dimension_2
			OBJECT=Dimension_3
				DimensionName=\"H2OPressureLay\"
				Size=12
			END_OBJECT=Dimension_3
			OBJECT=Dimension_4
				DimensionName=\"CoarseCloudLayer\"
				Size=3
			END_OBJECT=Dimension_4
			OBJECT=Dimension_5
				DimensionName=\"FineCloudLayer\"
				Size=12
			END_OBJECT=Dimension_5
			OBJECT=Dimension_6
				DimensionName=\"EmisFreqIR\"
				Size=4
			END_OBJECT=Dimension_6
		END_GROUP=Dimension
		GROUP=DataField
			OBJECT=DataField_1
				DataFieldName=\"TotalCounts_TqJ_D\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_1
			OBJECT=DataField_2
				DataFieldName=\"SurfPres_Forecast_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_2
			OBJECT=DataField_3
				DataFieldName=\"SurfPres_Forecast_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_3
			OBJECT=DataField_4
				DataFieldName=\"SurfPres_Forecast_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_4
			OBJECT=DataField_5
				DataFieldName=\"SurfPres_Forecast_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_5
			OBJECT=DataField_6
				DataFieldName=\"SurfPres_Forecast_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_6
			OBJECT=DataField_7
				DataFieldName=\"SurfSkinTemp_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_7
			OBJECT=DataField_8
				DataFieldName=\"SurfSkinTemp_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_8
			OBJECT=DataField_9
				DataFieldName=\"SurfSkinTemp_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_9
			OBJECT=DataField_10
				DataFieldName=\"SurfSkinTemp_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_10
			OBJECT=DataField_11
				DataFieldName=\"SurfSkinTemp_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_11
			OBJECT=DataField_12
				DataFieldName=\"SurfSkinTemp_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_12
			OBJECT=DataField_13
				DataFieldName=\"EmisIR_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_13
			OBJECT=DataField_14
				DataFieldName=\"EmisIR_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_14
			OBJECT=DataField_15
				DataFieldName=\"EmisIR_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_15
			OBJECT=DataField_16
				DataFieldName=\"EmisIR_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_16
			OBJECT=DataField_17
				DataFieldName=\"EmisIR_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_17
			OBJECT=DataField_18
				DataFieldName=\"EmisIR_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqIR\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_18
			OBJECT=DataField_19
				DataFieldName=\"Temperature_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_19
			OBJECT=DataField_20
				DataFieldName=\"Temperature_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_20
			OBJECT=DataField_21
				DataFieldName=\"Temperature_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_21
			OBJECT=DataField_22
				DataFieldName=\"Temperature_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_22
			OBJECT=DataField_23
				DataFieldName=\"Temperature_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_23
			OBJECT=DataField_24
				DataFieldName=\"Temperature_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_24
			OBJECT=DataField_25
				DataFieldName=\"SurfAirTemp_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_25
			OBJECT=DataField_26
				DataFieldName=\"SurfAirTemp_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_26
			OBJECT=DataField_27
				DataFieldName=\"SurfAirTemp_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_27
			OBJECT=DataField_28
				DataFieldName=\"SurfAirTemp_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_28
			OBJECT=DataField_29
				DataFieldName=\"SurfAirTemp_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_29
			OBJECT=DataField_30
				DataFieldName=\"SurfAirTemp_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_30
			OBJECT=DataField_31
				DataFieldName=\"TropPres_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_31
			OBJECT=DataField_32
				DataFieldName=\"TropPres_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_32
			OBJECT=DataField_33
				DataFieldName=\"TropPres_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_33
			OBJECT=DataField_34
				DataFieldName=\"TropPres_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_34
			OBJECT=DataField_35
				DataFieldName=\"TropPres_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_35
			OBJECT=DataField_36
				DataFieldName=\"TropTemp_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_36
			OBJECT=DataField_37
				DataFieldName=\"TropTemp_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_37
			OBJECT=DataField_38
				DataFieldName=\"TropTemp_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_38
			OBJECT=DataField_39
				DataFieldName=\"TropTemp_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_39
			OBJECT=DataField_40
				DataFieldName=\"TropTemp_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_40
			OBJECT=DataField_41
				DataFieldName=\"TotH2OVap_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_41
			OBJECT=DataField_42
				DataFieldName=\"TotH2OVap_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_42
			OBJECT=DataField_43
				DataFieldName=\"TotH2OVap_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_43
			OBJECT=DataField_44
				DataFieldName=\"TotH2OVap_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_44
			OBJECT=DataField_45
				DataFieldName=\"TotH2OVap_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_45
			OBJECT=DataField_46
				DataFieldName=\"TotH2OVap_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_46
			OBJECT=DataField_47
				DataFieldName=\"H2O_MMR_Lyr_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_47
			OBJECT=DataField_48
				DataFieldName=\"H2O_MMR_Lyr_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_48
			OBJECT=DataField_49
				DataFieldName=\"H2O_MMR_Lyr_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_49
			OBJECT=DataField_50
				DataFieldName=\"H2O_MMR_Lyr_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_50
			OBJECT=DataField_51
				DataFieldName=\"H2O_MMR_Lyr_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_51
			OBJECT=DataField_52
				DataFieldName=\"H2O_MMR_Lyr_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLay\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_52
			OBJECT=DataField_53
				DataFieldName=\"H2O_MMR_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_53
			OBJECT=DataField_54
				DataFieldName=\"H2O_MMR_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_54
			OBJECT=DataField_55
				DataFieldName=\"H2O_MMR_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_55
			OBJECT=DataField_56
				DataFieldName=\"H2O_MMR_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_56
			OBJECT=DataField_57
				DataFieldName=\"H2O_MMR_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_57
			OBJECT=DataField_58
				DataFieldName=\"H2O_MMR_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_58
			OBJECT=DataField_59
				DataFieldName=\"H2O_MMR_Surf_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_59
			OBJECT=DataField_60
				DataFieldName=\"H2O_MMR_Surf_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_60
			OBJECT=DataField_61
				DataFieldName=\"H2O_MMR_Surf_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_61
			OBJECT=DataField_62
				DataFieldName=\"H2O_MMR_Surf_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_62
			OBJECT=DataField_63
				DataFieldName=\"H2O_MMR_Surf_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_63
			OBJECT=DataField_64
				DataFieldName=\"H2O_MMR_Surf_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_64
			OBJECT=DataField_65
				DataFieldName=\"RelHum_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_65
			OBJECT=DataField_66
				DataFieldName=\"RelHum_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_66
			OBJECT=DataField_67
				DataFieldName=\"RelHum_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_67
			OBJECT=DataField_68
				DataFieldName=\"RelHum_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_68
			OBJECT=DataField_69
				DataFieldName=\"RelHum_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_69
			OBJECT=DataField_70
				DataFieldName=\"RelHumSurf_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDF";
        String StructMetadata_4 "E_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_70
			OBJECT=DataField_71
				DataFieldName=\"RelHumSurf_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_71
			OBJECT=DataField_72
				DataFieldName=\"RelHumSurf_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_72
			OBJECT=DataField_73
				DataFieldName=\"RelHumSurf_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_73
			OBJECT=DataField_74
				DataFieldName=\"RelHumSurf_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_74
			OBJECT=DataField_75
				DataFieldName=\"RelHum_liquid_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_75
			OBJECT=DataField_76
				DataFieldName=\"RelHum_liquid_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_76
			OBJECT=DataField_77
				DataFieldName=\"RelHum_liquid_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_77
			OBJECT=DataField_78
				DataFieldName=\"RelHum_liquid_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_78
			OBJECT=DataField_79
				DataFieldName=\"RelHum_liquid_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"H2OPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_79
			OBJECT=DataField_80
				DataFieldName=\"RelHumSurf_liquid_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_80
			OBJECT=DataField_81
				DataFieldName=\"RelHumSurf_liquid_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_81
			OBJECT=DataField_82
				DataFieldName=\"RelHumSurf_liquid_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_82
			OBJECT=DataField_83
				DataFieldName=\"RelHumSurf_liquid_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_83
			OBJECT=DataField_84
				DataFieldName=\"RelHumSurf_liquid_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_84
			OBJECT=DataField_85
				DataFieldName=\"TropHeight_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_85
			OBJECT=DataField_86
				DataFieldName=\"TropHeight_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_86
			OBJECT=DataField_87
				DataFieldName=\"TropHeight_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_87
			OBJECT=DataField_88
				DataFieldName=\"TropHeight_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_88
			OBJECT=DataField_89
				DataFieldName=\"TropHeight_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_89
			OBJECT=DataField_90
				DataFieldName=\"GPHeight_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_90
			OBJECT=DataField_91
				DataFieldName=\"GPHeight_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_91
			OBJECT=DataField_92
				DataFieldName=\"GPHeight_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_92
			OBJECT=DataField_93
				DataFieldName=\"GPHeight_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_93
			OBJECT=DataField_94
				DataFieldName=\"GPHeight_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_94
			OBJECT=DataField_95
				DataFieldName=\"CloudFrc_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_95
			OBJECT=DataField_96
				DataFieldName=\"CloudFrc_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_96
			OBJECT=DataField_97
				DataFieldName=\"CloudFrc_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_97
			OBJECT=DataField_98
				DataFieldName=\"CloudFrc_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_98
			OBJECT=DataField_99
				DataFieldName=\"CloudFrc_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_99
			OBJECT=DataField_100
				DataFieldName=\"CloudFrc_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_100
			OBJECT=DataField_101
				DataFieldName=\"CloudTopPres_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_101
			OBJECT=DataField_102
				DataFieldName=\"CloudTopPres_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_102
			OBJECT=DataField_103
				DataFieldName=\"CloudTopPres_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_103
			OBJECT=DataField_104
				DataFieldName=\"CloudTopPres_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_104
			OBJECT=DataField_105
				DataFieldName=\"CloudTopPres_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_105
			OBJECT=DataField_106
				DataFieldName=\"CloudTopPres_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_106
			OBJECT=DataField_107
				DataFieldName=\"CloudTopTemp_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_107
			OBJECT=DataField_108
				DataFieldName=\"CloudTopTemp_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_108
			OBJECT=DataField_109
				DataFieldName=\"CloudTopTemp_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_109
			OBJECT=DataField_110
				DataFieldName=\"CloudTopTemp_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_110
			OBJECT=DataField_111
				DataFieldName=\"CloudTopTemp_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_111
			OBJECT=DataField_112
				DataFieldName=\"CloudTopTemp_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_112
			OBJECT=DataField_113
				DataFieldName=\"FineCloudFrc_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_113
			OBJECT=DataField_114
				DataFieldName=\"FineCloudFrc_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_114
			OBJECT=DataField_115
				DataFieldName=\"FineCloudFrc_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_115
			OBJECT=DataField_116
				DataFieldName=\"FineCloudFrc_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_116
			OBJECT=DataField_117
				DataFieldName=\"FineCloudFrc_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"FineCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_117
			OBJECT=DataField_118
				DataFieldName=\"CoarseCloudFrc_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_118
			OBJECT=DataField_119
				DataFieldName=\"CoarseCloudFrc_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_119
			OBJECT=DataField_120
				DataFieldName=\"CoarseCloudFrc_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_120
			OBJECT=DataField_121
				DataFieldName=\"CoarseCloudFrc_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_121
			OBJECT=DataField_122
				DataFieldName=\"CoarseCloudFrc_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_122
			OBJECT=DataField_123
				DataFieldName=\"CoarseCloudPres_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_123
			OBJECT=DataField_124
				DataFieldName=\"CoarseCloudPres_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_124
			OBJECT=DataField_125
				DataFieldName=\"CoarseCloudPres_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_125
			OBJECT=DataField_126
				DataFieldName=\"CoarseCloudPres_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_126
			OBJECT=DataField_127
				DataFieldName=\"CoarseCloudPres_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_127
			OBJECT=DataField_128
				DataFieldName=\"CoarseCloudTemp_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_128
			OBJECT=DataField_129
				DataFieldName=\"CoarseCloudTemp_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_129
			OBJECT=DataField_130
				DataFieldName=\"CoarseCloudTemp_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_130
			OBJECT=DataField_131
				DataFieldName=\"CoarseCloudTemp_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_131
			OBJECT=DataField_132
				DataFieldName=\"CoarseCloudTemp_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"CoarseCloudLayer\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_132
			OBJECT=DataField_133
				DataFieldName=\"TotO3_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_133
			OBJECT=DataField_134
				DataFieldName=\"TotO3_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_134
			OBJECT=DataField_135
				DataFieldName=\"TotO3_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_135
			OBJECT=DataField_136
				DataFieldName=\"TotO3_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_136
			OBJECT=DataField_137
				DataFieldName=\"TotO3_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_137
			OBJECT=DataField_138
				DataFieldName=\"TotO3_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_138
			OBJECT=DataField_139
				DataFieldName=\"O3_VMR_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_139
			OBJECT=DataField_140
				DataFieldName=\"O3_VMR_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_140
			OBJECT=DataField_141
				DataFieldName=\"O3_VMR_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_141
			OBJECT=DataField_142
				DataFieldName=\"O3_VMR_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_142
			OBJECT=DataField_143
				DataFieldName=\"O3_VMR_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_143
			OBJECT=DataField_144
				DataFieldName=\"O3_VMR_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_144
			OBJECT=DataField_145
				DataFieldName=\"TotCO_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_145
			OBJECT=DataField_146
				DataFieldName=\"TotCO_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_146
			OBJECT=DataField_147
				DataFieldName=\"TotCO_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_147
			OBJECT=DataField_148
				DataFieldName=\"TotCO_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_148
			OBJECT=DataField_149
				DataFieldName=\"TotCO_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_149
			OBJECT=DataField_150
				DataFieldName=\"CO_VMR_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_150
			OBJECT=DataField_151
				DataFieldName=\"CO_VMR_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_151
			OBJECT=DataField_152
				DataFieldName=\"CO_VMR_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_152
			OBJECT=DataField_153
				DataFieldName=\"CO_VMR_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_153
			OBJECT=DataField_154
				DataFieldName=\"CO_VMR_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_154
			OBJECT=DataField_155
				DataFieldName=\"CO_VMR_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_155
			OBJECT=DataField_156
				DataFieldName=\"TotCH4_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_156
			OBJECT=DataField_157
				DataFieldName=\"TotCH4_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_157
			OBJECT=DataField_158
				DataFieldName=\"TotCH4_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_158
			OBJECT=DataField_159
				DataFieldName=\"TotCH4_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_159
			OBJECT=DataField_160
				DataFieldName=\"TotCH4_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_160
			OBJECT=DataField_161
				DataFieldName=\"CH4_VMR_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_161
			OBJECT=DataField_162
				DataFieldName=\"CH4_VMR_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_162
			OBJECT=DataField_163
				DataFieldName=\"CH4_VMR_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_163
			OBJECT=DataField_164
				DataFieldName=\"CH4_VMR_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_164
			OBJECT=DataField_165
				DataFieldName=\"CH4_VMR_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_165
			OBJECT=DataField_166
				DataFieldName=\"CH4_VMR_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_166
			OBJECT=DataField_167
				DataFieldName=\"OLR_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_167
			OBJECT=DataField_168
				DataFieldName=\"OLR_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_168
			OBJECT=DataField_169
				DataFieldName=\"OLR_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_169
			OBJECT=DataField_170
				DataFieldName=\"OLR_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_170
			OBJECT=DataField_171
				DataFieldName=\"OLR_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_171
			OBJECT=DataField_172
				DataFieldName=\"OLR_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_172
			OBJECT=DataField_173
				DataFieldName=\"ClrOLR_TqJ_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_173
			OBJECT=DataField_174
				DataFieldName=\"ClrOLR_TqJ_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_174
			OBJECT=DataField_175
				DataFieldName=\"ClrOLR_TqJ_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_175
			OBJECT=DataField_176
				DataFieldName=\"ClrOLR_TqJ_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_176
			OBJECT=DataField_177
				DataFieldName=\"ClrOLR_TqJ_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_177
			OBJECT=DataField_178
				DataFieldName=\"ClrOLR_TqJ_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_178
		END_GROUP=DataField
		GROUP=MergedFields
		END_GROUP=MergedFields
	END_GROUP=GRID_4
	GROUP=GRID_5
		GridName=\"ascending_MW_only\"
		XDim=360
		YDim=180
		UpperLeftPointMtrs=(-179030000.000000,89030000.000000)
		LowerRightMtrs=(180030000.000000,-90030000.000000)
		Projection=GCTP_GEO
		GROUP=Dimension
			OBJECT=Dimension_1
				DimensionName=\"StdPressureLev\"
				Size=24
			END_OBJECT=Dimension_1
			OBJECT=Dimension_2
				DimensionName=\"H2OPressureLev\"
				Size=12
			END_OBJECT=Dimension_2
			OBJECT=Dimension_3
				DimensionName=\"H2OPressureLay\"
				Size=12
			END_OBJECT=Dimension_3
			OBJECT=Dimension_4
				DimensionName=\"CoarseCloudLayer\"
				Size=3
			END_OBJECT=Dimension_4
			OBJECT=Dimension_5
				DimensionName=\"FineCloudLayer\"
				Size=12
			END_OBJECT=Dimension_5
			OBJECT=Dimension_6
				DimensionName=\"EmisFreqMW\"
				Size=3
			END_OBJECT=Dimension_6
		END_GROUP=Dimension
		GROUP=DataField
			OBJECT=DataField_1
				DataFieldName=\"TotalCounts_MW_A\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_1
			OBJECT=DataField_2
				DataFieldName=\"Emis_MW_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqMW\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_2
			OBJECT=DataField_3
				DataFieldName=\"Emis_MW_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"EmisFreqMW\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_3
			OBJECT=DataField_4
				DataFieldName=\"Emis_MW_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqMW\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_4
			OBJECT=DataField_5
				DataFieldName=\"Emis_MW_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqMW\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_5
			OBJECT=DataField_6
				DataFieldName=\"Emis_MW_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqMW\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_6
			OBJECT=DataField_7
				DataFieldName=\"Temperature_MW_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_7
			OBJECT=DataField_8
				DataFieldName=\"Temperature_MW_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_8
			OBJECT=DataField_9
				DataFieldName=\"Temperature_MW_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_9
			OBJECT=DataField_10
				DataFieldName=\"Temperature_MW_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_10
			OBJECT=DataField_11
				DataFieldName=\"Temperature_MW_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_11
			OBJECT=DataField_12
				DataFieldName=\"TotH2OVap_MW_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_12
			OBJECT=DataField_13
				DataFieldName=\"TotH2OVap_MW_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_13
			OBJECT=DataField_14
				DataFieldName=\"TotH2OVap_MW_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_14
			OBJECT=DataField_15
				DataFieldName=\"TotH2OVap_MW_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_15
			OBJECT=DataField_16
				DataFieldName=\"TotH2OVap_MW_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_16
			OBJECT=DataField_17
				DataFieldName=\"GPHeight_MW_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_17
			OBJECT=DataField_18
				DataFieldName=\"GPHeight_MW_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_18
			OBJECT=DataField_19
				DataFieldName=\"GPHeight_MW_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_19
			OBJECT=DataField_20
				DataFieldName=\"GPHeight_MW_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_20
			OBJECT=DataField_21
				DataFieldName=\"GPHeight_MW_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_21
			OBJECT=DataField_22
				DataFieldName=\"TotCldLiqH2O_MW_A\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_22
			OBJECT=DataField_23
				DataFieldName=\"TotCldLiqH2O_MW_A_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_23
			OBJECT=DataField_24
				DataFieldName=\"TotCldLiqH2O_MW_A_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_24
			OBJECT=DataField_25
				DataFieldName=\"TotCldLiqH2O_MW_A_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_25
			OBJECT=DataField_26
				DataFieldName=\"TotCldLiqH2O_MW_A_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_26
			OBJECT=DataField_27
				DataFieldName=\"TotCldLiqH2O_MW_A_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_27
		END_GROUP=DataField
		GROUP=MergedFields
		END_GROUP=MergedFields
	END_GROUP=GRID_5
	GROUP=GRID_6
		GridName=\"descending_MW_only\"
		XDim=360
		YDim=180
		UpperLeftPointMtrs=(-179030000.000000,89030000.000000)
		LowerRightMtrs=(180030000.000000,-90030000.000000)
		Projection=GCTP_GEO
		GROUP=Dimension
			OBJECT=Dimension_1
				DimensionName=\"StdPressureLev\"
				Size=24
			END_OBJECT=Dimension_1
			OBJECT=Dimension_2
				DimensionName=\"H2OPressureLev\"
				Size=12
			END_OBJECT=Dimension_2
			OBJECT=Dimension_3
				DimensionName=\"H2OPressureLay\"
				Size=12
			END_OBJECT=Dimension_3
			OBJECT=Dimension_4
				DimensionName=\"CoarseCloudLayer\"
				Size=3
			END_OBJECT=Dimension_4
			OBJECT=Dimension_5
				DimensionName=\"FineCloudLayer\"
				Size=12
			END_OBJECT=Dimension_5
			OBJECT=Dimension_6
				DimensionName=\"EmisFreqMW\"
				Size=3
			END_OBJECT=Dimension_6
		END_GROUP=Dimension
		GROUP=DataField
			OBJECT=DataField_1
				DataFieldName=\"TotalCounts_MW_D\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_1
			OBJECT=DataField_2
				DataFieldName=\"Emis_MW_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqMW\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_2
			OBJECT=DataField_3
				DataFieldName=\"Emis_MW_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"EmisFreqMW\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_3
			OBJECT=DataField_4
				DataFieldName=\"Emis_MW_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqMW\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_4
			OBJECT=DataField_5
				DataFieldName=\"Emis_MW_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqMW\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_5
			OBJECT=DataField_6
				DataFieldName=\"Emis_MW_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"EmisFreqMW\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_6
			OBJECT=DataField_7
				DataFieldName=\"Temperature_MW_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_7
			OBJECT=DataField_8
				DataFieldName=\"Temperature_MW_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_8
			OBJECT=DataField_9
				DataFieldName=\"Temperature_MW_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_9
			OBJECT=DataField_10
				DataFieldName=\"Tempe";
        String StructMetadata_5 "rature_MW_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_10
			OBJECT=DataField_11
				DataFieldName=\"Temperature_MW_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_11
			OBJECT=DataField_12
				DataFieldName=\"TotH2OVap_MW_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_12
			OBJECT=DataField_13
				DataFieldName=\"TotH2OVap_MW_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_13
			OBJECT=DataField_14
				DataFieldName=\"TotH2OVap_MW_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_14
			OBJECT=DataField_15
				DataFieldName=\"TotH2OVap_MW_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_15
			OBJECT=DataField_16
				DataFieldName=\"TotH2OVap_MW_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_16
			OBJECT=DataField_17
				DataFieldName=\"GPHeight_MW_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_17
			OBJECT=DataField_18
				DataFieldName=\"GPHeight_MW_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_18
			OBJECT=DataField_19
				DataFieldName=\"GPHeight_MW_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_19
			OBJECT=DataField_20
				DataFieldName=\"GPHeight_MW_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_20
			OBJECT=DataField_21
				DataFieldName=\"GPHeight_MW_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"StdPressureLev\",\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_21
			OBJECT=DataField_22
				DataFieldName=\"TotCldLiqH2O_MW_D\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_22
			OBJECT=DataField_23
				DataFieldName=\"TotCldLiqH2O_MW_D_ct\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_23
			OBJECT=DataField_24
				DataFieldName=\"TotCldLiqH2O_MW_D_sdev\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_24
			OBJECT=DataField_25
				DataFieldName=\"TotCldLiqH2O_MW_D_min\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_25
			OBJECT=DataField_26
				DataFieldName=\"TotCldLiqH2O_MW_D_max\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_26
			OBJECT=DataField_27
				DataFieldName=\"TotCldLiqH2O_MW_D_err\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_27
		END_GROUP=DataField
		GROUP=MergedFields
		END_GROUP=MergedFields
	END_GROUP=GRID_6
	GROUP=GRID_7
		GridName=\"location\"
		XDim=360
		YDim=180
		UpperLeftPointMtrs=(-179030000.000000,89030000.000000)
		LowerRightMtrs=(180030000.000000,-90030000.000000)
		Projection=GCTP_GEO
		GROUP=Dimension
		END_GROUP=Dimension
		GROUP=DataField
			OBJECT=DataField_1
				DataFieldName=\"Latitude\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_1
			OBJECT=DataField_2
				DataFieldName=\"Longitude\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_2
			OBJECT=DataField_3
				DataFieldName=\"LandSeaMask\"
				DataType=DFNT_INT16
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_3
			OBJECT=DataField_4
				DataFieldName=\"Topography\"
				DataType=DFNT_FLOAT32
				DimList=(\"YDim\",\"XDim\")
				CompressionType=HDFE_COMP_DEFLATE
				DeflateLevel=5
			END_OBJECT=DataField_4
		END_GROUP=DataField
		GROUP=MergedFields
		END_GROUP=MergedFields
	END_GROUP=GRID_7
END_GROUP=GridStructure
GROUP=PointStructure
END_GROUP=PointStructure
END
";
        String identifier_product_doi "10.5067/AQUA/AIRS/DATA303";
        String identifier_product_doi_authority "http://dx.doi.org/";
        String coremetadata "
GROUP                  = INVENTORYMETADATA
  GROUPTYPE            = MASTERGROUP

  GROUP                  = ECSDATAGRANULE

    OBJECT                 = LOCALGRANULEID
      NUM_VAL              = 1
      VALUE                = \"AIRS.2015.01.01.L3.RetStd_IR001.v6.0.11.0.G15013155825.hdf\"
    END_OBJECT             = LOCALGRANULEID

    OBJECT                 = PRODUCTIONDATETIME
      NUM_VAL              = 1
      VALUE                = \"2015-01-13T20:58:27.000Z\"
    END_OBJECT             = PRODUCTIONDATETIME

    OBJECT                 = LOCALVERSIONID
      NUM_VAL              = 1
      VALUE                = \"Unspecified\"
    END_OBJECT             = LOCALVERSIONID

  END_GROUP              = ECSDATAGRANULE

  GROUP                  = MEASUREDPARAMETER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"1\"

      GROUP                  = QAFLAGS
        CLASS                = \"1\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"1\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"1\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"1\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"1\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"1\"
        NUM_VAL              = 1
        VALUE                = \"Surface Skin Temperature\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"2\"

      GROUP                  = QAFLAGS
        CLASS                = \"2\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"2\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"2\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"2\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"2\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"2\"
        NUM_VAL              = 1
        VALUE                = \"Surface Air Temperature\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"3\"

      GROUP                  = QAFLAGS
        CLASS                = \"3\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"3\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"3\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"3\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"3\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"3\"
        NUM_VAL              = 1
        VALUE                = \"Atmospheric Temperature\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"4\"

      GROUP                  = QAFLAGS
        CLASS                = \"4\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"4\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"4\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"4\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"4\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"4\"
        NUM_VAL              = 1
        VALUE                = \"Water Vapor Mass Mixing Ratio\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"5\"

      GROUP                  = QAFLAGS
        CLASS                = \"5\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"5\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"5\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"5\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"5\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"5\"
        NUM_VAL              = 1
        VALUE                = \"Total Precipitable Water Vapor\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"6\"

      GROUP                  = QAFLAGS
        CLASS                = \"6\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"6\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"6\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"6\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"6\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"6\"
        NUM_VAL              = 1
        VALUE                = \"Ozone Volume Mixing Ratio\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"7\"

      GROUP                  = QAFLAGS
        CLASS                = \"7\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"7\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"7\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"7\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"7\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"7\"
        NUM_VAL              = 1
        VALUE                = \"Total Ozone Burden\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"8\"

      GROUP                  = QAFLAGS
        CLASS                = \"8\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"8\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"8\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"8\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"8\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"8\"
        NUM_VAL              = 1
        VALUE                = \"Spectral IR Surface Emissivities\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"9\"

      GROUP                  = QAFLAGS
        CLASS                = \"9\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"9\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"9\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"9\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"9\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"9\"
        NUM_VAL              = 1
        VALUE                = \"Spectral IR Surf Bidirect Reflectivity\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"10\"

      GROUP                  = QAFLAGS
        CLASS                = \"10\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"10\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"10\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"10\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"10\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"10\"
        NUM_VAL              = 1
        VALUE                = \"MW Surface Brightness\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"11\"

      GROUP                  = QAFLAGS
        CLASS                = \"11\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"11\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"11\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"11\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"11\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"11\"
        NUM_VAL              = 1
        VALUE                = \"MW Emissivity\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"12\"

      GROUP                  = QAFLAGS
        CLASS                = \"12\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"12\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"12\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"12\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"12\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"12\"
        NUM_VAL              = 1
        VALUE                = \"Total Cloud Water\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"13\"

      GROUP                  = QAFLAGS
        CLASS                = \"13\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"13\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"13\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"13\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"13\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"13\"
        NUM_VAL              = 1
        VALUE                = \"Cloud Top Temperature\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"14\"

      GROUP                  = QAFLAGS
        CLASS                = \"14\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"14\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"14\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"14\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"14\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"14\"
        NUM_VAL              = 1
        VALUE                = \"Cloud Top Pressure\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"15\"

      GROUP                  = QAFLAGS
        CLASS                = \"15\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"15\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"15\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"15\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"15\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"15\"
        NUM_VAL              = 1
        VALUE                = \"Effective Cloud Fraction\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"16\"

      GROUP                  = QAFLAGS
        CLASS                = \"16\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"16\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"16\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"16\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"16\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"16\"
        NUM_VAL              = 1
        VALUE                = \"Geopotential Heights\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

    OBJECT                 = MEASUREDPARAMETERCONTAINER
      CLASS                = \"17\"

      GROUP                  = QAFLAGS
        CLASS                = \"17\"

        OBJECT                 = AUTOMATICQUALITYFLAGEXPLANATION
          NUM_VAL              = 1
          CLASS                = \"17\"
          VALUE                = \"Based on percentage of product that is good. Suspect used where true quality is not known.\"
        END_OBJECT             = AUTOMATICQUALITYFLAGEXPLANATION

        OBJECT                 = AUTOMATICQUALITYFLAG
          NUM_VAL              = 1
          CLASS                = \"17\"
          VALUE                = \"Passed\"
        END_OBJECT             = AUTOMATICQUALITYFLAG

      END_GROUP              = QAFLAGS

      GROUP                  = QASTATS
        CLASS                = \"17\"

        OBJECT                 = QAPERCENTMISSINGDATA
          NUM_VAL              = 1
          CLASS                = \"17\"
          VALUE                = 0
        END_OBJECT             = QAPERCENTMISSINGDATA

      END_GROUP              = QASTATS

      OBJECT                 = PARAMETERNAME
        CLASS                = \"17\"
        NUM_VAL              = 1
        VALUE                = \"Geopotential Height of Surface\"
      END_OBJECT             = PARAMETERNAME

    END_OBJECT             = MEASUREDPARAMETERCONTAINER

  END_GROUP              = MEASUREDPARAMETER

  GROUP                  = COLLECTIONDESCRIPTIONCLASS

    OBJECT                 = VERSIONID
      NUM_VAL              = 1
      VALUE                = 6
    END_OBJECT             = VERSIONID

    OBJECT                 = SHORTNAME
      NUM_VAL              = 1
      VALUE                = \"AIRS3STD\"
    END_OBJECT             = SHORTNAME

  END_GROUP              = COLLECTIONDESCRIPTIONCLASS

  GROUP                  = INPUTGRANULE

    OBJECT                 = INPUTPOINTER
      NUM_VAL              = 650
      VALUE                = (\"L3h.land_sea_mask_1x1.v2.0.0.anc\", \"ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.055.L2.RetStd_IR.v6.0.11.0.G14365175636.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.056.L2.RetStd_IR.v6.0.11.0.G14365180609.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.057.L2.RetStd_IR.v6.0.11.0.G14365180401.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.058.L2.RetStd_IR.v6.0.11.0.G14365180553.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.059.L2.RetStd_IR.v6.0.11.0.G14365181436.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.060.L2.RetStd_IR.v6.0.11.0.G14365180531.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.061.L2.RetStd_IR.v6.0.11.0.G14365181233.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.062.L2.RetStd_IR.v6.0.11.0.G14365180832.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.063.L2.RetStd_IR.v6.0.11.0.G14365183039.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.064.L2.RetStd_IR.v6.0.11.0.G14365182829.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.065.L2.RetStd_IR.v6.0.11.0.G14365182053.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.066.L2.RetStd_IR.v6.0.11.0.G14365183514.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.067.L2.RetStd_IR.v6.0.11.0.G14365183828.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.068.L2.RetStd_IR.v6.0.11.0.G14365183507.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.069.L2.RetStd_IR.v6.0.11.0.G14365183535.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.070.L2.RetStd_IR.v6.0.11.0.G14365183316.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.071.L2.RetStd_IR.v6.0.11.0.G14365152846.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.072.L2.RetStd_IR.v6.0.11.0.G14365153202.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.073.L2.RetStd_IR.v6.0.11.0.G14365153118.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.074.L2.RetStd_IR.v6.0.11.0.G14365153119.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.075.L2.RetStd_IR.v6.0.11.0.G14365153426.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.076.L2.RetStd_IR.v6.0.11.0.G14365184042.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.077.L2.RetStd_IR.v6.0.11.0.G14365183557.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.078.L2.RetStd_IR.v6.0.11.0.G14365183532.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.079.L2.RetStd_IR.v6.0.11.0.G14365183104.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.080.L2.RetStd_IR.v6.0.11.0.G14365184608.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.081.L2.RetStd_IR.v6.0.11.0.G14365184504.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.082.L2.RetStd_IR.v6.0.11.0.G14365184050.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.083.L2.RetStd_IR.v6.0.11.0.G14365184154.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.084.L2.RetStd_IR.v6.0.11.0.G14365183901.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.085.L2.RetStd_IR.v6.0.11.0.G14365153038.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.086.L2.RetStd_IR.v6.0.11.0.G14365153448.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.087.L2.RetStd_IR.v6.0.11.0.G14365152807.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.088.L2.RetStd_IR.v6.0.11.0.G14365153349.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.089.L2.RetStd_IR.v6.0.11.0.G14365153047.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.090.L2.RetStd_IR.v6.0.11.0.G14365153612.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.091.L2.RetStd_IR.v6.0.11.0.G14365153108.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.092.L2.RetStd_IR.v6.0.11.0.G14365153812.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.093.L2.RetStd_IR.v6.0.11.0.G14365183855.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.094.L2.RetStd_IR.v6.0.11.0.G14365183812.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.095.L2.RetStd_IR.v6.0.11.0.G14365184633.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.096.L2.RetStd_IR.v6.0.11.0.G14365184444.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.097.L2.RetStd_IR.v6.0.11.0.G14365184648.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.098.L2.RetStd_IR.v6.0.11.0.G14365183704.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.099.L2.RetStd_IR.v6.0.11.0.G15001112252.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.100.L2.RetStd_IR.v6.0.11.0.G15001112337.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.101.L2.RetStd_IR.v6.0.11.0.G15001112454.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.102.L2.RetStd_IR.v6.0.11.0.G15001121450.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.103.L2.RetStd_IR.v6.0.11.0.G15001121555.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.104.L2.RetStd_IR.v6.0.11.0.G15001121126.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.105.L2.RetStd_IR.v6.0.11.0.G15001121847.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.106.L2.RetStd_IR.v6.0.11.0.G15001121719.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.107.L2.RetStd_IR.v6.0.11.0.G15001121742.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.108.L2.RetStd_IR.v6.0.11.0.G15001121931.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.109.L2.RetStd_IR.v6.0.11.0.G15001112317.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.110.L2.RetStd_IR.v6.0.11.0.G15001111928.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.111.L2.RetStd_IR.v6.0.11.0.G15001112201.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.112.L2.RetStd_IR.v6.0.11.0.G15001112555.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.113.L2.RetStd_IR.v6.0.11.0.G15001112533.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.114.L2.RetStd_IR.v6.0.11.0.G15001112324.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.115.L2.RetStd_IR.v6.0.11.0.G15001112456.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.116.L2.RetStd_IR.v6.0.11.0.G15001113125.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.117.L2.RetStd_IR.v6.0.11.0.G15001113307.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.118.L2.RetStd_IR.v6.0.11.0.G15001121744.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.119.L2.RetStd_IR.v6.0.11.0.G15001121523.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.120.L2.RetStd_IR.v6.0.11.0.G15001121809.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.121.L2.RetStd_IR.v6.0.11.0.G15001122742.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.122.L2.RetStd_IR.v6.0.11.0.G15001122658.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.123.L2.RetStd_IR.v6.0.11.0.G15001122647.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.124.L2.RetStd_IR.v6.0.11.0.G15001122758.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.125.L2.RetStd_IR.v6.0.11.0.G15001122916.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.126.L2.RetStd_IR.v6.0.11.0.G15001112919.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.127.L2.RetStd_IR.v6.0.11.0.G15001112540.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.128.L2.RetStd_IR.v6.0.11.0.G15001112352.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.129.L2.RetStd_IR.v6.0.11.0.G15001112411.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.130.L2.RetStd_IR.v6.0.11.0.G15001113230.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.131.L2.RetStd_IR.v6.0.11.0.G15001113119.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.132.L2.RetStd_IR.v6.0.11.0.G15001113007.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.133.L2.RetStd_IR.v6.0.11.0.G15001113225.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.134.L2.RetStd_IR.v6.0.11.0.G15001113808.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.135.L2.RetStd_IR.v6.0.11.0.G15001122341.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.136.L2.RetStd_IR.v6.0.11.0.G15001122617.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.137.L2.RetStd_IR.v6.0.11.0.G15001122757.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.138.L2.RetStd_IR.v6.0.11.0.G15001122806.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.139.L2.RetStd_IR.v6.0.11.0.G15001121953.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.140.L2.RetStd_IR.v6.0.11.0.G15001123930.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.141.L2.RetStd_IR.v6.0.11.0.G15001123125.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.142.L2.RetStd_IR.v6.0.11.0.G15001113748.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.143.L2.RetStd_IR.v6.0.11.0.G15001112737.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.144.L2.RetStd_IR.v6.0.11.0.G15001113003.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.145.L2.RetStd_IR.v6.0.11.0.G15001113230.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.146.L2.RetStd_IR.v6.0.11.0.G15001114106.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.147.L2.RetStd_IR.v6.0.11.0.G15001114058.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.148.L2.RetStd_IR.v6.0.11.0.G15001114011.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.149.L2.RetStd_IR.v6.0.11.0.G15001113240.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.150.L2.RetStd_IR.v6.0.11.0.G15001114027.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.151.L2.RetStd_IR.v6.0.11.0.G15001122827.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.152.L2.RetStd_IR.v6.0.11.0.G15001123538.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.153.L2.RetStd_IR.v6.0.11.0.G15001123916.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.154.L2.RetStd_IR.v6.0.11.0.G15001124009.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.155.L2.RetStd_IR.v6.0.11.0.G15001123025.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.156.L2.RetStd_IR.v6.0.11.0.G15001123730.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.157.L2.RetStd_IR.v6.0.11.0.G15001123737.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.158.L2.RetStd_IR.v6.0.11.0.G15001123910.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.159.L2.RetStd_IR.v6.0.11.0.G15001124227.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.160.L2.RetStd_IR.v6.0.11.0.G15001124401.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.161.L2.RetStd_IR.v6.0.11.0.G15001114845.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.162.L2.RetStd_IR.v6.0.11.0.G15001124228.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.163.L2.RetStd_IR.v6.0.11.0.G15001114844.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.164.L2.RetStd_IR.v6.0.11.0.G15001124857.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.165.L2.RetStd_IR.v6.0.11.0.G15001124952.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.166.L2.RetStd_IR.v6.0.11.0.G15001124925.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.167.L2.RetStd_IR.v6.0.11.0.G15001124554.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.168.L2.RetStd_IR.v6.0.11.0.G15001133331.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.169.L2.RetStd_IR.v6.0.11.0.G15001132820.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.170.L2.RetStd_IR.v6.0.11.0.G15001133254.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.171.L2.RetStd_IR.v6.0.11.0.G15001134121.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.172.L2.RetStd_IR.v6.0.11.0.G15001132820.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.173.L2.RetStd_IR.v6.0.11.0.G15001133547.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.174.L2.RetStd_IR.v6.0.11.0.G15001133537.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.175.L2.RetStd_IR.v6.0.11.0.G15001124547.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.176.L2.RetStd_IR.v6.0.11.0.G15001123747.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.177.L2.RetStd_IR.v6.0.11.0.G15001125030.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.178.L2.RetStd_IR.v6.0.11.0.G15001130611.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.179.L2.RetStd_IR.v6.0.11.0.G15001132817.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.180.L2.RetStd_IR.v6.0.11.0.G15001133118.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.181.L2.RetStd_IR.v6.0.11.0.G15001133309.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.182.L2.RetStd_IR.v6.0.11.0.G15001133107.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.183.L2.RetStd_IR.v6.0.11.0.G15001131946.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.184.L2.RetStd_IR.v6.0.11.0.G15001133155.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.185.L2.RetStd_IR.v6.0.11.0.G15001133220.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.186.L2.RetStd_IR.v6.0.11.0.G15001132310.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.187.L2.RetStd_IR.v6.0.11.0.G15001132626.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.188.L2.RetStd_IR.v6.0.11.0.G15001133340.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.189.L2.RetStd_IR.v6.0.11.0.G15001132921.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.190.L2.RetStd_IR.v6.0.11.0.G15001133754.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.191.L2.RetStd_IR.v6.0.11.0.G15001133910.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.192.L2.RetStd_IR.v6.0.11.0.G15001132252.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.193.L2.RetStd_IR.v6.0.11.0.G15001132853.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.194.L2.RetStd_IR.v6.0.11.0.G15001132309.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.195.L2.RetStd_IR.v6.0.11.0.G15001133230.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.196.L2.RetStd_IR.v6.0.11.0.G15001133152.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.197.L2.RetStd_IR.v6.0.11.0.G15001133131.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.198.L2.RetStd_IR.v6.0.11.0.G15001133019.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.199.L2.RetStd_IR.v6.0.11.0.G15001132942.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.200.L2.RetStd_IR.v6.0.11.0.G15001133106.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.201.L2.RetStd_IR.v6.0.11.0.G15001133439.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.202.L2.RetStd_IR.v6.0.11.0.G15001134303.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.203.L2.RetStd_IR.v6.0.11.0.G15005130154.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.207.L2.RetStd_IR.v6.0.11.0.G15005130151.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.208.L2.RetStd_IR.v6.0.11.0.G15001133403.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.209.L2.RetStd_IR.v6.0.11.0.G15001133154.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.210.L2.RetStd_IR.v6.0.11.0.G15001140847.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.211.L2.RetStd_IR.v6.0.11.0.G15001140918.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.212.L2.RetStd_IR.v6.0.11.0.G15001140048.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.213.L2.RetStd_IR.v6.0.11.0.G15001140836.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.214.L2.RetStd_IR.v6.0.11.0.G15001143257.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.215.L2.RetStd_IR.v6.0.11.0.G15001143509.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.216.L2.RetStd_IR.v6.0.11.0.G15001143224.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.217.L2.RetStd_IR.v6.0.11.0.G15001142507.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.218.L2.RetStd_IR.v6.0.11.0.G15001142356.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.219.L2.RetStd_IR.v6.0.11.0.G15001142449.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.220.L2.RetStd_IR.v6.0.11.0.G15001143342.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.221.L2.RetStd_IR.v6.0.11.0.G15001142649.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.222.L2.RetStd_IR.v6.0.11.0.G15001142507.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.223.L2.RetStd_IR.v6.0.11.0.G15001143551.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.224.L2.RetStd_IR.v6.0.11.0.G15001143436.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.225.L2.RetStd_IR.v6.0.11.0.G15001143407.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.226.L2.RetStd_IR.v6.0.11.0.G15001143334.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.227.L2.RetStd_IR.v6.0.11.0.G15001143724.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.228.L2.RetStd_IR.v6.0.11.0.G15001142825.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.229.L2.RetStd_IR.v6.0.11.0.G15001143906.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.230.L2.RetStd_IR.v6.0.11.0.G15001143334.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.231.L2.RetStd_IR.v6.0.11.0.G15001143856.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.232.L2.RetStd_IR.v6.0.11.0.G15001143931.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.233.L2.RetStd_IR.v6.0.11.0.G15001143734.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.234.L2.RetStd_IR.v6.0.11.0.G15001143742.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.235.L2.RetStd_IR.v6.0.11.0.G15001143709.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.236.L2.RetStd_IR.v6.0.11.0.G15001144722.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.237.L2.RetStd_IR.v6.0.11.0.G15001151739.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.238.L2.RetStd_IR.v6.0.11.0.G15001151841.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.239.L2.RetStd_IR.v6.0.11.0.G15001151414.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2014/365/AIRS.2014.12.31.240.L2.RetStd_IR.v6.0.11.0.G15001151429.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.001.L2.RetStd_IR.v6.0.11.0.G15001152042.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.002.L2.RetStd_IR.v6.0.11.0.G15001151208.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.003.L2.RetStd_IR.v6.0.11.0.G15001152330.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.004.L2.RetStd_IR.v6.0.11.0.G15001152513.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.005.L2.RetStd_IR.v6.0.11.0.G15001151614.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.006.L2.RetStd_IR.v6.0.11.0.G15001151846.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.007.L2.RetStd_IR.v6.0.11.0.G15001152452.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.008.L2.RetStd_IR.v6.0.11.0.G15001152727.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.009.L2.RetStd_IR.v6.0.11.0.G15001151627.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.010.L2.RetStd_IR.v6.0.11.0.G15001152200.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.011.L2.RetStd_IR.v6.0.11.0.G15001152521.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.012.L2.RetStd_IR.v6.0.11.0.G15001152452.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.013.L2.RetStd_IR.v6.0.11.0.G15001151700.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.014.L2.RetStd_IR.v6.0.11.0.G15001152535.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.015.L2.RetStd_IR.v6.0.11.0.G15001151943.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.016.L2.RetStd_IR.v6.0.11.0.G15001151926.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.017.L2.RetStd_IR.v6.0.11.0.G15001151956.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.018.L2.RetStd_IR.v6.0.11.0.G15001153329.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.019.L2.RetStd_IR.v6.0.11.0.G15001152849.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.020.L2.RetStd_IR.v6.0.11.0.G15001153854.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.021.L2.RetStd_IR.v6.0.11.0.G15001153942.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.022.L2.RetStd_IR.v6.0.11.0.G15001153854.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.023.L2.RetStd_IR.v6.0.11.0.G15001153138.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.024.L2.RetStd_IR.v6.0.11.0.G15001154050.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.025.L2.RetStd_IR.v6.0.11.0.G15001154036.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.026.L2.RetStd_IR.v6.0.11.0.G15001153845.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.027.L2.RetStd_IR.v6.0.11.0.G15001154028.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.028.L2.RetStd_IR.v6.0.11.0.G15001155105.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.029.L2.RetStd_IR.v6.0.11.0.G15001154239.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.030.L2.RetStd_IR.v6.0.11.0.G15001154402.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.031.L2.RetStd_IR.v6.0.11.0.G15001155222.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.032.L2.RetStd_IR.v6.0.11.0.G15001154526.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.033.L2.RetStd_IR.v6.0.11.0.G15001155447.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.034.L2.RetStd_IR.v6.0.11.0.G15001155127.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.035.L2.RetStd_IR.v6.0.11.0.G15001155102.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.036.L2.RetStd_IR.v6.0.11.0.G15001154633.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.037.L2.RetStd_IR.v6.0.11.0.G15001154716.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.038.L2.RetStd_IR.v6.0.11.0.G15001155536.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.039.L2.RetStd_IR.v6.0.11.0.G15001155703.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.040.L2.RetStd_IR.v6.0.11.0.G15001155837.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.041.L2.RetStd_IR.v6.0.11.0.G15001155847.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.042.L2.RetStd_IR.v6.0.11.0.G15001155605.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.043.L2.RetStd_IR.v6.0.11.0.G15001155602.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.044.L2.RetStd_IR.v6.0.11.0.G15001160024.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS";
        String coremetadata_1 "/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.045.L2.RetStd_IR.v6.0.11.0.G15001155356.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.046.L2.RetStd_IR.v6.0.11.0.G15001160150.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.047.L2.RetStd_IR.v6.0.11.0.G15001160025.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.048.L2.RetStd_IR.v6.0.11.0.G15001155255.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.049.L2.RetStd_IR.v6.0.11.0.G15001160227.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.050.L2.RetStd_IR.v6.0.11.0.G15001160259.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.051.L2.RetStd_IR.v6.0.11.0.G15001160059.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.052.L2.RetStd_IR.v6.0.11.0.G15001160023.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.053.L2.RetStd_IR.v6.0.11.0.G15001160420.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.054.L2.RetStd_IR.v6.0.11.0.G15001160527.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.055.L2.RetStd_IR.v6.0.11.0.G15001160542.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.056.L2.RetStd_IR.v6.0.11.0.G15001160541.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.057.L2.RetStd_IR.v6.0.11.0.G15001160420.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.058.L2.RetStd_IR.v6.0.11.0.G15001155707.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.059.L2.RetStd_IR.v6.0.11.0.G15001160304.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.060.L2.RetStd_IR.v6.0.11.0.G15001160458.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.061.L2.RetStd_IR.v6.0.11.0.G15001160000.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.062.L2.RetStd_IR.v6.0.11.0.G15001160948.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.063.L2.RetStd_IR.v6.0.11.0.G15001161300.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.064.L2.RetStd_IR.v6.0.11.0.G15001161047.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.065.L2.RetStd_IR.v6.0.11.0.G15001161344.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.066.L2.RetStd_IR.v6.0.11.0.G15001160521.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.067.L2.RetStd_IR.v6.0.11.0.G15001161238.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.068.L2.RetStd_IR.v6.0.11.0.G15001160928.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.069.L2.RetStd_IR.v6.0.11.0.G15001160457.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.070.L2.RetStd_IR.v6.0.11.0.G15001161641.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.071.L2.RetStd_IR.v6.0.11.0.G15001161626.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.072.L2.RetStd_IR.v6.0.11.0.G15001161630.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.073.L2.RetStd_IR.v6.0.11.0.G15001161503.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.074.L2.RetStd_IR.v6.0.11.0.G15001161447.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.075.L2.RetStd_IR.v6.0.11.0.G15001161546.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.076.L2.RetStd_IR.v6.0.11.0.G15001161611.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.077.L2.RetStd_IR.v6.0.11.0.G15001161508.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.078.L2.RetStd_IR.v6.0.11.0.G15001161622.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.079.L2.RetStd_IR.v6.0.11.0.G15001161850.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.080.L2.RetStd_IR.v6.0.11.0.G15001161330.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.081.L2.RetStd_IR.v6.0.11.0.G15001162050.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.082.L2.RetStd_IR.v6.0.11.0.G15001162126.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.083.L2.RetStd_IR.v6.0.11.0.G15001162127.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.084.L2.RetStd_IR.v6.0.11.0.G15001161314.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.085.L2.RetStd_IR.v6.0.11.0.G15001161203.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.086.L2.RetStd_IR.v6.0.11.0.G15001162413.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.087.L2.RetStd_IR.v6.0.11.0.G15001162629.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.088.L2.RetStd_IR.v6.0.11.0.G15001162549.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.089.L2.RetStd_IR.v6.0.11.0.G15001162651.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.090.L2.RetStd_IR.v6.0.11.0.G15001162543.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.091.L2.RetStd_IR.v6.0.11.0.G15001161835.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.092.L2.RetStd_IR.v6.0.11.0.G15001162614.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.093.L2.RetStd_IR.v6.0.11.0.G15001162343.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.094.L2.RetStd_IR.v6.0.11.0.G15001162459.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.095.L2.RetStd_IR.v6.0.11.0.G15001162051.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.096.L2.RetStd_IR.v6.0.11.0.G15001162935.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.097.L2.RetStd_IR.v6.0.11.0.G15001162309.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.098.L2.RetStd_IR.v6.0.11.0.G15001162341.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.099.L2.RetStd_IR.v6.0.11.0.G15002114912.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.100.L2.RetStd_IR.v6.0.11.0.G15002105152.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.101.L2.RetStd_IR.v6.0.11.0.G15002104255.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.102.L2.RetStd_IR.v6.0.11.0.G15002104550.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.103.L2.RetStd_IR.v6.0.11.0.G15002105611.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.104.L2.RetStd_IR.v6.0.11.0.G15002105410.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.105.L2.RetStd_IR.v6.0.11.0.G15002104610.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.106.L2.RetStd_IR.v6.0.11.0.G15002105220.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.107.L2.RetStd_IR.v6.0.11.0.G15002105231.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.108.L2.RetStd_IR.v6.0.11.0.G15002105515.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.109.L2.RetStd_IR.v6.0.11.0.G15002114710.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.110.L2.RetStd_IR.v6.0.11.0.G15002115324.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.111.L2.RetStd_IR.v6.0.11.0.G15002115907.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.112.L2.RetStd_IR.v6.0.11.0.G15002115706.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.113.L2.RetStd_IR.v6.0.11.0.G15002115556.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.114.L2.RetStd_IR.v6.0.11.0.G15002115858.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.115.L2.RetStd_IR.v6.0.11.0.G15002120011.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.116.L2.RetStd_IR.v6.0.11.0.G15002105441.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.117.L2.RetStd_IR.v6.0.11.0.G15002110109.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.118.L2.RetStd_IR.v6.0.11.0.G15002105852.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.119.L2.RetStd_IR.v6.0.11.0.G15002110600.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.120.L2.RetStd_IR.v6.0.11.0.G15002110501.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.121.L2.RetStd_IR.v6.0.11.0.G15002105542.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.122.L2.RetStd_IR.v6.0.11.0.G15002110458.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.123.L2.RetStd_IR.v6.0.11.0.G15002110543.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.124.L2.RetStd_IR.v6.0.11.0.G15002110615.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.125.L2.RetStd_IR.v6.0.11.0.G15002110349.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.126.L2.RetStd_IR.v6.0.11.0.G15002115301.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.127.L2.RetStd_IR.v6.0.11.0.G15002120040.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.128.L2.RetStd_IR.v6.0.11.0.G15002120238.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.129.L2.RetStd_IR.v6.0.11.0.G15002120005.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.130.L2.RetStd_IR.v6.0.11.0.G15002120016.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.131.L2.RetStd_IR.v6.0.11.0.G15002120103.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.132.L2.RetStd_IR.v6.0.11.0.G15002120220.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.133.L2.RetStd_IR.v6.0.11.0.G15002110236.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.134.L2.RetStd_IR.v6.0.11.0.G15002110423.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.135.L2.RetStd_IR.v6.0.11.0.G15002110949.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.136.L2.RetStd_IR.v6.0.11.0.G15002111218.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.137.L2.RetStd_IR.v6.0.11.0.G15002111332.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.138.L2.RetStd_IR.v6.0.11.0.G15002111109.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.139.L2.RetStd_IR.v6.0.11.0.G15002111040.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.140.L2.RetStd_IR.v6.0.11.0.G15002111233.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.141.L2.RetStd_IR.v6.0.11.0.G15002111107.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.142.L2.RetStd_IR.v6.0.11.0.G15002115753.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.143.L2.RetStd_IR.v6.0.11.0.G15002115906.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.144.L2.RetStd_IR.v6.0.11.0.G15002120300.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.145.L2.RetStd_IR.v6.0.11.0.G15002115429.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.146.L2.RetStd_IR.v6.0.11.0.G15002115359.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.147.L2.RetStd_IR.v6.0.11.0.G15002121009.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.148.L2.RetStd_IR.v6.0.11.0.G15002120243.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.149.L2.RetStd_IR.v6.0.11.0.G15002111104.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.150.L2.RetStd_IR.v6.0.11.0.G15002110415.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.151.L2.RetStd_IR.v6.0.11.0.G15002110924.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.152.L2.RetStd_IR.v6.0.11.0.G15002111152.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.153.L2.RetStd_IR.v6.0.11.0.G15002112133.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.154.L2.RetStd_IR.v6.0.11.0.G15002112231.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.155.L2.RetStd_IR.v6.0.11.0.G15002111130.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.156.L2.RetStd_IR.v6.0.11.0.G15002112116.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.157.L2.RetStd_IR.v6.0.11.0.G15002112328.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.158.L2.RetStd_IR.v6.0.11.0.G15002111705.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.159.L2.RetStd_IR.v6.0.11.0.G15002120640.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.160.L2.RetStd_IR.v6.0.11.0.G15002120855.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.161.L2.RetStd_IR.v6.0.11.0.G15002121010.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.162.L2.RetStd_IR.v6.0.11.0.G15002120211.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.163.L2.RetStd_IR.v6.0.11.0.G15002120936.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.164.L2.RetStd_IR.v6.0.11.0.G15002120840.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.165.L2.RetStd_IR.v6.0.11.0.G15002121126.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.166.L2.RetStd_IR.v6.0.11.0.G15002111557.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.167.L2.RetStd_IR.v6.0.11.0.G15002110800.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.168.L2.RetStd_IR.v6.0.11.0.G15002112142.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.169.L2.RetStd_IR.v6.0.11.0.G15002113003.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.170.L2.RetStd_IR.v6.0.11.0.G15002112915.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.171.L2.RetStd_IR.v6.0.11.0.G15002120326.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.172.L2.RetStd_IR.v6.0.11.0.G15002121142.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.173.L2.RetStd_IR.v6.0.11.0.G15002112118.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.174.L2.RetStd_IR.v6.0.11.0.G15002121844.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.175.L2.RetStd_IR.v6.0.11.0.G15002121805.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.176.L2.RetStd_IR.v6.0.11.0.G15002125833.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.177.L2.RetStd_IR.v6.0.11.0.G15002130357.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.178.L2.RetStd_IR.v6.0.11.0.G15002130857.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.179.L2.RetStd_IR.v6.0.11.0.G15002130247.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.180.L2.RetStd_IR.v6.0.11.0.G15002125953.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.181.L2.RetStd_IR.v6.0.11.0.G15002130101.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.182.L2.RetStd_IR.v6.0.11.0.G15002121331.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.183.L2.RetStd_IR.v6.0.11.0.G15002121114.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.184.L2.RetStd_IR.v6.0.11.0.G15002122254.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.185.L2.RetStd_IR.v6.0.11.0.G15002122522.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.186.L2.RetStd_IR.v6.0.11.0.G15002122513.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.187.L2.RetStd_IR.v6.0.11.0.G15002121621.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.188.L2.RetStd_IR.v6.0.11.0.G15002121708.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.189.L2.RetStd_IR.v6.0.11.0.G15002130015.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.190.L2.RetStd_IR.v6.0.11.0.G15002130829.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.191.L2.RetStd_IR.v6.0.11.0.G15002130608.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.192.L2.RetStd_IR.v6.0.11.0.G15002130236.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.193.L2.RetStd_IR.v6.0.11.0.G15002130513.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.194.L2.RetStd_IR.v6.0.11.0.G15002130424.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.195.L2.RetStd_IR.v6.0.11.0.G15002130610.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.196.L2.RetStd_IR.v6.0.11.0.G15002131235.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.197.L2.RetStd_IR.v6.0.11.0.G15002131203.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.198.L2.RetStd_IR.v6.0.11.0.G15002131251.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.199.L2.RetStd_IR.v6.0.11.0.G15002130219.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.200.L2.RetStd_IR.v6.0.11.0.G15002130826.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.201.L2.RetStd_IR.v6.0.11.0.G15002131246.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.202.L2.RetStd_IR.v6.0.11.0.G15002131340.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.203.L2.RetStd_IR.v6.0.11.0.G15002131321.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.204.L2.RetStd_IR.v6.0.11.0.G15002130923.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.205.L2.RetStd_IR.v6.0.11.0.G15002131031.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.206.L2.RetStd_IR.v6.0.11.0.G15002130742.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.207.L2.RetStd_IR.v6.0.11.0.G15002125940.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.208.L2.RetStd_IR.v6.0.11.0.G15002131036.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.209.L2.RetStd_IR.v6.0.11.0.G15002131302.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.210.L2.RetStd_IR.v6.0.11.0.G15002131406.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.211.L2.RetStd_IR.v6.0.11.0.G15002131056.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.212.L2.RetStd_IR.v6.0.11.0.G15002131934.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.213.L2.RetStd_IR.v6.0.11.0.G15002132104.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.214.L2.RetStd_IR.v6.0.11.0.G15002135805.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.215.L2.RetStd_IR.v6.0.11.0.G15002135608.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.216.L2.RetStd_IR.v6.0.11.0.G15002140045.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.217.L2.RetStd_IR.v6.0.11.0.G15002135803.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.218.L2.RetStd_IR.v6.0.11.0.G15002140704.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.219.L2.RetStd_IR.v6.0.11.0.G15002135319.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.220.L2.RetStd_IR.v6.0.11.0.G15002140611.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.221.L2.RetStd_IR.v6.0.11.0.G15002135907.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.222.L2.RetStd_IR.v6.0.11.0.G15002141019.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.223.L2.RetStd_IR.v6.0.11.0.G15002140425.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.224.L2.RetStd_IR.v6.0.11.0.G15002140057.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.225.L2.RetStd_IR.v6.0.11.0.G15002140356.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.226.L2.RetStd_IR.v6.0.11.0.G15002140509.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.227.L2.RetStd_IR.v6.0.11.0.G15002135626.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.228.L2.RetStd_IR.v6.0.11.0.G15002140444.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.229.L2.RetStd_IR.v6.0.11.0.G15002140509.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.230.L2.RetStd_IR.v6.0.11.0.G15002140723.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.231.L2.RetStd_IR.v6.0.11.0.G15002140928.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.232.L2.RetStd_IR.v6.0.11.0.G15002140326.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.233.L2.RetStd_IR.v6.0.11.0.G15002140608.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.234.L2.RetStd_IR.v6.0.11.0.G15002141022.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.235.L2.RetStd_IR.v6.0.11.0.G15002142102.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.236.L2.RetStd_IR.v6.0.11.0.G15002142056.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.237.L2.RetStd_IR.v6.0.11.0.G15002142838.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.238.L2.RetStd_IR.v6.0.11.0.G15002143740.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.239.L2.RetStd_IR.v6.0.11.0.G15002143808.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/001/AIRS.2015.01.01.240.L2.RetStd_IR.v6.0.11.0.G15002143506.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.001.L2.RetStd_IR.v6.0.11.0.G15002144502.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.002.L2.RetStd_IR.v6.0.11.0.G15002143656.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.003.L2.RetStd_IR.v6.0.11.0.G15002145440.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.004.L2.RetStd_IR.v6.0.11.0.G15002145435.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.005.L2.RetStd_IR.v6.0.11.0.G15002144541.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.006.L2.RetStd_IR.v6.0.11.0.G15002145629.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.007.L2.RetStd_IR.v6.0.11.0.G15002150004.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.008.L2.RetStd_IR.v6.0.11.0.G15002145610.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.009.L2.RetStd_IR.v6.0.11.0.G15002145547.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.010.L2.RetStd_IR.v6.0.11.0.G15002145845.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.011.L2.RetStd_IR.v6.0.11.0.G15002150142.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.012.L2.RetStd_IR.v6.0.11.0.G15002145344.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.013.L2.RetStd_IR.v6.0.11.0.G15002150302.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.014.L2.RetStd_IR.v6.0.11.0.G15002150452.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.015.L2.RetStd_IR.v6.0.11.0.G15002150443.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.016.L2.RetStd_IR.v6.0.11.0.G15002145601.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.017.L2.RetStd_IR.v6.0.11.0.G15002145317.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.018.L2.RetStd_IR.v6.0.11.0.G15002150302.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.019.L2.RetStd_IR.v6.0.11.0.G15002150402.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.020.L2.RetStd_IR.v6.0.11.0.G15002150059.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.021.L2.RetStd_IR.v6.0.11.0.G15002145511.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.022.L2.RetStd_IR.v6.0.11.0.G15002150630.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.023.L2.RetStd_IR.v6.0.11.0.G15002150640.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.024.L2.RetStd_IR.v6.0.11.0.G15002145829.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.025.L2.RetStd_IR.v6.0.11.0.G15002150528.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.026.L2.RetStd_IR.v6.0.11.0.G15002150855.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.027.L2.RetStd_IR.v6.0.11.0.G15002151315.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.028.L2.RetStd_IR.v6.0.11.0.G15002151410.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.029.L2.RetStd_IR.v6.0.11.0.G15002151414.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.030.L2.RetStd_IR.v6.0.11.0.G15002151545.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.031.L2.RetStd_IR.v6.0.11.0.G15002151559.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.032.L2.RetStd_IR.v6.0.11.0.G15002150753.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.033.L2.RetStd_IR.v6.0.11.0.G15002151150.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.034.L2.RetStd_IR.v6.0.11.0.G15002151433.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.035.L2.RetStd_IR.v6.0.11.0.G15002150908.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.036.L2.RetStd_IR.v6.0.11.0.G15002151854.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.037.L2.RetStd_IR.v6.0.11.0.G15002152151.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.038.L2.RetStd_IR.v6.0.11.0.G15002152007.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.039.L2.RetStd_IR.v6.0.11.0.G15002151415.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.040.L2.RetStd_IR.v6.0.11.0.G15002152405.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.041.L2.RetStd_IR.v6.0.11.0.G15002152409.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.042.L2.RetStd_IR.v6.0.11.0.G15002151441.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.043.L2.RetStd_IR.v6.0.11.0.G15002152533.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.044.L2.RetStd_IR.v6.0.11.0.G15002152820.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.045.L2.RetStd_IR.v6.0.11.0.G15002152811.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.046.L2.RetStd_IR.v6.0.11.0.G15002152804.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.047.L2.RetStd_IR.v6.0.11.0.G15002152003.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.048.L2.RetStd_IR.v6.0.11.0.G15002152847.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.049.L2.RetStd_IR.v6.0.11.0.G15002152545.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.050.L2.RetStd_IR.v6.0.11.0.G15002152458.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.051.L2.RetStd_IR.v6.0.11.0.G15002152645.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.052.L2.RetStd_IR.v6.0.11.0.G15002152147.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.053.L2.RetStd_IR.v6.0.11.0.G15002153103.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.054.L2.RetStd_IR.v6.0.11.0.G15002153052.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.055.L2.RetStd_IR.v6.0.11.0.G15002152229.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.056.L2.RetStd_IR.v6.0.11.0.G15002152936.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.057.L2.RetStd_IR.v6.0.11.0.G15002152349.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.058.L2.RetStd_IR.v6.0.11.0.G15002152936.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.059.L2.RetStd_IR.v6.0.11.0.G15002152756.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.060.L2.RetStd_IR.v6.0.11.0.G15002152512.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.061.L2.RetStd_IR.v6.0.11.0.G15002152628.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.062.L2.RetStd_IR.v6.0.11.0.G15002153425.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.063.L2.RetStd_IR.v6.0.11.0.G15002153525.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.064.L2.RetStd_IR.v6.0.11.0.G15002153234.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.065.L2.RetStd_IR.v6.0.11.0.G15002153231.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.066.L2.RetStd_IR.v6.0.11.0.G15002153250.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.067.L2.RetStd_IR.v6.0.11.0.G15002153406.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.068.L2.RetStd_IR.v6.0.11.0.G15002153454.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.069.L2.RetStd_IR.v6.0.11.0.G15002153555.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.070.L2.RetStd_IR.v6.0.11.0.G15002153729.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.071.L2.RetStd_IR.v6.0.11.0.G15002153658.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.072.L2.RetStd_IR.v6.0.11.0.G15002153044.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.073.L2.RetStd_IR.v6.0.11.0.G15002154018.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.074.L2.RetStd_IR.v6.0.11.0.G15002153915.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.075.L2.RetStd_IR.v6.0.11.0.G15002153643.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.076.L2.RetStd_IR.v6.0.11.0.G15002153803.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.077.L2.RetStd_IR.v6.0.11.0.G15002153427.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.078.L2.RetStd_IR.v6.0.11.0.G15002153431.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.079.L2.RetStd_IR.v6.0.11.0.G15002154245.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.080.L2.RetStd_IR.v6.0.11.0.G15002154405.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.081.L2.RetStd_IR.v6.0.11.0.G15002154211.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.082.L2.RetStd_IR.v6.0.11.0.G15002154158.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.083.L2.RetStd_IR.v6.0.11.0.G15002154251.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.084.L2.RetStd_IR.v6.0.11.0.G15002154153.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.085.L2.RetStd_IR.v6.0.11.0.G15002154328.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.086.L2.RetStd_IR.v6.0.11.0.G15002154358.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.087.L2.RetStd_IR.v6.0.11.0.G15002154600.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.088.L2.RetStd_IR.v6.0.11.0.G15002154600.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.089.L2.RetStd_IR.v6.0.11.0.G15002154618.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.090.L2.RetStd_IR.v6.0.11.0.G15002153918.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.091.L2.RetStd_IR.v6.0.11.0.G15002154551.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.092.L2.RetStd_IR.v6.0.11.0.G15002153637.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.093.L2.RetStd_IR.v6.0.11.0.G15002154916.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.094.L2.RetStd_IR.v6.0.11.0.G15002154249.hdf\", \"
          ftp://ac15.gesdisc.eosdis.nasa.gov/ftp/data/s4pa/.X9mnTjTGY0BkHTnS/Aqua_AIRS_Level2/AIRS2RET.006/2015/002/AIRS.2015.01.02.095.L2.RetStd_IR.v6.0.11.0.G15002155306.hdf\")
    END_OBJECT             = INPUTPOINTER

  END_GROUP              = INPUTGRANULE

  GROUP                  = SPATIALDOMAINCONTAINER

    GROUP                  = HORIZONTALSPATIALDOMAINCONTAINER

      GROUP                  = BOUNDINGRECTANGLE

        OBJECT                 = EASTBOUNDINGCOORDINATE
          NUM_VAL              = 1
          VALUE                = 180.0
        END_OBJECT             = EASTBOUNDINGCOORDINATE

        OBJECT                 = WESTBOUNDINGCOORDINATE
          NUM_VAL              = 1
          VALUE                = -180.0
        END_OBJECT             = WESTBOUNDINGCOORDINATE

        OBJECT                 = SOUTHBOUNDINGCOORDINATE
          NUM_VAL              = 1
          VALUE                = -90.0
        END_OBJECT             = SOUTHBOUNDINGCOORDINATE

        OBJECT                 = NORTHBOUNDINGCOORDINATE
          NUM_VAL              = 1
          VALUE                = 90.0
        END_OBJECT             = NORTHBOUNDINGCOORDINATE

      END_GROUP              = BOUNDINGRECTANGLE

    END_GROUP              = HORIZONTALSPATIALDOMAINCONTAINER

    GROUP                  = GRANULELOCALITY

      OBJECT                 = LOCALITYVALUE
        NUM_VAL              = 1
        VALUE                = \"Global\"
      END_OBJECT             = LOCALITYVALUE

    END_GROUP              = GRANULELOCALITY

  END_GROUP              = SPATIALDOMAINCONTAINER

  GROUP                  = RANGEDATETIME

    OBJECT                 = RANGEENDINGDATE
      NUM_VAL              = 1
      VALUE                = \"2015-01-02\"
    END_OBJECT             = RANGEENDINGDATE

    OBJECT                 = RANGEENDINGTIME
      NUM_VAL              = 1
      VALUE                = \"00:00:00.000000Z\"
    END_OBJECT             = RANGEENDINGTIME

    OBJECT                 = RANGEBEGINNINGDATE
      NUM_VAL              = 1
      VALUE                = \"2015-01-01\"
    END_OBJECT             = RANGEBEGINNINGDATE

    OBJECT                 = RANGEBEGINNINGTIME
      NUM_VAL              = 1
      VALUE                = \"00:00:00.000000Z\"
    END_OBJECT             = RANGEBEGINNINGTIME

  END_GROUP              = RANGEDATETIME

  GROUP                  = PGEVERSIONCLASS

    OBJECT                 = PGEVERSION
      NUM_VAL              = 1
      VALUE                = \"6.0.11.0\"
    END_OBJECT             = PGEVERSION

  END_GROUP              = PGEVERSIONCLASS

  GROUP                  = ASSOCIATEDPLATFORMINSTRUMENTSENSOR

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"1\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"1\"
        NUM_VAL              = 1
        VALUE                = \"HSB Channel 1\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"1\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"1\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"1\"
        NUM_VAL              = 1
        VALUE                = \"HSB\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"2\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"2\"
        NUM_VAL              = 1
        VALUE                = \"HSB Channel 2\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"2\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"2\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"2\"
        NUM_VAL              = 1
        VALUE                = \"HSB\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"3\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"3\"
        NUM_VAL              = 1
        VALUE                = \"HSB Channel 3\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"3\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"3\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"3\"
        NUM_VAL              = 1
        VALUE                = \"HSB\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"4\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"4\"
        NUM_VAL              = 1
        VALUE                = \"HSB Channel 4\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"4\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"4\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"4\"
        NUM_VAL              = 1
        VALUE                = \"HSB\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"5\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"5\"
        NUM_VAL              = 1
        VALUE                = \"HSB Channel 5\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"5\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"5\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"5\"
        NUM_VAL              = 1
        VALUE                = \"HSB\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"6\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"6\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 3\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"6\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"6\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"6\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"7\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"7\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 4\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"7\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"7\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"7\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"8\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"8\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 5\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"8\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"8\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"8\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"9\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"9\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 6\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"9\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"9\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"9\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"10\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"10\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 7\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"10\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"10\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"10\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CL";
        String coremetadata_2 "ASS                = \"11\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"11\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 8\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"11\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"11\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"11\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"12\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"12\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 9\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"12\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"12\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"12\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"13\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"13\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 10\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"13\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"13\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"13\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"14\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"14\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 11\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"14\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"14\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"14\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"15\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"15\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 12\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"15\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"15\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"15\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"16\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"16\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 13\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"16\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"16\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"16\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"17\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"17\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 14\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"17\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"17\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"17\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"18\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"18\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 15\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"18\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"18\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"18\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"19\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"19\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 1\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"19\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"19\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"19\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"20\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"20\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A Channel 2\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"20\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"20\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"20\"
        NUM_VAL              = 1
        VALUE                = \"AMSU-A\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"21\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"21\"
        NUM_VAL              = 1
        VALUE                = \"M1a Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"21\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"21\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"21\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"22\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"22\"
        NUM_VAL              = 1
        VALUE                = \"M1b Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"22\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"22\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"22\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"23\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"23\"
        NUM_VAL              = 1
        VALUE                = \"M2a Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"23\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"23\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"23\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"24\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"24\"
        NUM_VAL              = 1
        VALUE                = \"M2b Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"24\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"24\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"24\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"25\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"25\"
        NUM_VAL              = 1
        VALUE                = \"M3 Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"25\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"25\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"25\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"26\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"26\"
        NUM_VAL              = 1
        VALUE                = \"M4a Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"26\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"26\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"26\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"27\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"27\"
        NUM_VAL              = 1
        VALUE                = \"M4b Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"27\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"27\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"27\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"28\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"28\"
        NUM_VAL              = 1
        VALUE                = \"M4c Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"28\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"28\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"28\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"29\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"29\"
        NUM_VAL              = 1
        VALUE                = \"M4d Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"29\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"29\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"29\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"30\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"30\"
        NUM_VAL              = 1
        VALUE                = \"M5 Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"30\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"30\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"30\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"31\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"31\"
        NUM_VAL              = 1
        VALUE                = \"M6 Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"31\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"31\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"31\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"32\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"32\"
        NUM_VAL              = 1
        VALUE                = \"M7 Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"32\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"32\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"32\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"33\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"33\"
        NUM_VAL              = 1
        VALUE                = \"M8 Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"33\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"33\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"33\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"34\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"34\"
        NUM_VAL              = 1
        VALUE                = \"M9 Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"34\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"34\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"34\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"35\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"35\"
        NUM_VAL              = 1
        VALUE                = \"M10 Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"35\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"35\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"35\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"36\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"36\"
        NUM_VAL              = 1
        VALUE                = \"M11 Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"36\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"36\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"36\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"37\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"37\"
        NUM_VAL              = 1
        VALUE                = \"M12 Detector Array\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"37\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"37\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"37\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"38\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"38\"
        NUM_VAL              = 1
        VALUE                = \"VNIR Channel 1\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"38\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"38\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"38\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"39\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"39\"
        NUM_VAL              = 1
        VALUE                = \"VNIR Channel 2\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"39\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"39\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"39\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"40\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"40\"
        NUM_VAL              = 1
        VALUE                = \"VNIR Channel 3\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"40\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"40\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"40\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

    OBJECT                 = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER
      CLASS                = \"41\"

      OBJECT                 = ASSOCIATEDSENSORSHORTNAME
        CLASS                = \"41\"
        NUM_VAL              = 1
        VALUE                = \"VNIR Channel 4\"
      END_OBJECT             = ASSOCIATEDSENSORSHORTNAME

      OBJECT                 = ASSOCIATEDPLATFORMSHORTNAME
        CLASS                = \"41\"
        NUM_VAL              = 1
        VALUE                = \"Aqua\"
      END_OBJECT             = ASSOCIATEDPLATFORMSHORTNAME

      OBJECT                 = OPERATIONMODE
        CLASS                = \"41\"
        NUM_VAL              = 1
        VALUE                = \"Normal\"
      END_OBJECT             = OPERATIONMODE

      OBJECT                 = ASSOCIATEDINSTRUMENTSHORTNAME
        CLASS                = \"41\"
        NUM_VAL              = 1
        VALUE                = \"AIRS\"
      END_OBJECT             = ASSOCIATEDINSTRUMENTSHORTNAME

    END_OBJECT             = ASSOCIATEDPLATFORMINSTRUMENTSENSORCONTAINER

  END_GROUP              = ASSOCIATEDPLATFORMINSTRUMENTSENSOR

  GROUP                  = ADDITIONALATTRIBUTES

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"1\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"1\"
        NUM_VAL              = 1
        VALUE                = \"NumBadData\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"1\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"1\"
          VALUE                = \"4094\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"2\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"2\"
        NUM_VAL              = 1
        VALUE                = \"NumSpecialData\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"2\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"2\"
          VALUE                = \"0\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"3\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"3\"
        NUM_VAL              = 1
        VALUE                = \"NumProcessData\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"3\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"3\"
          VALUE                = \"1394500\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"4\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"4\"
        NUM_VAL              = 1
        VALUE                = \"NumMissingData\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"4\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"4\"
          VALUE                = \"6\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"5\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"5\"
        NUM_VAL              = 1
        VALUE                = \"NumTotalData\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"5\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"5\"
          VALUE                = \"1398600\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"6\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"6\"
        NUM_VAL              = 1
        VALUE                = \"NumFpe\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"6\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"6\"
          VALUE                = \"0\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"7\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"7\"
        NUM_VAL              = 1
        VALUE                = \"ProductGenerationFacility\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"7\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"7\"
          VALUE                = \"G\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"8\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"8\"
        NUM_VAL              = 1
        VALUE                = \"InputGranuleCount\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"8\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"8\"
          VALUE                = \"518\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"9\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"9\"
        NUM_VAL              = 1
        VALUE                = \"NumDaysPlanned\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"9\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"9\"
          VALUE                = \"1\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"10\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"10\"
        NUM_VAL              = 1
        VALUE                = \"NumDaysInCycle\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"10\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"10\"
          VALUE                = \"1\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"11\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"11\"
        NUM_VAL              = 1
        VALUE                = \"StartDayInAquaRepeat\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"11\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"11\"
          VALUE                = \"1\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"12\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"12\"
        NUM_VAL              = 1
        VALUE                = \"AIRSRunTag\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"12\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"12\"
          VALUE                = \"15013155825\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"13\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"13\"
        NUM_VAL              = 1
        VALUE                = \"ProductGenerationHostInformation\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"13\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"13\"
          VALUE                = \"ac13.gesdisc.eosdis.nasa.gov Linux 2.6.32-504.1.3.el6.x86_64 #1_SMP_Tue_Nov_11_17:57:25_UTC_2014 x86_64\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"14\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"14\"
        NUM_VAL              = 1
        VALUE                = \"identifier_product_doi\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"14\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"14\"
          VALUE                = \"10.5067/AQUA/AIRS/DATA303\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

    OBJECT                 = ADDITIONALATTRIBUTESCONTAINER
      CLASS                = \"15\"

      OBJECT                 = ADDITIONALATTRIBUTENAME
        CLASS                = \"15\"
        NUM_VAL              = 1
        VALUE                = \"identifier_product_doi_authority\"
      END_OBJECT             = ADDITIONALATTRIBUTENAME

      GROUP                  = INFORMATIONCONTENT
        CLASS                = \"15\"

        OBJECT                 = PARAMETERVALUE
          NUM_VAL              = 1
          CLASS                = \"15\"
          VALUE                = \"http://dx.doi.org/\"
        END_OBJECT             = PARAMETERVALUE

      END_GROUP              = INFORMATIONCONTENT

    END_OBJECT             = ADDITIONALATTRIBUTESCONTAINER

  END_GROUP              = ADDITIONALATTRIBUTES

END_GROUP              = INVENTORYMETADATA

END
";
    }
    ClrOLR_A {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_A";
        String fullnamepath "/ClrOLR_A";
    }
    GPHeight_MW_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_MW_A_sdev";
        String fullnamepath "/GPHeight_MW_A_sdev";
    }
    CO_VMR_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "CO_VMR_TqJ_D_ct";
        String fullnamepath "/CO_VMR_TqJ_D_ct";
    }
    RelHum_liquid_A_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_A_max";
        String fullnamepath "/RelHum_liquid_A_max";
    }
    Temperature_A_err {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_A_err";
        String fullnamepath "/Temperature_A_err";
    }
    TotH2OVap_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_TqJ_A";
        String fullnamepath "/TotH2OVap_TqJ_A";
    }
    TotCldLiqH2O_MW_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TotCldLiqH2O_MW_A_min";
        String fullnamepath "/TotCldLiqH2O_MW_A_min";
    }
    RelHum_A_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_A_min";
        String fullnamepath "/RelHum_A_min";
    }
    TropHeight_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "TropHeight_TqJ_D_ct";
        String fullnamepath "/TropHeight_TqJ_D_ct";
    }
    TropHeight_D_ct {
        Int16 _FillValue 0;
        String origname "TropHeight_D_ct";
        String fullnamepath "/TropHeight_D_ct";
    }
    O3_VMR_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_TqJ_D";
        String fullnamepath "/O3_VMR_TqJ_D";
    }
    SurfPres_Forecast_D_ct {
        Int16 _FillValue 0;
        String origname "SurfPres_Forecast_D_ct";
        String fullnamepath "/SurfPres_Forecast_D_ct";
    }
    CloudFrc_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_A_sdev";
        String fullnamepath "/CloudFrc_A_sdev";
    }
    RelHum_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_TqJ_D_max";
        String fullnamepath "/RelHum_TqJ_D_max";
    }
    CoarseCloudPres_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_TqJ_D_sdev";
        String fullnamepath "/CoarseCloudPres_TqJ_D_sdev";
    }
    CloudFrc_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_TqJ_D_max";
        String fullnamepath "/CloudFrc_TqJ_D_max";
    }
    RelHumSurf_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "RelHumSurf_TqJ_D_ct";
        String fullnamepath "/RelHumSurf_TqJ_D_ct";
    }
    O3_VMR_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_TqJ_A_max";
        String fullnamepath "/O3_VMR_TqJ_A_max";
    }
    CloudTopPres_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_TqJ_D_min";
        String fullnamepath "/CloudTopPres_TqJ_D_min";
    }
    Emis_MW_D {
        Float32 _FillValue -9999.000000;
        String origname "Emis_MW_D";
        String fullnamepath "/Emis_MW_D";
    }
    CloudTopPres_A {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_A";
        String fullnamepath "/CloudTopPres_A";
    }
    SurfSkinTemp_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_TqJ_D";
        String fullnamepath "/SurfSkinTemp_TqJ_D";
    }
    CloudFrc_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_TqJ_A_err";
        String fullnamepath "/CloudFrc_TqJ_A_err";
    }
    H2O_MMR_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_TqJ_D";
        String fullnamepath "/H2O_MMR_TqJ_D";
    }
    TotCO_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "TotCO_TqJ_A_ct";
        String fullnamepath "/TotCO_TqJ_A_ct";
    }
    TropPres_D_ct {
        Int16 _FillValue 0;
        String origname "TropPres_D_ct";
        String fullnamepath "/TropPres_D_ct";
    }
    FineCloudFrc_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_TqJ_A_sdev";
        String fullnamepath "/FineCloudFrc_TqJ_A_sdev";
    }
    TotCO_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_TqJ_D_max";
        String fullnamepath "/TotCO_TqJ_D_max";
    }
    ClrOLR_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_TqJ_A";
        String fullnamepath "/ClrOLR_TqJ_A";
    }
    TotCldLiqH2O_MW_A_err {
        Float32 _FillValue -9999.000000;
        String origname "TotCldLiqH2O_MW_A_err";
        String fullnamepath "/TotCldLiqH2O_MW_A_err";
    }
    H2O_MMR_Surf_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_TqJ_D_min";
        String fullnamepath "/H2O_MMR_Surf_TqJ_D_min";
    }
    Temperature_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "Temperature_TqJ_D_ct";
        String fullnamepath "/Temperature_TqJ_D_ct";
    }
    TotCH4_A_ct {
        Int16 _FillValue 0;
        String origname "TotCH4_A_ct";
        String fullnamepath "/TotCH4_A_ct";
    }
    H2O_MMR_Lyr_A_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_A_min";
        String fullnamepath "/H2O_MMR_Lyr_A_min";
    }
    CO_VMR_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_TqJ_D_sdev";
        String fullnamepath "/CO_VMR_TqJ_D_sdev";
    }
    H2O_MMR_Surf_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_TqJ_D_err";
        String fullnamepath "/H2O_MMR_Surf_TqJ_D_err";
    }
    Temperature_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_TqJ_D";
        String fullnamepath "/Temperature_TqJ_D";
    }
    CloudFrc_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_TqJ_D_err";
        String fullnamepath "/CloudFrc_TqJ_D_err";
    }
    RelHum_A_ct {
        Int16 _FillValue 0;
        String origname "RelHum_A_ct";
        String fullnamepath "/RelHum_A_ct";
    }
    RelHumSurf_liquid_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "RelHumSurf_liquid_TqJ_A_ct";
        String fullnamepath "/RelHumSurf_liquid_TqJ_A_ct";
    }
    H2O_MMR_Lyr_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_D_sdev";
        String fullnamepath "/H2O_MMR_Lyr_D_sdev";
    }
    EmisIR_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_A_sdev";
        String fullnamepath "/EmisIR_A_sdev";
    }
    TropTemp_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_A_sdev";
        String fullnamepath "/TropTemp_A_sdev";
    }
    TropPres_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_D_sdev";
        String fullnamepath "/TropPres_D_sdev";
    }
    OLR_D_err {
        Float32 _FillValue -9999.000000;
        String origname "OLR_D_err";
        String fullnamepath "/OLR_D_err";
    }
    O3_VMR_D {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_D";
        String fullnamepath "/O3_VMR_D";
    }
    H2O_MMR_D_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_D_ct";
        String fullnamepath "/H2O_MMR_D_ct";
    }
    GPHeight_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_TqJ_D";
        String fullnamepath "/GPHeight_TqJ_D";
    }
    SurfSkinTemp_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_TqJ_A_err";
        String fullnamepath "/SurfSkinTemp_TqJ_A_err";
    }
    TropTemp_D_ct {
        Int16 _FillValue 0;
        String origname "TropTemp_D_ct";
        String fullnamepath "/TropTemp_D_ct";
    }
    Temperature_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_TqJ_A_min";
        String fullnamepath "/Temperature_TqJ_A_min";
    }
    TotH2OVap_A_err {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_A_err";
        String fullnamepath "/TotH2OVap_A_err";
    }
    Temperature_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_TqJ_D_err";
        String fullnamepath "/Temperature_TqJ_D_err";
    }
    OLR_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "OLR_TqJ_D_min";
        String fullnamepath "/OLR_TqJ_D_min";
    }
    SurfSkinTemp_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_TqJ_D_max";
        String fullnamepath "/SurfSkinTemp_TqJ_D_max";
    }
    SurfSkinTemp_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_TqJ_A";
        String fullnamepath "/SurfSkinTemp_TqJ_A";
    }
    CloudTopPres_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_D_sdev";
        String fullnamepath "/CloudTopPres_D_sdev";
    }
    SurfPres_Forecast_A {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_A";
        String fullnamepath "/SurfPres_Forecast_A";
    }
    H2O_MMR_Lyr_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_TqJ_D_err";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_D_err";
    }
    CoarseCloudTemp_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_TqJ_A";
        String fullnamepath "/CoarseCloudTemp_TqJ_A";
    }
    CH4_VMR_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_TqJ_D_min";
        String fullnamepath "/CH4_VMR_TqJ_D_min";
    }
    RelHum_liquid_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "RelHum_liquid_TqJ_A_ct";
        String fullnamepath "/RelHum_liquid_TqJ_A_ct";
    }
    TropHeight_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_TqJ_A_min";
        String fullnamepath "/TropHeight_TqJ_A_min";
    }
    H2O_MMR_Lyr_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_Lyr_TqJ_A_ct";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_A_ct";
    }
    TotalCounts_TqJ_A {
        Int16 _FillValue 0;
        String origname "TotalCounts_TqJ_A";
        String fullnamepath "/TotalCounts_TqJ_A";
    }
    OLR_A_max {
        Float32 _FillValue -9999.000000;
        String origname "OLR_A_max";
        String fullnamepath "/OLR_A_max";
    }
    TropPres_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_TqJ_A_max";
        String fullnamepath "/TropPres_TqJ_A_max";
    }
    TotO3_D {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_D";
        String fullnamepath "/TotO3_D";
    }
    CoarseCloudTemp_D_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudTemp_D_ct";
        String fullnamepath "/CoarseCloudTemp_D_ct";
    }
    TropHeight_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_D_min";
        String fullnamepath "/TropHeight_D_min";
    }
    TotH2OVap_D_ct {
        Int16 _FillValue 0;
        String origname "TotH2OVap_D_ct";
        String fullnamepath "/TotH2OVap_D_ct";
    }
    FineCloudFrc_D_max {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_D_max";
        String fullnamepath "/FineCloudFrc_D_max";
    }
    O3_VMR_A_ct {
        Int16 _FillValue 0;
        String origname "O3_VMR_A_ct";
        String fullnamepath "/O3_VMR_A_ct";
    }
    GPHeight_MW_A_max {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_MW_A_max";
        String fullnamepath "/GPHeight_MW_A_max";
    }
    CO_VMR_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_TqJ_A_max";
        String fullnamepath "/CO_VMR_TqJ_A_max";
    }
    CloudTopPres_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "CloudTopPres_TqJ_A_ct";
        String fullnamepath "/CloudTopPres_TqJ_A_ct";
    }
    H2O_MMR_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_TqJ_A";
        String fullnamepath "/H2O_MMR_TqJ_A";
    }
    EmisIR_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_TqJ_A_sdev";
        String fullnamepath "/EmisIR_TqJ_A_sdev";
    }
    TotH2OVap_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_TqJ_D_err";
        String fullnamepath "/TotH2OVap_TqJ_D_err";
    }
    TotalCounts_A {
        Int16 _FillValue 0;
        String origname "TotalCounts_A";
        String fullnamepath "/TotalCounts_A";
    }
    O3_VMR_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_A_sdev";
        String fullnamepath "/O3_VMR_A_sdev";
    }
    GPHeight_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_TqJ_D_max";
        String fullnamepath "/GPHeight_TqJ_D_max";
    }
    O3_VMR_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_D_sdev";
        String fullnamepath "/O3_VMR_D_sdev";
    }
    CH4_VMR_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_TqJ_D_max";
        String fullnamepath "/CH4_VMR_TqJ_D_max";
    }
    CloudTopTemp_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_TqJ_A_sdev";
        String fullnamepath "/CloudTopTemp_TqJ_A_sdev";
    }
    TotH2OVap_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "TotH2OVap_TqJ_A_ct";
        String fullnamepath "/TotH2OVap_TqJ_A_ct";
    }
    CloudTopTemp_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_TqJ_D_min";
        String fullnamepath "/CloudTopTemp_TqJ_D_min";
    }
    TotCH4_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_D_min";
        String fullnamepath "/TotCH4_D_min";
    }
    CoarseCloudPres_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudPres_TqJ_D_ct";
        String fullnamepath "/CoarseCloudPres_TqJ_D_ct";
    }
    TotH2OVap_MW_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_MW_A_sdev";
        String fullnamepath "/TotH2OVap_MW_A_sdev";
    }
    CoarseCloudFrc_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_TqJ_D";
        String fullnamepath "/CoarseCloudFrc_TqJ_D";
    }
    CloudFrc_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_TqJ_D_min";
        String fullnamepath "/CloudFrc_TqJ_D_min";
    }
    RelHumSurf_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "RelHumSurf_TqJ_A_ct";
        String fullnamepath "/RelHumSurf_TqJ_A_ct";
    }
    CloudFrc_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "CloudFrc_TqJ_D_ct";
        String fullnamepath "/CloudFrc_TqJ_D_ct";
    }
    CoarseCloudTemp_D {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_D";
        String fullnamepath "/CoarseCloudTemp_D";
    }
    TropPres_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_A_min";
        String fullnamepath "/TropPres_A_min";
    }
    RelHum_liquid_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_TqJ_D_max";
        String fullnamepath "/RelHum_liquid_TqJ_D_max";
    }
    FineCloudFrc_A_min {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_A_min";
        String fullnamepath "/FineCloudFrc_A_min";
    }
    ClrOLR_D {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_D";
        String fullnamepath "/ClrOLR_D";
    }
    H2O_MMR_Surf_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_TqJ_A";
        String fullnamepath "/H2O_MMR_Surf_TqJ_A";
    }
    O3_VMR_A_min {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_A_min";
        String fullnamepath "/O3_VMR_A_min";
    }
    CloudTopTemp_D_ct {
        Int16 _FillValue 0;
        String origname "CloudTopTemp_D_ct";
        String fullnamepath "/CloudTopTemp_D_ct";
    }
    FineCloudFrc_A_max {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_A_max";
        String fullnamepath "/FineCloudFrc_A_max";
    }
    FineCloudFrc_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_A_sdev";
        String fullnamepath "/FineCloudFrc_A_sdev";
    }
    CloudTopTemp_A_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_A_err";
        String fullnamepath "/CloudTopTemp_A_err";
    }
    RelHumSurf_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_A_sdev";
        String fullnamepath "/RelHumSurf_A_sdev";
    }
    CloudFrc_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_TqJ_A";
        String fullnamepath "/CloudFrc_TqJ_A";
    }
    CO_VMR_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_A_min";
        String fullnamepath "/CO_VMR_A_min";
    }
    SurfAirTemp_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_TqJ_D_sdev";
        String fullnamepath "/SurfAirTemp_TqJ_D_sdev";
    }
    FineCloudFrc_D_ct {
        Int16 _FillValue 0;
        String origname "FineCloudFrc_D_ct";
        String fullnamepath "/FineCloudFrc_D_ct";
    }
    TotCH4_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_TqJ_D_min";
        String fullnamepath "/TotCH4_TqJ_D_min";
    }
    TropTemp_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_TqJ_A_max";
        String fullnamepath "/TropTemp_TqJ_A_max";
    }
    RelHumSurf_A_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_A_max";
        String fullnamepath "/RelHumSurf_A_max";
    }
    Temperature_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_TqJ_D_sdev";
        String fullnamepath "/Temperature_TqJ_D_sdev";
    }
    TotCH4_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "TotCH4_TqJ_D_ct";
        String fullnamepath "/TotCH4_TqJ_D_ct";
    }
    H2O_MMR_Surf_D_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_D_min";
        String fullnamepath "/H2O_MMR_Surf_D_min";
    }
    EmisIR_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_TqJ_D_err";
        String fullnamepath "/EmisIR_TqJ_D_err";
    }
    H2O_MMR_A {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_A";
        String fullnamepath "/H2O_MMR_A";
    }
    O3_VMR_D_min {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_D_min";
        String fullnamepath "/O3_VMR_D_min";
    }
    Temperature_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "Temperature_TqJ_A_ct";
        String fullnamepath "/Temperature_TqJ_A_ct";
    }
    CO_VMR_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_D_max";
        String fullnamepath "/CO_VMR_D_max";
    }
    TropTemp_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_TqJ_D_max";
        String fullnamepath "/TropTemp_TqJ_D_max";
    }
    CloudTopPres_D {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_D";
        String fullnamepath "/CloudTopPres_D";
    }
    TropPres_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "TropPres_TqJ_D_ct";
        String fullnamepath "/TropPres_TqJ_D_ct";
    }
    CloudTopPres_A_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_A_err";
        String fullnamepath "/CloudTopPres_A_err";
    }
    CloudTopPres_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_TqJ_A_err";
        String fullnamepath "/CloudTopPres_TqJ_A_err";
    }
    CloudTopPres_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_TqJ_A_min";
        String fullnamepath "/CloudTopPres_TqJ_A_min";
    }
    CloudTopPres_D_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_D_err";
        String fullnamepath "/CloudTopPres_D_err";
    }
    CloudTopPres_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_D_min";
        String fullnamepath "/CloudTopPres_D_min";
    }
    TotH2OVap_MW_A_ct {
        Int16 _FillValue 0;
        String origname "TotH2OVap_MW_A_ct";
        String fullnamepath "/TotH2OVap_MW_A_ct";
    }
    O3_VMR_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "O3_VMR_TqJ_A_ct";
        String fullnamepath "/O3_VMR_TqJ_A_ct";
    }
    TotO3_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_TqJ_D_err";
        String fullnamepath "/TotO3_TqJ_D_err";
    }
    H2O_MMR_Surf_D {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_D";
        String fullnamepath "/H2O_MMR_Surf_D";
    }
    TropTemp_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_D_min";
        String fullnamepath "/TropTemp_D_min";
    }
    CH4_VMR_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_TqJ_D";
        String fullnamepath "/CH4_VMR_TqJ_D";
    }
    RelHumSurf_liquid_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_TqJ_D_max";
        String fullnamepath "/RelHumSurf_liquid_TqJ_D_max";
    }
    Emis_MW_A_ct {
        Int16 _FillValue 0;
        String origname "Emis_MW_A_ct";
        String fullnamepath "/Emis_MW_A_ct";
    }
    EmisIR_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_TqJ_A_min";
        String fullnamepath "/EmisIR_TqJ_A_min";
    }
    CH4_VMR_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "CH4_VMR_TqJ_D_ct";
        String fullnamepath "/CH4_VMR_TqJ_D_ct";
    }
    TotO3_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_TqJ_D";
        String fullnamepath "/TotO3_TqJ_D";
    }
    RelHum_liquid_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_TqJ_D";
        String fullnamepath "/RelHum_liquid_TqJ_D";
    }
    TotCH4_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_TqJ_A_max";
        String fullnamepath "/TotCH4_TqJ_A_max";
    }
    GPHeight_A_max {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_A_max";
        String fullnamepath "/GPHeight_A_max";
    }
    O3_VMR_D_max {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_D_max";
        String fullnamepath "/O3_VMR_D_max";
    }
    SurfAirTemp_A_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_A_max";
        String fullnamepath "/SurfAirTemp_A_max";
    }
    Temperature_A_ct {
        Int16 _FillValue 0;
        String origname "Temperature_A_ct";
        String fullnamepath "/Temperature_A_ct";
    }
    H2O_MMR_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_TqJ_A_min";
        String fullnamepath "/H2O_MMR_TqJ_A_min";
    }
    SurfSkinTemp_D_ct {
        Int16 _FillValue 0;
        String origname "SurfSkinTemp_D_ct";
        String fullnamepath "/SurfSkinTemp_D_ct";
    }
    O3_VMR_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_TqJ_A_err";
        String fullnamepath "/O3_VMR_TqJ_A_err";
    }
    EmisIR_D_err {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_D_err";
        String fullnamepath "/EmisIR_D_err";
    }
    Temperature_A_max {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_A_max";
        String fullnamepath "/Temperature_A_max";
    }
    RelHumSurf_liquid_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_TqJ_D_min";
        String fullnamepath "/RelHumSurf_liquid_TqJ_D_min";
    }
    ClrOLR_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_TqJ_D_sdev";
        String fullnamepath "/ClrOLR_TqJ_D_sdev";
    }
    EmisIR_A_ct {
        Int16 _FillValue 0;
        String origname "EmisIR_A_ct";
        String fullnamepath "/EmisIR_A_ct";
    }
    OLR_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "OLR_TqJ_D";
        String fullnamepath "/OLR_TqJ_D";
    }
    Temperature_MW_A {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_MW_A";
        String fullnamepath "/Temperature_MW_A";
    }
    SurfAirTemp_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_TqJ_A_sdev";
        String fullnamepath "/SurfAirTemp_TqJ_A_sdev";
    }
    CloudTopPres_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_TqJ_D_sdev";
        String fullnamepath "/CloudTopPres_TqJ_D_sdev";
    }
    CloudTopTemp_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_TqJ_A_err";
        String fullnamepath "/CloudTopTemp_TqJ_A_err";
    }
    TotCH4_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_A_sdev";
        String fullnamepath "/TotCH4_A_sdev";
    }
    EmisIR_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_TqJ_D_sdev";
        String fullnamepath "/EmisIR_TqJ_D_sdev";
    }
    RelHumSurf_D_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_D_max";
        String fullnamepath "/RelHumSurf_D_max";
    }
    SurfAirTemp_D_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_D_min";
        String fullnamepath "/SurfAirTemp_D_min";
    }
    FineCloudFrc_A {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_A";
        String fullnamepath "/FineCloudFrc_A";
    }
    SurfAirTemp_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_TqJ_D_max";
        String fullnamepath "/SurfAirTemp_TqJ_D_max";
    }
    TropHeight_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_TqJ_D_min";
        String fullnamepath "/TropHeight_TqJ_D_min";
    }
    H2O_MMR_Lyr_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_TqJ_A_max";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_A_max";
    }
    SurfSkinTemp_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "SurfSkinTemp_TqJ_D_ct";
        String fullnamepath "/SurfSkinTemp_TqJ_D_ct";
    }
    SurfAirTemp_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_TqJ_A_err";
        String fullnamepath "/SurfAirTemp_TqJ_A_err";
    }
    ClrOLR_A_err {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_A_err";
        String fullnamepath "/ClrOLR_A_err";
    }
    TropTemp_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_D_sdev";
        String fullnamepath "/TropTemp_D_sdev";
    }
    CloudTopTemp_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_D_min";
        String fullnamepath "/CloudTopTemp_D_min";
    }
    CoarseCloudPres_D {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_D";
        String fullnamepath "/CoarseCloudPres_D";
    }
    TropPres_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_D_min";
        String fullnamepath "/TropPres_D_min";
    }
    TotCO_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_TqJ_A_sdev";
        String fullnamepath "/TotCO_TqJ_A_sdev";
    }
    H2O_MMR_Surf_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_A_sdev";
        String fullnamepath "/H2O_MMR_Surf_A_sdev";
    }
    TotH2OVap_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_A_sdev";
        String fullnamepath "/TotH2OVap_A_sdev";
    }
    TotCldLiqH2O_MW_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TotCldLiqH2O_MW_D_max";
        String fullnamepath "/TotCldLiqH2O_MW_D_max";
    }
    CH4_VMR_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_A_max";
        String fullnamepath "/CH4_VMR_A_max";
    }
    CH4_VMR_A {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_A";
        String fullnamepath "/CH4_VMR_A";
    }
    H2O_MMR_Surf_D_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_D_max";
        String fullnamepath "/H2O_MMR_Surf_D_max";
    }
    SurfSkinTemp_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_TqJ_D_sdev";
        String fullnamepath "/SurfSkinTemp_TqJ_D_sdev";
    }
    OLR_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "OLR_A_sdev";
        String fullnamepath "/OLR_A_sdev";
    }
    TotCO_D {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_D";
        String fullnamepath "/TotCO_D";
    }
    CoarseCloudFrc_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudFrc_TqJ_D_ct";
        String fullnamepath "/CoarseCloudFrc_TqJ_D_ct";
    }
    TotCH4_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_D_max";
        String fullnamepath "/TotCH4_D_max";
    }
    H2O_MMR_A_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_A_ct";
        String fullnamepath "/H2O_MMR_A_ct";
    }
    H2O_MMR_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_TqJ_A_ct";
        String fullnamepath "/H2O_MMR_TqJ_A_ct";
    }
    SurfSkinTemp_A_err {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_A_err";
        String fullnamepath "/SurfSkinTemp_A_err";
    }
    SurfPres_Forecast_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_TqJ_D_sdev";
        String fullnamepath "/SurfPres_Forecast_TqJ_D_sdev";
    }
    CO_VMR_A_err {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_A_err";
        String fullnamepath "/CO_VMR_A_err";
    }
    TotH2OVap_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_D_min";
        String fullnamepath "/TotH2OVap_D_min";
    }
    Temperature_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_TqJ_A";
        String fullnamepath "/Temperature_TqJ_A";
    }
    TotO3_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_TqJ_D_max";
        String fullnamepath "/TotO3_TqJ_D_max";
    }
    SurfAirTemp_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_TqJ_D_err";
        String fullnamepath "/SurfAirTemp_TqJ_D_err";
    }
    GPHeight_MW_D_min {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_MW_D_min";
        String fullnamepath "/GPHeight_MW_D_min";
    }
    CO_VMR_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_D_sdev";
        String fullnamepath "/CO_VMR_D_sdev";
    }
    RelHum_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_TqJ_A_max";
        String fullnamepath "/RelHum_TqJ_A_max";
    }
    RelHumSurf_liquid_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "RelHumSurf_liquid_TqJ_D_ct";
        String fullnamepath "/RelHumSurf_liquid_TqJ_D_ct";
    }
    H2O_MMR_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_TqJ_A_sdev";
        String fullnamepath "/H2O_MMR_TqJ_A_sdev";
    }
    CH4_VMR_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_TqJ_D_sdev";
        String fullnamepath "/CH4_VMR_TqJ_D_sdev";
    }
    O3_VMR_D_err {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_D_err";
        String fullnamepath "/O3_VMR_D_err";
    }
    TotO3_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_A_sdev";
        String fullnamepath "/TotO3_A_sdev";
    }
    OLR_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "OLR_TqJ_A_err";
        String fullnamepath "/OLR_TqJ_A_err";
    }
    CloudFrc_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_TqJ_D";
        String fullnamepath "/CloudFrc_TqJ_D";
    }
    SurfAirTemp_A_ct {
        Int16 _FillValue 0;
        String origname "SurfAirTemp_A_ct";
        String fullnamepath "/SurfAirTemp_A_ct";
    }
    CoarseCloudFrc_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_TqJ_A_sdev";
        String fullnamepath "/CoarseCloudFrc_TqJ_A_sdev";
    }
    RelHumSurf_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_TqJ_D";
        String fullnamepath "/RelHumSurf_TqJ_D";
    }
    TotCH4_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_TqJ_A_sdev";
        String fullnamepath "/TotCH4_TqJ_A_sdev";
    }
    CO_VMR_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_TqJ_A_sdev";
        String fullnamepath "/CO_VMR_TqJ_A_sdev";
    }
    H2O_MMR_Surf_A_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_A_err";
        String fullnamepath "/H2O_MMR_Surf_A_err";
    }
    RelHumSurf_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_TqJ_A_max";
        String fullnamepath "/RelHumSurf_TqJ_A_max";
    }
    TotCH4_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_D_sdev";
        String fullnamepath "/TotCH4_D_sdev";
    }
    TotH2OVap_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_TqJ_A_err";
        String fullnamepath "/TotH2OVap_TqJ_A_err";
    }
    GPHeight_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_TqJ_D_min";
        String fullnamepath "/GPHeight_TqJ_D_min";
    }
    SurfSkinTemp_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_A_sdev";
        String fullnamepath "/SurfSkinTemp_A_sdev";
    }
    CO_VMR_D {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_D";
        String fullnamepath "/CO_VMR_D";
    }
    CloudTopTemp_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_TqJ_D";
        String fullnamepath "/CloudTopTemp_TqJ_D";
    }
    TotO3_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_TqJ_A_sdev";
        String fullnamepath "/TotO3_TqJ_A_sdev";
    }
    CoarseCloudFrc_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_TqJ_D_min";
        String fullnamepath "/CoarseCloudFrc_TqJ_D_min";
    }
    TropPres_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_TqJ_D";
        String fullnamepath "/TropPres_TqJ_D";
    }
    RelHum_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "RelHum_TqJ_A_ct";
        String fullnamepath "/RelHum_TqJ_A_ct";
    }
    Temperature_MW_D_ct {
        Int16 _FillValue 0;
        String origname "Temperature_MW_D_ct";
        String fullnamepath "/Temperature_MW_D_ct";
    }
    Temperature_D_err {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_D_err";
        String fullnamepath "/Temperature_D_err";
    }
    TotH2OVap_D {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_D";
        String fullnamepath "/TotH2OVap_D";
    }
    H2O_MMR_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_D_sdev";
        String fullnamepath "/H2O_MMR_D_sdev";
    }
    CoarseCloudPres_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_TqJ_A_sdev";
        String fullnamepath "/CoarseCloudPres_TqJ_A_sdev";
    }
    CoarseCloudPres_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_A_min";
        String fullnamepath "/CoarseCloudPres_A_min";
    }
    EmisIR_D_max {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_D_max";
        String fullnamepath "/EmisIR_D_max";
    }
    TropPres_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_A_max";
        String fullnamepath "/TropPres_A_max";
    }
    Temperature_MW_D_min {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_MW_D_min";
        String fullnamepath "/Temperature_MW_D_min";
    }
    RelHum_D_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_D_max";
        String fullnamepath "/RelHum_D_max";
    }
    TotalCounts_MW_D {
        Int16 _FillValue 0;
        String origname "TotalCounts_MW_D";
        String fullnamepath "/TotalCounts_MW_D";
    }
    CH4_VMR_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "CH4_VMR_TqJ_A_ct";
        String fullnamepath "/CH4_VMR_TqJ_A_ct";
    }
    OLR_D_min {
        Float32 _FillValue -9999.000000;
        String origname "OLR_D_min";
        String fullnamepath "/OLR_D_min";
    }
    CoarseCloudPres_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_D_sdev";
        String fullnamepath "/CoarseCloudPres_D_sdev";
    }
    RelHum_liquid_D_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_D_max";
        String fullnamepath "/RelHum_liquid_D_max";
    }
    CH4_VMR_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_TqJ_A";
        String fullnamepath "/CH4_VMR_TqJ_A";
    }
    SurfAirTemp_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_A_sdev";
        String fullnamepath "/SurfAirTemp_A_sdev";
    }
    RelHumSurf_D_ct {
        Int16 _FillValue 0;
        String origname "RelHumSurf_D_ct";
        String fullnamepath "/RelHumSurf_D_ct";
    }
    CO_VMR_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_D_min";
        String fullnamepath "/CO_VMR_D_min";
    }
    TotO3_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_TqJ_A_min";
        String fullnamepath "/TotO3_TqJ_A_min";
    }
    RelHumSurf_liquid_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_D_sdev";
        String fullnamepath "/RelHumSurf_liquid_D_sdev";
    }
    SurfAirTemp_A_err {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_A_err";
        String fullnamepath "/SurfAirTemp_A_err";
    }
    CloudFrc_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_TqJ_A_max";
        String fullnamepath "/CloudFrc_TqJ_A_max";
    }
    OLR_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "OLR_TqJ_D_sdev";
        String fullnamepath "/OLR_TqJ_D_sdev";
    }
    OLR_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "OLR_D_sdev";
        String fullnamepath "/OLR_D_sdev";
    }
    SurfPres_Forecast_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_TqJ_A_sdev";
        String fullnamepath "/SurfPres_Forecast_TqJ_A_sdev";
    }
    TotH2OVap_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_A_min";
        String fullnamepath "/TotH2OVap_A_min";
    }
    TotH2OVap_MW_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_MW_D_min";
        String fullnamepath "/TotH2OVap_MW_D_min";
    }
    SurfAirTemp_D {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_D";
        String fullnamepath "/SurfAirTemp_D";
    }
    CoarseCloudFrc_A {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_A";
        String fullnamepath "/CoarseCloudFrc_A";
    }
    CoarseCloudPres_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_A_max";
        String fullnamepath "/CoarseCloudPres_A_max";
    }
    CloudTopPres_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "CloudTopPres_TqJ_D_ct";
        String fullnamepath "/CloudTopPres_TqJ_D_ct";
    }
    RelHum_liquid_D_ct {
        Int16 _FillValue 0;
        String origname "RelHum_liquid_D_ct";
        String fullnamepath "/RelHum_liquid_D_ct";
    }
    OLR_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "OLR_TqJ_D_max";
        String fullnamepath "/OLR_TqJ_D_max";
    }
    H2O_MMR_Surf_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_TqJ_A_sdev";
        String fullnamepath "/H2O_MMR_Surf_TqJ_A_sdev";
    }
    TotCldLiqH2O_MW_A {
        Float32 _FillValue -9999.000000;
        String origname "TotCldLiqH2O_MW_A";
        String fullnamepath "/TotCldLiqH2O_MW_A";
    }
    TotH2OVap_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_D_sdev";
        String fullnamepath "/TotH2OVap_D_sdev";
    }
    CloudTopTemp_A_ct {
        Int16 _FillValue 0;
        String origname "CloudTopTemp_A_ct";
        String fullnamepath "/CloudTopTemp_A_ct";
    }
    CO_VMR_A_ct {
        Int16 _FillValue 0;
        String origname "CO_VMR_A_ct";
        String fullnamepath "/CO_VMR_A_ct";
    }
    FineCloudFrc_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_TqJ_A_min";
        String fullnamepath "/FineCloudFrc_TqJ_A_min";
    }
    Temperature_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_TqJ_A_err";
        String fullnamepath "/Temperature_TqJ_A_err";
    }
    OLR_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "OLR_TqJ_A_min";
        String fullnamepath "/OLR_TqJ_A_min";
    }
    CH4_VMR_D_ct {
        Int16 _FillValue 0;
        String origname "CH4_VMR_D_ct";
        String fullnamepath "/CH4_VMR_D_ct";
    }
    RelHum_liquid_A {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_A";
        String fullnamepath "/RelHum_liquid_A";
    }
    O3_VMR_A_err {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_A_err";
        String fullnamepath "/O3_VMR_A_err";
    }
    TropTemp_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_TqJ_A_sdev";
        String fullnamepath "/TropTemp_TqJ_A_sdev";
    }
    ClrOLR_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_TqJ_A_min";
        String fullnamepath "/ClrOLR_TqJ_A_min";
    }
    CloudFrc_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_D_min";
        String fullnamepath "/CloudFrc_D_min";
    }
    TotH2OVap_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_TqJ_D_min";
        String fullnamepath "/TotH2OVap_TqJ_D_min";
    }
    CloudFrc_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_TqJ_D_sdev";
        String fullnamepath "/CloudFrc_TqJ_D_sdev";
    }
    CoarseCloudFrc_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_TqJ_A_min";
        String fullnamepath "/CoarseCloudFrc_TqJ_A_min";
    }
    FineCloudFrc_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "FineCloudFrc_TqJ_A_ct";
        String fullnamepath "/FineCloudFrc_TqJ_A_ct";
    }
    TotO3_A {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_A";
        String fullnamepath "/TotO3_A";
    }
    RelHum_liquid_D_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_D_min";
        String fullnamepath "/RelHum_liquid_D_min";
    }
    TropPres_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_A_sdev";
        String fullnamepath "/TropPres_A_sdev";
    }
    CoarseCloudPres_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_TqJ_D_min";
        String fullnamepath "/CoarseCloudPres_TqJ_D_min";
    }
    TotH2OVap_A_ct {
        Int16 _FillValue 0;
        String origname "TotH2OVap_A_ct";
        String fullnamepath "/TotH2OVap_A_ct";
    }
    TropTemp_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_TqJ_A";
        String fullnamepath "/TropTemp_TqJ_A";
    }
    RelHum_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_TqJ_A";
        String fullnamepath "/RelHum_TqJ_A";
    }
    RelHum_liquid_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_A_sdev";
        String fullnamepath "/RelHum_liquid_A_sdev";
    }
    GPHeight_A_ct {
        Int16 _FillValue 0;
        String origname "GPHeight_A_ct";
        String fullnamepath "/GPHeight_A_ct";
    }
    SurfPres_Forecast_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_TqJ_D";
        String fullnamepath "/SurfPres_Forecast_TqJ_D";
    }
    RelHum_liquid_A_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_A_min";
        String fullnamepath "/RelHum_liquid_A_min";
    }
    TropTemp_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_TqJ_D_sdev";
        String fullnamepath "/TropTemp_TqJ_D_sdev";
    }
    TotO3_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_TqJ_D_sdev";
        String fullnamepath "/TotO3_TqJ_D_sdev";
    }
    GPHeight_MW_A_ct {
        Int16 _FillValue 0;
        String origname "GPHeight_MW_A_ct";
        String fullnamepath "/GPHeight_MW_A_ct";
    }
    TropPres_A_ct {
        Int16 _FillValue 0;
        String origname "TropPres_A_ct";
        String fullnamepath "/TropPres_A_ct";
    }
    EmisIR_D_min {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_D_min";
        String fullnamepath "/EmisIR_D_min";
    }
    TotCH4_D_ct {
        Int16 _FillValue 0;
        String origname "TotCH4_D_ct";
        String fullnamepath "/TotCH4_D_ct";
    }
    SurfAirTemp_D_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_D_max";
        String fullnamepath "/SurfAirTemp_D_max";
    }
    CH4_VMR_A_err {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_A_err";
        String fullnamepath "/CH4_VMR_A_err";
    }
    TropHeight_A_ct {
        Int16 _FillValue 0;
        String origname "TropHeight_A_ct";
        String fullnamepath "/TropHeight_A_ct";
    }
    SurfSkinTemp_A_ct {
        Int16 _FillValue 0;
        String origname "SurfSkinTemp_A_ct";
        String fullnamepath "/SurfSkinTemp_A_ct";
    }
    CoarseCloudFrc_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_A_sdev";
        String fullnamepath "/CoarseCloudFrc_A_sdev";
    }
    CloudTopTemp_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_A_max";
        String fullnamepath "/CloudTopTemp_A_max";
    }
    FineCloudFrc_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_TqJ_D";
        String fullnamepath "/FineCloudFrc_TqJ_D";
    }
    H2O_MMR_A_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_A_max";
        String fullnamepath "/H2O_MMR_A_max";
    }
    ClrOLR_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_TqJ_D_min";
        String fullnamepath "/ClrOLR_TqJ_D_min";
    }
    TotO3_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_TqJ_A";
        String fullnamepath "/TotO3_TqJ_A";
    }
    H2O_MMR_Lyr_A_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_Lyr_A_ct";
        String fullnamepath "/H2O_MMR_Lyr_A_ct";
    }
    CloudTopTemp_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_TqJ_A";
        String fullnamepath "/CloudTopTemp_TqJ_A";
    }
    Emis_MW_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "Emis_MW_D_sdev";
        String fullnamepath "/Emis_MW_D_sdev";
    }
    RelHumSurf_liquid_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_TqJ_A_sdev";
        String fullnamepath "/RelHumSurf_liquid_TqJ_A_sdev";
    }
    CloudFrc_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "CloudFrc_TqJ_A_ct";
        String fullnamepath "/CloudFrc_TqJ_A_ct";
    }
    OLR_D_max {
        Float32 _FillValue -9999.000000;
        String origname "OLR_D_max";
        String fullnamepath "/OLR_D_max";
    }
    OLR_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "OLR_TqJ_D_ct";
        String fullnamepath "/OLR_TqJ_D_ct";
    }
    RelHum_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_TqJ_A_sdev";
        String fullnamepath "/RelHum_TqJ_A_sdev";
    }
    CO_VMR_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_TqJ_D_min";
        String fullnamepath "/CO_VMR_TqJ_D_min";
    }
    CloudTopPres_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_A_min";
        String fullnamepath "/CloudTopPres_A_min";
    }
    ClrOLR_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "ClrOLR_TqJ_A_ct";
        String fullnamepath "/ClrOLR_TqJ_A_ct";
    }
    TropPres_A {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_A";
        String fullnamepath "/TropPres_A";
    }
    TotalCounts_D {
        Int16 _FillValue 0;
        String origname "TotalCounts_D";
        String fullnamepath "/TotalCounts_D";
    }
    RelHumSurf_liquid_A_ct {
        Int16 _FillValue 0;
        String origname "RelHumSurf_liquid_A_ct";
        String fullnamepath "/RelHumSurf_liquid_A_ct";
    }
    H2O_MMR_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_TqJ_A_max";
        String fullnamepath "/H2O_MMR_TqJ_A_max";
    }
    TotCO_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_A_sdev";
        String fullnamepath "/TotCO_A_sdev";
    }
    ClrOLR_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_D_sdev";
        String fullnamepath "/ClrOLR_D_sdev";
    }
    Temperature_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_TqJ_A_sdev";
        String fullnamepath "/Temperature_TqJ_A_sdev";
    }
    H2O_MMR_D_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_D_max";
        String fullnamepath "/H2O_MMR_D_max";
    }
    TotCO_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_TqJ_D_sdev";
        String fullnamepath "/TotCO_TqJ_D_sdev";
    }
    SurfSkinTemp_A {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_A";
        String fullnamepath "/SurfSkinTemp_A";
    }
    SurfSkinTemp_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_D_sdev";
        String fullnamepath "/SurfSkinTemp_D_sdev";
    }
    H2O_MMR_Surf_A_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_Surf_A_ct";
        String fullnamepath "/H2O_MMR_Surf_A_ct";
    }
    TropHeight_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_D_max";
        String fullnamepath "/TropHeight_D_max";
    }
    CoarseCloudTemp_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_TqJ_A_min";
        String fullnamepath "/CoarseCloudTemp_TqJ_A_min";
    }
    TropTemp_A {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_A";
        String fullnamepath "/TropTemp_A";
    }
    TropHeight_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_A_sdev";
        String fullnamepath "/TropHeight_A_sdev";
    }
    TotCldLiqH2O_MW_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TotCldLiqH2O_MW_A_max";
        String fullnamepath "/TotCldLiqH2O_MW_A_max";
    }
    TotCH4_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_TqJ_D";
        String fullnamepath "/TotCH4_TqJ_D";
    }
    CloudFrc_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_A_min";
        String fullnamepath "/CloudFrc_A_min";
    }
    RelHum_A {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_A";
        String fullnamepath "/RelHum_A";
    }
    CloudTopTemp_D {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_D";
        String fullnamepath "/CloudTopTemp_D";
    }
    Temperature_D_max {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_D_max";
        String fullnamepath "/Temperature_D_max";
    }
    CloudTopPres_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_A_max";
        String fullnamepath "/CloudTopPres_A_max";
    }
    CoarseCloudTemp_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_A_min";
        String fullnamepath "/CoarseCloudTemp_A_min";
    }
    TropTemp_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "TropTemp_TqJ_D_ct";
        String fullnamepath "/TropTemp_TqJ_D_ct";
    }
    Temperature_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_TqJ_D_max";
        String fullnamepath "/Temperature_TqJ_D_max";
    }
    RelHumSurf_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_D_sdev";
        String fullnamepath "/RelHumSurf_D_sdev";
    }
    CloudFrc_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_TqJ_A_min";
        String fullnamepath "/CloudFrc_TqJ_A_min";
    }
    TotCldLiqH2O_MW_D_ct {
        Int16 _FillValue 0;
        String origname "TotCldLiqH2O_MW_D_ct";
        String fullnamepath "/TotCldLiqH2O_MW_D_ct";
    }
    H2O_MMR_Surf_A {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_A";
        String fullnamepath "/H2O_MMR_Surf_A";
    }
    TropPres_D {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_D";
        String fullnamepath "/TropPres_D";
    }
    CoarseCloudFrc_D {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_D";
        String fullnamepath "/CoarseCloudFrc_D";
    }
    CoarseCloudFrc_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudFrc_TqJ_A_ct";
        String fullnamepath "/CoarseCloudFrc_TqJ_A_ct";
    }
    CloudFrc_D_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_D_err";
        String fullnamepath "/CloudFrc_D_err";
    }
    O3_VMR_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_TqJ_D_err";
        String fullnamepath "/O3_VMR_TqJ_D_err";
    }
    SurfSkinTemp_D_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_D_min";
        String fullnamepath "/SurfSkinTemp_D_min";
    }
    TotCO_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_TqJ_A_min";
        String fullnamepath "/TotCO_TqJ_A_min";
    }
    H2O_MMR_Surf_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_TqJ_A_min";
        String fullnamepath "/H2O_MMR_Surf_TqJ_A_min";
    }
    TotCldLiqH2O_MW_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TotCldLiqH2O_MW_D_min";
        String fullnamepath "/TotCldLiqH2O_MW_D_min";
    }
    H2O_MMR_Lyr_D_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_D_min";
        String fullnamepath "/H2O_MMR_Lyr_D_min";
    }
    CoarseCloudFrc_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_D_sdev";
        String fullnamepath "/CoarseCloudFrc_D_sdev";
    }
    RelHum_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_TqJ_A_min";
        String fullnamepath "/RelHum_TqJ_A_min";
    }
    TotCO_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_A_min";
        String fullnamepath "/TotCO_A_min";
    }
    TotCO_D_ct {
        Int16 _FillValue 0;
        String origname "TotCO_D_ct";
        String fullnamepath "/TotCO_D_ct";
    }
    TropTemp_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "TropTemp_TqJ_A_ct";
        String fullnamepath "/TropTemp_TqJ_A_ct";
    }
    CH4_VMR_D {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_D";
        String fullnamepath "/CH4_VMR_D";
    }
    RelHum_liquid_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_TqJ_D_sdev";
        String fullnamepath "/RelHum_liquid_TqJ_D_sdev";
    }
    ClrOLR_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "ClrOLR_TqJ_D_ct";
        String fullnamepath "/ClrOLR_TqJ_D_ct";
    }
    CoarseCloudTemp_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudTemp_TqJ_A_ct";
        String fullnamepath "/CoarseCloudTemp_TqJ_A_ct";
    }
    EmisIR_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_TqJ_A";
        String fullnamepath "/EmisIR_TqJ_A";
    }
    Emis_MW_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "Emis_MW_A_sdev";
        String fullnamepath "/Emis_MW_A_sdev";
    }
    SurfSkinTemp_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_TqJ_A_min";
        String fullnamepath "/SurfSkinTemp_TqJ_A_min";
    }
    H2O_MMR_Surf_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_TqJ_D_sdev";
        String fullnamepath "/H2O_MMR_Surf_TqJ_D_sdev";
    }
    H2O_MMR_Lyr_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_Lyr_TqJ_D_ct";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_D_ct";
    }
    TotCH4_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_TqJ_D_sdev";
        String fullnamepath "/TotCH4_TqJ_D_sdev";
    }
    CloudFrc_A {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_A";
        String fullnamepath "/CloudFrc_A";
    }
    SurfPres_Forecast_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_TqJ_A_min";
        String fullnamepath "/SurfPres_Forecast_TqJ_A_min";
    }
    RelHumSurf_D {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_D";
        String fullnamepath "/RelHumSurf_D";
    }
    TotCO_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_D_max";
        String fullnamepath "/TotCO_D_max";
    }
    EmisIR_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_TqJ_A_max";
        String fullnamepath "/EmisIR_TqJ_A_max";
    }
    EmisIR_D_ct {
        Int16 _FillValue 0;
        String origname "EmisIR_D_ct";
        String fullnamepath "/EmisIR_D_ct";
    }
    RelHum_liquid_A_ct {
        Int16 _FillValue 0;
        String origname "RelHum_liquid_A_ct";
        String fullnamepath "/RelHum_liquid_A_ct";
    }
    GPHeight_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "GPHeight_TqJ_D_ct";
        String fullnamepath "/GPHeight_TqJ_D_ct";
    }
    TropHeight_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_A_min";
        String fullnamepath "/TropHeight_A_min";
    }
    TotH2OVap_MW_D {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_MW_D";
        String fullnamepath "/TotH2OVap_MW_D";
    }
    RelHumSurf_liquid_D_ct {
        Int16 _FillValue 0;
        String origname "RelHumSurf_liquid_D_ct";
        String fullnamepath "/RelHumSurf_liquid_D_ct";
    }
    CO_VMR_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "CO_VMR_TqJ_A_ct";
        String fullnamepath "/CO_VMR_TqJ_A_ct";
    }
    CoarseCloudFrc_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_TqJ_A_max";
        String fullnamepath "/CoarseCloudFrc_TqJ_A_max";
    }
    TotCH4_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_A_min";
        String fullnamepath "/TotCH4_A_min";
    }
    TropTemp_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_D_max";
        String fullnamepath "/TropTemp_D_max";
    }
    TotalCounts_TqJ_D {
        Int16 _FillValue 0;
        String origname "TotalCounts_TqJ_D";
        String fullnamepath "/TotalCounts_TqJ_D";
    }
    OLR_A_ct {
        Int16 _FillValue 0;
        String origname "OLR_A_ct";
        String fullnamepath "/OLR_A_ct";
    }
    CloudTopPres_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_TqJ_A_sdev";
        String fullnamepath "/CloudTopPres_TqJ_A_sdev";
    }
    TropPres_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_TqJ_D_min";
        String fullnamepath "/TropPres_TqJ_D_min";
    }
    H2O_MMR_Lyr_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_TqJ_D";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_D";
    }
    CoarseCloudTemp_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_A_max";
        String fullnamepath "/CoarseCloudTemp_A_max";
    }
    CoarseCloudPres_A_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudPres_A_ct";
        String fullnamepath "/CoarseCloudPres_A_ct";
    }
    RelHum_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_TqJ_D_min";
        String fullnamepath "/RelHum_TqJ_D_min";
    }
    Temperature_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_TqJ_D_min";
        String fullnamepath "/Temperature_TqJ_D_min";
    }
    CoarseCloudTemp_A_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudTemp_A_ct";
        String fullnamepath "/CoarseCloudTemp_A_ct";
    }
    ClrOLR_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_A_sdev";
        String fullnamepath "/ClrOLR_A_sdev";
    }
    TropTemp_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_TqJ_A_min";
        String fullnamepath "/TropTemp_TqJ_A_min";
    }
    TropHeight_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_TqJ_A";
        String fullnamepath "/TropHeight_TqJ_A";
    }
    SurfSkinTemp_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_TqJ_D_err";
        String fullnamepath "/SurfSkinTemp_TqJ_D_err";
    }
    O3_VMR_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_TqJ_D_max";
        String fullnamepath "/O3_VMR_TqJ_D_max";
    }
    ClrOLR_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_TqJ_A_err";
        String fullnamepath "/ClrOLR_TqJ_A_err";
    }
    CoarseCloudFrc_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_TqJ_A";
        String fullnamepath "/CoarseCloudFrc_TqJ_A";
    }
    SurfPres_Forecast_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "SurfPres_Forecast_TqJ_A_ct";
        String fullnamepath "/SurfPres_Forecast_TqJ_A_ct";
    }
    SurfSkinTemp_D_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_D_max";
        String fullnamepath "/SurfSkinTemp_D_max";
    }
    FineCloudFrc_D_min {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_D_min";
        String fullnamepath "/FineCloudFrc_D_min";
    }
    OLR_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "OLR_TqJ_A_ct";
        String fullnamepath "/OLR_TqJ_A_ct";
    }
    RelHum_liquid_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_TqJ_A_max";
        String fullnamepath "/RelHum_liquid_TqJ_A_max";
    }
    RelHum_D_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_D_min";
        String fullnamepath "/RelHum_D_min";
    }
    CoarseCloudPres_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_TqJ_A";
        String fullnamepath "/CoarseCloudPres_TqJ_A";
    }
    RelHum_D_ct {
        Int16 _FillValue 0;
        String origname "RelHum_D_ct";
        String fullnamepath "/RelHum_D_ct";
    }
    H2O_MMR_Lyr_D {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_D";
        String fullnamepath "/H2O_MMR_Lyr_D";
    }
    SurfAirTemp_D_ct {
        Int16 _FillValue 0;
        String origname "SurfAirTemp_D_ct";
        String fullnamepath "/SurfAirTemp_D_ct";
    }
    EmisIR_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_TqJ_D_max";
        String fullnamepath "/EmisIR_TqJ_D_max";
    }
    SurfAirTemp_A {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_A";
        String fullnamepath "/SurfAirTemp_A";
    }
    TotCldLiqH2O_MW_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotCldLiqH2O_MW_A_sdev";
        String fullnamepath "/TotCldLiqH2O_MW_A_sdev";
    }
    TotCO_A {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_A";
        String fullnamepath "/TotCO_A";
    }
    FineCloudFrc_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_TqJ_D_max";
        String fullnamepath "/FineCloudFrc_TqJ_D_max";
    }
    CloudFrc_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_TqJ_A_sdev";
        String fullnamepath "/CloudFrc_TqJ_A_sdev";
    }
    TotO3_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_D_sdev";
        String fullnamepath "/TotO3_D_sdev";
    }
    RelHum_A_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_A_max";
        String fullnamepath "/RelHum_A_max";
    }
    H2O_MMR_Surf_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_TqJ_A_err";
        String fullnamepath "/H2O_MMR_Surf_TqJ_A_err";
    }
    CloudTopPres_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_A_sdev";
        String fullnamepath "/CloudTopPres_A_sdev";
    }
    FineCloudFrc_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_TqJ_A";
        String fullnamepath "/FineCloudFrc_TqJ_A";
    }
    TotCO_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_A_max";
        String fullnamepath "/TotCO_A_max";
    }
    RelHum_liquid_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_TqJ_D_min";
        String fullnamepath "/RelHum_liquid_TqJ_D_min";
    }
    CloudTopTemp_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_D_sdev";
        String fullnamepath "/CloudTopTemp_D_sdev";
    }
    RelHumSurf_liquid_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_TqJ_A";
        String fullnamepath "/RelHumSurf_liquid_TqJ_A";
    }
    H2O_MMR_A_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_A_err";
        String fullnamepath "/H2O_MMR_A_err";
    }
    CoarseCloudTemp_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_TqJ_A_sdev";
        String fullnamepath "/CoarseCloudTemp_TqJ_A_sdev";
    }
    H2O_MMR_D {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_D";
        String fullnamepath "/H2O_MMR_D";
    }
    CloudFrc_A_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_A_err";
        String fullnamepath "/CloudFrc_A_err";
    }
    TotH2OVap_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_TqJ_A_sdev";
        String fullnamepath "/TotH2OVap_TqJ_A_sdev";
    }
    GPHeight_MW_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_MW_D_sdev";
        String fullnamepath "/GPHeight_MW_D_sdev";
    }
    CoarseCloudFrc_A_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudFrc_A_ct";
        String fullnamepath "/CoarseCloudFrc_A_ct";
    }
    CO_VMR_D_ct {
        Int16 _FillValue 0;
        String origname "CO_VMR_D_ct";
        String fullnamepath "/CO_VMR_D_ct";
    }
    H2O_MMR_Surf_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_TqJ_D";
        String fullnamepath "/H2O_MMR_Surf_TqJ_D";
    }
    OLR_A_min {
        Float32 _FillValue -9999.000000;
        String origname "OLR_A_min";
        String fullnamepath "/OLR_A_min";
    }
    CloudFrc_D {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_D";
        String fullnamepath "/CloudFrc_D";
    }
    FineCloudFrc_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_TqJ_D_sdev";
        String fullnamepath "/FineCloudFrc_TqJ_D_sdev";
    }
    TotCO_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "TotCO_TqJ_D_ct";
        String fullnamepath "/TotCO_TqJ_D_ct";
    }
    TotH2OVap_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_TqJ_A_max";
        String fullnamepath "/TotH2OVap_TqJ_A_max";
    }
    CoarseCloudPres_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_TqJ_A_max";
        String fullnamepath "/CoarseCloudPres_TqJ_A_max";
    }
    TotH2OVap_MW_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_MW_A_min";
        String fullnamepath "/TotH2OVap_MW_A_min";
    }
    H2O_MMR_D_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_D_min";
        String fullnamepath "/H2O_MMR_D_min";
    }
    EmisIR_A_err {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_A_err";
        String fullnamepath "/EmisIR_A_err";
    }
    GPHeight_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_TqJ_A_max";
        String fullnamepath "/GPHeight_TqJ_A_max";
    }
    RelHum_liquid_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_D_sdev";
        String fullnamepath "/RelHum_liquid_D_sdev";
    }
    O3_VMR_A {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_A";
        String fullnamepath "/O3_VMR_A";
    }
    TotH2OVap_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_TqJ_D";
        String fullnamepath "/TotH2OVap_TqJ_D";
    }
    CloudTopTemp_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_TqJ_A_max";
        String fullnamepath "/CloudTopTemp_TqJ_A_max";
    }
    RelHumSurf_A {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_A";
        String fullnamepath "/RelHumSurf_A";
    }
    OLR_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "OLR_TqJ_A";
        String fullnamepath "/OLR_TqJ_A";
    }
    GPHeight_A_min {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_A_min";
        String fullnamepath "/GPHeight_A_min";
    }
    SurfSkinTemp_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "SurfSkinTemp_TqJ_A_ct";
        String fullnamepath "/SurfSkinTemp_TqJ_A_ct";
    }
    H2O_MMR_Lyr_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_TqJ_D_max";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_D_max";
    }
    SurfAirTemp_A_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_A_min";
        String fullnamepath "/SurfAirTemp_A_min";
    }
    CoarseCloudFrc_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_D_min";
        String fullnamepath "/CoarseCloudFrc_D_min";
    }
    TotCH4_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "TotCH4_TqJ_A_ct";
        String fullnamepath "/TotCH4_TqJ_A_ct";
    }
    CoarseCloudTemp_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_A_sdev";
        String fullnamepath "/CoarseCloudTemp_A_sdev";
    }
    CloudTopPres_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_TqJ_A_max";
        String fullnamepath "/CloudTopPres_TqJ_A_max";
    }
    TotH2OVap_MW_D_ct {
        Int16 _FillValue 0;
        String origname "TotH2OVap_MW_D_ct";
        String fullnamepath "/TotH2OVap_MW_D_ct";
    }
    CO_VMR_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_A_max";
        String fullnamepath "/CO_VMR_A_max";
    }
    TropHeight_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_TqJ_A_max";
        String fullnamepath "/TropHeight_TqJ_A_max";
    }
    GPHeight_MW_D {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_MW_D";
        String fullnamepath "/GPHeight_MW_D";
    }
    GPHeight_D {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_D";
        String fullnamepath "/GPHeight_D";
    }
    Temperature_MW_D_max {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_MW_D_max";
        String fullnamepath "/Temperature_MW_D_max";
    }
    Temperature_D {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_D";
        String fullnamepath "/Temperature_D";
    }
    H2O_MMR_Surf_A_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_A_min";
        String fullnamepath "/H2O_MMR_Surf_A_min";
    }
    TropHeight_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_TqJ_A_sdev";
        String fullnamepath "/TropHeight_TqJ_A_sdev";
    }
    CloudTopTemp_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_TqJ_A_min";
        String fullnamepath "/CloudTopTemp_TqJ_A_min";
    }
    GPHeight_D_min {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_D_min";
        String fullnamepath "/GPHeight_D_min";
    }
    CloudTopTemp_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "CloudTopTemp_TqJ_A_ct";
        String fullnamepath "/CloudTopTemp_TqJ_A_ct";
    }
    GPHeight_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_TqJ_D_sdev";
        String fullnamepath "/GPHeight_TqJ_D_sdev";
    }
    TropHeight_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_TqJ_D_max";
        String fullnamepath "/TropHeight_TqJ_D_max";
    }
    ClrOLR_D_min {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_D_min";
        String fullnamepath "/ClrOLR_D_min";
    }
    CoarseCloudTemp_A {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_A";
        String fullnamepath "/CoarseCloudTemp_A";
    }
    H2O_MMR_D_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_D_err";
        String fullnamepath "/H2O_MMR_D_err";
    }
    CoarseCloudPres_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudPres_TqJ_A_ct";
        String fullnamepath "/CoarseCloudPres_TqJ_A_ct";
    }
    H2O_MMR_Lyr_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_A_sdev";
        String fullnamepath "/H2O_MMR_Lyr_A_sdev";
    }
    CH4_VMR_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_TqJ_A_max";
        String fullnamepath "/CH4_VMR_TqJ_A_max";
    }
    TropTemp_A_ct {
        Int16 _FillValue 0;
        String origname "TropTemp_A_ct";
        String fullnamepath "/TropTemp_A_ct";
    }
    H2O_MMR_Lyr_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_TqJ_D_min";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_D_min";
    }
    TropTemp_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_TqJ_D";
        String fullnamepath "/TropTemp_TqJ_D";
    }
    SurfPres_Forecast_A_ct {
        Int16 _FillValue 0;
        String origname "SurfPres_Forecast_A_ct";
        String fullnamepath "/SurfPres_Forecast_A_ct";
    }
    TropHeight_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_TqJ_D";
        String fullnamepath "/TropHeight_TqJ_D";
    }
    LandSeaMask {
        Int16 _FillValue 0;
        String origname "LandSeaMask";
        String fullnamepath "/LandSeaMask";
    }
    CloudFrc_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_A_max";
        String fullnamepath "/CloudFrc_A_max";
    }
    TropTemp_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_TqJ_D_min";
        String fullnamepath "/TropTemp_TqJ_D_min";
    }
    H2O_MMR_Surf_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_D_sdev";
        String fullnamepath "/H2O_MMR_Surf_D_sdev";
    }
    CloudTopPres_D_ct {
        Int16 _FillValue 0;
        String origname "CloudTopPres_D_ct";
        String fullnamepath "/CloudTopPres_D_ct";
    }
    RelHumSurf_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_TqJ_A_sdev";
        String fullnamepath "/RelHumSurf_TqJ_A_sdev";
    }
    CoarseCloudFrc_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_A_min";
        String fullnamepath "/CoarseCloudFrc_A_min";
    }
    GPHeight_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "GPHeight_TqJ_A_ct";
        String fullnamepath "/GPHeight_TqJ_A_ct";
    }
    GPHeight_MW_D_ct {
        Int16 _FillValue 0;
        String origname "GPHeight_MW_D_ct";
        String fullnamepath "/GPHeight_MW_D_ct";
    }
    TropTemp_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_A_min";
        String fullnamepath "/TropTemp_A_min";
    }
    CloudTopTemp_D_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_D_err";
        String fullnamepath "/CloudTopTemp_D_err";
    }
    CoarseCloudTemp_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudTemp_TqJ_D_ct";
        String fullnamepath "/CoarseCloudTemp_TqJ_D_ct";
    }
    Temperature_MW_D {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_MW_D";
        String fullnamepath "/Temperature_MW_D";
    }
    FineCloudFrc_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_D_sdev";
        String fullnamepath "/FineCloudFrc_D_sdev";
    }
    TotCO_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_D_min";
        String fullnamepath "/TotCO_D_min";
    }
    ClrOLR_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_TqJ_A_max";
        String fullnamepath "/ClrOLR_TqJ_A_max";
    }
    CoarseCloudPres_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_TqJ_D";
        String fullnamepath "/CoarseCloudPres_TqJ_D";
    }
    RelHumSurf_A_ct {
        Int16 _FillValue 0;
        String origname "RelHumSurf_A_ct";
        String fullnamepath "/RelHumSurf_A_ct";
    }
    CoarseCloudPres_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_A_sdev";
        String fullnamepath "/CoarseCloudPres_A_sdev";
    }
    CoarseCloudPres_D_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudPres_D_ct";
        String fullnamepath "/CoarseCloudPres_D_ct";
    }
    H2O_MMR_Surf_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_TqJ_D_max";
        String fullnamepath "/H2O_MMR_Surf_TqJ_D_max";
    }
    TropPres_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_TqJ_A";
        String fullnamepath "/TropPres_TqJ_A";
    }
    TotH2OVap_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_TqJ_D_sdev";
        String fullnamepath "/TotH2OVap_TqJ_D_sdev";
    }
    RelHumSurf_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_TqJ_D_min";
        String fullnamepath "/RelHumSurf_TqJ_D_min";
    }
    ClrOLR_D_err {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_D_err";
        String fullnamepath "/ClrOLR_D_err";
    }
    GPHeight_D_ct {
        Int16 _FillValue 0;
        String origname "GPHeight_D_ct";
        String fullnamepath "/GPHeight_D_ct";
    }
    CoarseCloudPres_A {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_A";
        String fullnamepath "/CoarseCloudPres_A";
    }
    H2O_MMR_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_TqJ_A_err";
        String fullnamepath "/H2O_MMR_TqJ_A_err";
    }
    EmisIR_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "EmisIR_TqJ_D_ct";
        String fullnamepath "/EmisIR_TqJ_D_ct";
    }
    TotCH4_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_A_max";
        String fullnamepath "/TotCH4_A_max";
    }
    Emis_MW_D_ct {
        Int16 _FillValue 0;
        String origname "Emis_MW_D_ct";
        String fullnamepath "/Emis_MW_D_ct";
    }
    H2O_MMR_Lyr_D_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_D_max";
        String fullnamepath "/H2O_MMR_Lyr_D_max";
    }
    CH4_VMR_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_TqJ_A_min";
        String fullnamepath "/CH4_VMR_TqJ_A_min";
    }
    TropHeight_D {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_D";
        String fullnamepath "/TropHeight_D";
    }
    TropPres_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_D_max";
        String fullnamepath "/TropPres_D_max";
    }
    Temperature_MW_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_MW_A_sdev";
        String fullnamepath "/Temperature_MW_A_sdev";
    }
    RelHum_D {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_D";
        String fullnamepath "/RelHum_D";
    }
    EmisIR_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_TqJ_D";
        String fullnamepath "/EmisIR_TqJ_D";
    }
    EmisIR_A {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_A";
        String fullnamepath "/EmisIR_A";
    }
    O3_VMR_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_TqJ_A_min";
        String fullnamepath "/O3_VMR_TqJ_A_min";
    }
    TotO3_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_A_max";
        String fullnamepath "/TotO3_A_max";
    }
    ClrOLR_D_max {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_D_max";
        String fullnamepath "/ClrOLR_D_max";
    }
    CO_VMR_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_A_sdev";
        String fullnamepath "/CO_VMR_A_sdev";
    }
    CoarseCloudFrc_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_A_max";
        String fullnamepath "/CoarseCloudFrc_A_max";
    }
    Emis_MW_A {
        Float32 _FillValue -9999.000000;
        String origname "Emis_MW_A";
        String fullnamepath "/Emis_MW_A";
    }
    RelHumSurf_liquid_D_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_D_max";
        String fullnamepath "/RelHumSurf_liquid_D_max";
    }
    GPHeight_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_TqJ_A";
        String fullnamepath "/GPHeight_TqJ_A";
    }
    OLR_A {
        Float32 _FillValue -9999.000000;
        String origname "OLR_A";
        String fullnamepath "/OLR_A";
    }
    TropPres_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_TqJ_A_sdev";
        String fullnamepath "/TropPres_TqJ_A_sdev";
    }
    TotalCounts_MW_A {
        Int16 _FillValue 0;
        String origname "TotalCounts_MW_A";
        String fullnamepath "/TotalCounts_MW_A";
    }
    CloudTopPres_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_TqJ_D";
        String fullnamepath "/CloudTopPres_TqJ_D";
    }
    RelHumSurf_liquid_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_TqJ_D";
        String fullnamepath "/RelHumSurf_liquid_TqJ_D";
    }
    TotCO_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_TqJ_D";
        String fullnamepath "/TotCO_TqJ_D";
    }
    TotCO_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_D_sdev";
        String fullnamepath "/TotCO_D_sdev";
    }
    EmisIR_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_D_sdev";
        String fullnamepath "/EmisIR_D_sdev";
    }
    H2O_MMR_Surf_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_TqJ_A_max";
        String fullnamepath "/H2O_MMR_Surf_TqJ_A_max";
    }
    CO_VMR_D_err {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_D_err";
        String fullnamepath "/CO_VMR_D_err";
    }
    H2O_MMR_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_TqJ_D_err";
        String fullnamepath "/H2O_MMR_TqJ_D_err";
    }
    SurfAirTemp_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_TqJ_A";
        String fullnamepath "/SurfAirTemp_TqJ_A";
    }
    ClrOLR_A_max {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_A_max";
        String fullnamepath "/ClrOLR_A_max";
    }
    RelHumSurf_liquid_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_TqJ_A_max";
        String fullnamepath "/RelHumSurf_liquid_TqJ_A_max";
    }
    CoarseCloudTemp_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_D_sdev";
        String fullnamepath "/CoarseCloudTemp_D_sdev";
    }
    GPHeight_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_TqJ_A_sdev";
        String fullnamepath "/GPHeight_TqJ_A_sdev";
    }
    H2O_MMR_Lyr_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_TqJ_D_sdev";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_D_sdev";
    }
    H2O_MMR_Surf_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_Surf_TqJ_D_ct";
        String fullnamepath "/H2O_MMR_Surf_TqJ_D_ct";
    }
    RelHumSurf_liquid_D {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_D";
        String fullnamepath "/RelHumSurf_liquid_D";
    }
    SurfAirTemp_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_D_sdev";
        String fullnamepath "/SurfAirTemp_D_sdev";
    }
    CloudTopPres_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_TqJ_A";
        String fullnamepath "/CloudTopPres_TqJ_A";
    }
    RelHumSurf_liquid_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_TqJ_A_min";
        String fullnamepath "/RelHumSurf_liquid_TqJ_A_min";
    }
    TotH2OVap_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_TqJ_A_min";
        String fullnamepath "/TotH2OVap_TqJ_A_min";
    }
    TotH2OVap_MW_A {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_MW_A";
        String fullnamepath "/TotH2OVap_MW_A";
    }
    SurfAirTemp_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_TqJ_D";
        String fullnamepath "/SurfAirTemp_TqJ_D";
    }
    RelHum_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "RelHum_TqJ_D_ct";
        String fullnamepath "/RelHum_TqJ_D_ct";
    }
    TotCO_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_TqJ_A";
        String fullnamepath "/TotCO_TqJ_A";
    }
    SurfSkinTemp_D {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_D";
        String fullnamepath "/SurfSkinTemp_D";
    }
    CoarseCloudPres_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_D_max";
        String fullnamepath "/CoarseCloudPres_D_max";
    }
    H2O_MMR_A_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_A_min";
        String fullnamepath "/H2O_MMR_A_min";
    }
    ClrOLR_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_TqJ_D";
        String fullnamepath "/ClrOLR_TqJ_D";
    }
    CoarseCloudFrc_D_ct {
        Int16 _FillValue 0;
        String origname "CoarseCloudFrc_D_ct";
        String fullnamepath "/CoarseCloudFrc_D_ct";
    }
    RelHum_liquid_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_TqJ_A";
        String fullnamepath "/RelHum_liquid_TqJ_A";
    }
    CH4_VMR_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_TqJ_A_err";
        String fullnamepath "/CH4_VMR_TqJ_A_err";
    }
    CoarseCloudTemp_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_D_max";
        String fullnamepath "/CoarseCloudTemp_D_max";
    }
    RelHumSurf_liquid_A_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_A_min";
        String fullnamepath "/RelHumSurf_liquid_A_min";
    }
    RelHumSurf_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_TqJ_A_min";
        String fullnamepath "/RelHumSurf_TqJ_A_min";
    }
    TotCO_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_TqJ_A_max";
        String fullnamepath "/TotCO_TqJ_A_max";
    }
    Temperature_MW_A_min {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_MW_A_min";
        String fullnamepath "/Temperature_MW_A_min";
    }
    RelHumSurf_liquid_D_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_D_min";
        String fullnamepath "/RelHumSurf_liquid_D_min";
    }
    Temperature_MW_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_MW_D_sdev";
        String fullnamepath "/Temperature_MW_D_sdev";
    }
    TotCH4_D {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_D";
        String fullnamepath "/TotCH4_D";
    }
    EmisIR_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_TqJ_D_min";
        String fullnamepath "/EmisIR_TqJ_D_min";
    }
    TotCH4_A {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_A";
        String fullnamepath "/TotCH4_A";
    }
    RelHum_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_TqJ_D";
        String fullnamepath "/RelHum_TqJ_D";
    }
    H2O_MMR_Lyr_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_TqJ_A_err";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_A_err";
    }
    SurfAirTemp_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_TqJ_A_min";
        String fullnamepath "/SurfAirTemp_TqJ_A_min";
    }
    CH4_VMR_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_A_min";
        String fullnamepath "/CH4_VMR_A_min";
    }
    TropTemp_D {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_D";
        String fullnamepath "/TropTemp_D";
    }
    CloudFrc_D_ct {
        Int16 _FillValue 0;
        String origname "CloudFrc_D_ct";
        String fullnamepath "/CloudFrc_D_ct";
    }
    GPHeight_MW_A_min {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_MW_A_min";
        String fullnamepath "/GPHeight_MW_A_min";
    }
    CO_VMR_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_TqJ_A";
        String fullnamepath "/CO_VMR_TqJ_A";
    }
    TotO3_D_ct {
        Int16 _FillValue 0;
        String origname "TotO3_D_ct";
        String fullnamepath "/TotO3_D_ct";
    }
    TotO3_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "TotO3_TqJ_A_ct";
        String fullnamepath "/TotO3_TqJ_A_ct";
    }
    Temperature_A {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_A";
        String fullnamepath "/Temperature_A";
    }
    GPHeight_MW_D_max {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_MW_D_max";
        String fullnamepath "/GPHeight_MW_D_max";
    }
    ClrOLR_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_TqJ_D_err";
        String fullnamepath "/ClrOLR_TqJ_D_err";
    }
    CloudTopTemp_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_TqJ_D_sdev";
        String fullnamepath "/CloudTopTemp_TqJ_D_sdev";
    }
    Temperature_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_D_sdev";
        String fullnamepath "/Temperature_D_sdev";
    }
    OLR_D_ct {
        Int16 _FillValue 0;
        String origname "OLR_D_ct";
        String fullnamepath "/OLR_D_ct";
    }
    Temperature_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_A_sdev";
        String fullnamepath "/Temperature_A_sdev";
    }
    SurfSkinTemp_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_TqJ_A_max";
        String fullnamepath "/SurfSkinTemp_TqJ_A_max";
    }
    TotH2OVap_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_TqJ_D_max";
        String fullnamepath "/TotH2OVap_TqJ_D_max";
    }
    ClrOLR_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_TqJ_D_max";
        String fullnamepath "/ClrOLR_TqJ_D_max";
    }
    RelHum_liquid_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "RelHum_liquid_TqJ_D_ct";
        String fullnamepath "/RelHum_liquid_TqJ_D_ct";
    }
    O3_VMR_D_ct {
        Int16 _FillValue 0;
        String origname "O3_VMR_D_ct";
        String fullnamepath "/O3_VMR_D_ct";
    }
    Temperature_A_min {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_A_min";
        String fullnamepath "/Temperature_A_min";
    }
    TropHeight_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_TqJ_D_sdev";
        String fullnamepath "/TropHeight_TqJ_D_sdev";
    }
    TotCO_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TotCO_TqJ_D_min";
        String fullnamepath "/TotCO_TqJ_D_min";
    }
    Emis_MW_D_max {
        Float32 _FillValue -9999.000000;
        String origname "Emis_MW_D_max";
        String fullnamepath "/Emis_MW_D_max";
    }
    EmisIR_D {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_D";
        String fullnamepath "/EmisIR_D";
    }
    CoarseCloudPres_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_TqJ_D_max";
        String fullnamepath "/CoarseCloudPres_TqJ_D_max";
    }
    FineCloudFrc_A_ct {
        Int16 _FillValue 0;
        String origname "FineCloudFrc_A_ct";
        String fullnamepath "/FineCloudFrc_A_ct";
    }
    RelHum_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_TqJ_D_sdev";
        String fullnamepath "/RelHum_TqJ_D_sdev";
    }
    SurfPres_Forecast_A_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_A_max";
        String fullnamepath "/SurfPres_Forecast_A_max";
    }
    GPHeight_D_max {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_D_max";
        String fullnamepath "/GPHeight_D_max";
    }
    TotH2OVap_A {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_A";
        String fullnamepath "/TotH2OVap_A";
    }
    EmisIR_A_min {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_A_min";
        String fullnamepath "/EmisIR_A_min";
    }
    TotO3_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_TqJ_D_min";
        String fullnamepath "/TotO3_TqJ_D_min";
    }
    SurfSkinTemp_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_TqJ_A_sdev";
        String fullnamepath "/SurfSkinTemp_TqJ_A_sdev";
    }
    TropHeight_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "TropHeight_TqJ_A_ct";
        String fullnamepath "/TropHeight_TqJ_A_ct";
    }
    SurfSkinTemp_A_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_A_max";
        String fullnamepath "/SurfSkinTemp_A_max";
    }
    CloudTopPres_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_TqJ_D_max";
        String fullnamepath "/CloudTopPres_TqJ_D_max";
    }
    CoarseCloudTemp_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_D_min";
        String fullnamepath "/CoarseCloudTemp_D_min";
    }
    CO_VMR_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_TqJ_D_err";
        String fullnamepath "/CO_VMR_TqJ_D_err";
    }
    TotO3_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_A_min";
        String fullnamepath "/TotO3_A_min";
    }
    CH4_VMR_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_D_min";
        String fullnamepath "/CH4_VMR_D_min";
    }
    Temperature_D_min {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_D_min";
        String fullnamepath "/Temperature_D_min";
    }
    TropHeight_A {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_A";
        String fullnamepath "/TropHeight_A";
    }
    SurfPres_Forecast_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "SurfPres_Forecast_TqJ_D_ct";
        String fullnamepath "/SurfPres_Forecast_TqJ_D_ct";
    }
    CH4_VMR_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_D_max";
        String fullnamepath "/CH4_VMR_D_max";
    }
    TotO3_D_err {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_D_err";
        String fullnamepath "/TotO3_D_err";
    }
    CH4_VMR_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_A_sdev";
        String fullnamepath "/CH4_VMR_A_sdev";
    }
    Temperature_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_TqJ_A_max";
        String fullnamepath "/Temperature_TqJ_A_max";
    }
    TotH2OVap_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_D_max";
        String fullnamepath "/TotH2OVap_D_max";
    }
    CO_VMR_A {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_A";
        String fullnamepath "/CO_VMR_A";
    }
    H2O_MMR_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_TqJ_D_ct";
        String fullnamepath "/H2O_MMR_TqJ_D_ct";
    }
    ClrOLR_A_ct {
        Int16 _FillValue 0;
        String origname "ClrOLR_A_ct";
        String fullnamepath "/ClrOLR_A_ct";
    }
    TotCldLiqH2O_MW_D_err {
        Float32 _FillValue -9999.000000;
        String origname "TotCldLiqH2O_MW_D_err";
        String fullnamepath "/TotCldLiqH2O_MW_D_err";
    }
    EmisIR_A_max {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_A_max";
        String fullnamepath "/EmisIR_A_max";
    }
    H2O_MMR_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_TqJ_D_max";
        String fullnamepath "/H2O_MMR_TqJ_D_max";
    }
    RelHumSurf_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_TqJ_A";
        String fullnamepath "/RelHumSurf_TqJ_A";
    }
    OLR_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "OLR_TqJ_A_max";
        String fullnamepath "/OLR_TqJ_A_max";
    }
    Temperature_MW_A_ct {
        Int16 _FillValue 0;
        String origname "Temperature_MW_A_ct";
        String fullnamepath "/Temperature_MW_A_ct";
    }
    CloudTopTemp_A {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_A";
        String fullnamepath "/CloudTopTemp_A";
    }
    CH4_VMR_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_D_sdev";
        String fullnamepath "/CH4_VMR_D_sdev";
    }
    TotCldLiqH2O_MW_D {
        Float32 _FillValue -9999.000000;
        String origname "TotCldLiqH2O_MW_D";
        String fullnamepath "/TotCldLiqH2O_MW_D";
    }
    CO_VMR_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_TqJ_A_err";
        String fullnamepath "/CO_VMR_TqJ_A_err";
    }
    CloudTopPres_A_ct {
        Int16 _FillValue 0;
        String origname "CloudTopPres_A_ct";
        String fullnamepath "/CloudTopPres_A_ct";
    }
    RelHum_liquid_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_TqJ_A_min";
        String fullnamepath "/RelHum_liquid_TqJ_A_min";
    }
    H2O_MMR_Lyr_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_TqJ_A";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_A";
    }
    H2O_MMR_Lyr_D_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_Lyr_D_ct";
        String fullnamepath "/H2O_MMR_Lyr_D_ct";
    }
    CloudFrc_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_D_max";
        String fullnamepath "/CloudFrc_D_max";
    }
    CH4_VMR_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_TqJ_D_err";
        String fullnamepath "/CH4_VMR_TqJ_D_err";
    }
    TotO3_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "TotO3_TqJ_D_ct";
        String fullnamepath "/TotO3_TqJ_D_ct";
    }
    H2O_MMR_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_A_sdev";
        String fullnamepath "/H2O_MMR_A_sdev";
    }
    RelHum_liquid_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_TqJ_A_sdev";
        String fullnamepath "/RelHum_liquid_TqJ_A_sdev";
    }
    SurfSkinTemp_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_TqJ_D_min";
        String fullnamepath "/SurfSkinTemp_TqJ_D_min";
    }
    SurfAirTemp_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_TqJ_A_max";
        String fullnamepath "/SurfAirTemp_TqJ_A_max";
    }
    SurfPres_Forecast_D_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_D_max";
        String fullnamepath "/SurfPres_Forecast_D_max";
    }
    GPHeight_A {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_A";
        String fullnamepath "/GPHeight_A";
    }
    EmisIR_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "EmisIR_TqJ_A_ct";
        String fullnamepath "/EmisIR_TqJ_A_ct";
    }
    O3_VMR_A_max {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_A_max";
        String fullnamepath "/O3_VMR_A_max";
    }
    RelHumSurf_D_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_D_min";
        String fullnamepath "/RelHumSurf_D_min";
    }
    OLR_A_err {
        Float32 _FillValue -9999.000000;
        String origname "OLR_A_err";
        String fullnamepath "/OLR_A_err";
    }
    SurfPres_Forecast_D_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_D_min";
        String fullnamepath "/SurfPres_Forecast_D_min";
    }
    RelHumSurf_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_TqJ_D_sdev";
        String fullnamepath "/RelHumSurf_TqJ_D_sdev";
    }
    H2O_MMR_Lyr_A_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_A_max";
        String fullnamepath "/H2O_MMR_Lyr_A_max";
    }
    CoarseCloudFrc_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_TqJ_D_sdev";
        String fullnamepath "/CoarseCloudFrc_TqJ_D_sdev";
    }
    CoarseCloudFrc_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_D_max";
        String fullnamepath "/CoarseCloudFrc_D_max";
    }
    TotH2OVap_MW_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_MW_D_sdev";
        String fullnamepath "/TotH2OVap_MW_D_sdev";
    }
    TotO3_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_TqJ_A_max";
        String fullnamepath "/TotO3_TqJ_A_max";
    }
    CH4_VMR_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_TqJ_A_sdev";
        String fullnamepath "/CH4_VMR_TqJ_A_sdev";
    }
    TropTemp_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TropTemp_A_max";
        String fullnamepath "/TropTemp_A_max";
    }
    CloudTopTemp_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_D_max";
        String fullnamepath "/CloudTopTemp_D_max";
    }
    TropPres_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_TqJ_A_min";
        String fullnamepath "/TropPres_TqJ_A_min";
    }
    TropPres_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_TqJ_D_max";
        String fullnamepath "/TropPres_TqJ_D_max";
    }
    EmisIR_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "EmisIR_TqJ_A_err";
        String fullnamepath "/EmisIR_TqJ_A_err";
    }
    TropHeight_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_D_sdev";
        String fullnamepath "/TropHeight_D_sdev";
    }
    RelHumSurf_liquid_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_A_sdev";
        String fullnamepath "/RelHumSurf_liquid_A_sdev";
    }
    RelHumSurf_liquid_A {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_A";
        String fullnamepath "/RelHumSurf_liquid_A";
    }
    TotO3_A_err {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_A_err";
        String fullnamepath "/TotO3_A_err";
    }
    CO_VMR_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_TqJ_D_max";
        String fullnamepath "/CO_VMR_TqJ_D_max";
    }
    FineCloudFrc_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_TqJ_D_min";
        String fullnamepath "/FineCloudFrc_TqJ_D_min";
    }
    CloudTopPres_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_TqJ_D_err";
        String fullnamepath "/CloudTopPres_TqJ_D_err";
    }
    TotO3_TqJ_A_err {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_TqJ_A_err";
        String fullnamepath "/TotO3_TqJ_A_err";
    }
    ClrOLR_D_ct {
        Int16 _FillValue 0;
        String origname "ClrOLR_D_ct";
        String fullnamepath "/ClrOLR_D_ct";
    }
    SurfPres_Forecast_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_TqJ_A";
        String fullnamepath "/SurfPres_Forecast_TqJ_A";
    }
    CoarseCloudPres_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_TqJ_A_min";
        String fullnamepath "/CoarseCloudPres_TqJ_A_min";
    }
    GPHeight_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_TqJ_A_min";
        String fullnamepath "/GPHeight_TqJ_A_min";
    }
    CoarseCloudTemp_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_TqJ_D";
        String fullnamepath "/CoarseCloudTemp_TqJ_D";
    }
    RelHum_liquid_D {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_liquid_D";
        String fullnamepath "/RelHum_liquid_D";
    }
    SurfAirTemp_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "SurfAirTemp_TqJ_A_ct";
        String fullnamepath "/SurfAirTemp_TqJ_A_ct";
    }
    H2O_MMR_Surf_A_max {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_A_max";
        String fullnamepath "/H2O_MMR_Surf_A_max";
    }
    H2O_MMR_Lyr_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_TqJ_A_sdev";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_A_sdev";
    }
    TotO3_A_ct {
        Int16 _FillValue 0;
        String origname "TotO3_A_ct";
        String fullnamepath "/TotO3_A_ct";
    }
    Temperature_MW_A_max {
        Float32 _FillValue -9999.000000;
        String origname "Temperature_MW_A_max";
        String fullnamepath "/Temperature_MW_A_max";
    }
    TotH2OVap_MW_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_MW_D_max";
        String fullnamepath "/TotH2OVap_MW_D_max";
    }
    GPHeight_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_A_sdev";
        String fullnamepath "/GPHeight_A_sdev";
    }
    SurfAirTemp_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_TqJ_D_min";
        String fullnamepath "/SurfAirTemp_TqJ_D_min";
    }
    SurfSkinTemp_D_err {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_D_err";
        String fullnamepath "/SurfSkinTemp_D_err";
    }
    Emis_MW_A_max {
        Float32 _FillValue -9999.000000;
        String origname "Emis_MW_A_max";
        String fullnamepath "/Emis_MW_A_max";
    }
    H2O_MMR_Lyr_D_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_D_err";
        String fullnamepath "/H2O_MMR_Lyr_D_err";
    }
    TropPres_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "TropPres_TqJ_A_ct";
        String fullnamepath "/TropPres_TqJ_A_ct";
    }
    OLR_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "OLR_TqJ_D_err";
        String fullnamepath "/OLR_TqJ_D_err";
    }
    Emis_MW_A_min {
        Float32 _FillValue -9999.000000;
        String origname "Emis_MW_A_min";
        String fullnamepath "/Emis_MW_A_min";
    }
    TropHeight_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TropHeight_A_max";
        String fullnamepath "/TropHeight_A_max";
    }
    FineCloudFrc_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "FineCloudFrc_TqJ_D_ct";
        String fullnamepath "/FineCloudFrc_TqJ_D_ct";
    }
    H2O_MMR_Lyr_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_TqJ_A_min";
        String fullnamepath "/H2O_MMR_Lyr_TqJ_A_min";
    }
    CoarseCloudFrc_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudFrc_TqJ_D_max";
        String fullnamepath "/CoarseCloudFrc_TqJ_D_max";
    }
    RelHumSurf_liquid_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_TqJ_D_sdev";
        String fullnamepath "/RelHumSurf_liquid_TqJ_D_sdev";
    }
    CoarseCloudTemp_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_TqJ_D_min";
        String fullnamepath "/CoarseCloudTemp_TqJ_D_min";
    }
    H2O_MMR_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_TqJ_D_min";
        String fullnamepath "/H2O_MMR_TqJ_D_min";
    }
    TotH2OVap_D_err {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_D_err";
        String fullnamepath "/TotH2OVap_D_err";
    }
    H2O_MMR_Lyr_A {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_A";
        String fullnamepath "/H2O_MMR_Lyr_A";
    }
    H2O_MMR_Surf_TqJ_A_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_Surf_TqJ_A_ct";
        String fullnamepath "/H2O_MMR_Surf_TqJ_A_ct";
    }
    H2O_MMR_Surf_D_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Surf_D_err";
        String fullnamepath "/H2O_MMR_Surf_D_err";
    }
    GPHeight_MW_A {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_MW_A";
        String fullnamepath "/GPHeight_MW_A";
    }
    SurfPres_Forecast_D {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_D";
        String fullnamepath "/SurfPres_Forecast_D";
    }
    SurfPres_Forecast_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_TqJ_D_max";
        String fullnamepath "/SurfPres_Forecast_TqJ_D_max";
    }
    CloudFrc_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudFrc_D_sdev";
        String fullnamepath "/CloudFrc_D_sdev";
    }
    ClrOLR_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_TqJ_A_sdev";
        String fullnamepath "/ClrOLR_TqJ_A_sdev";
    }
    O3_VMR_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_TqJ_D_min";
        String fullnamepath "/O3_VMR_TqJ_D_min";
    }
    TotO3_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_D_max";
        String fullnamepath "/TotO3_D_max";
    }
    TotO3_D_min {
        Float32 _FillValue -9999.000000;
        String origname "TotO3_D_min";
        String fullnamepath "/TotO3_D_min";
    }
    RelHumSurf_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_TqJ_D_max";
        String fullnamepath "/RelHumSurf_TqJ_D_max";
    }
    TotCH4_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_TqJ_D_max";
        String fullnamepath "/TotCH4_TqJ_D_max";
    }
    RelHumSurf_liquid_A_max {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_liquid_A_max";
        String fullnamepath "/RelHumSurf_liquid_A_max";
    }
    SurfPres_Forecast_A_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_A_min";
        String fullnamepath "/SurfPres_Forecast_A_min";
    }
    CoarseCloudTemp_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_TqJ_D_max";
        String fullnamepath "/CoarseCloudTemp_TqJ_D_max";
    }
    TotCH4_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_TqJ_A";
        String fullnamepath "/TotCH4_TqJ_A";
    }
    CH4_VMR_D_err {
        Float32 _FillValue -9999.000000;
        String origname "CH4_VMR_D_err";
        String fullnamepath "/CH4_VMR_D_err";
    }
    RelHumSurf_A_min {
        Float32 _FillValue -9999.000000;
        String origname "RelHumSurf_A_min";
        String fullnamepath "/RelHumSurf_A_min";
    }
    CO_VMR_TqJ_D {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_TqJ_D";
        String fullnamepath "/CO_VMR_TqJ_D";
    }
    SurfPres_Forecast_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_TqJ_A_max";
        String fullnamepath "/SurfPres_Forecast_TqJ_A_max";
    }
    GPHeight_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "GPHeight_D_sdev";
        String fullnamepath "/GPHeight_D_sdev";
    }
    CloudTopTemp_TqJ_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_TqJ_D_max";
        String fullnamepath "/CloudTopTemp_TqJ_D_max";
    }
    CoarseCloudPres_D_min {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudPres_D_min";
        String fullnamepath "/CoarseCloudPres_D_min";
    }
    SurfSkinTemp_A_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfSkinTemp_A_min";
        String fullnamepath "/SurfSkinTemp_A_min";
    }
    FineCloudFrc_D {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_D";
        String fullnamepath "/FineCloudFrc_D";
    }
    RelHum_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_D_sdev";
        String fullnamepath "/RelHum_D_sdev";
    }
    TotH2OVap_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "TotH2OVap_TqJ_D_ct";
        String fullnamepath "/TotH2OVap_TqJ_D_ct";
    }
    O3_VMR_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_TqJ_D_sdev";
        String fullnamepath "/O3_VMR_TqJ_D_sdev";
    }
    CloudTopTemp_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_A_sdev";
        String fullnamepath "/CloudTopTemp_A_sdev";
    }
    H2O_MMR_Surf_D_ct {
        Int16 _FillValue 0;
        String origname "H2O_MMR_Surf_D_ct";
        String fullnamepath "/H2O_MMR_Surf_D_ct";
    }
    SurfAirTemp_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "SurfAirTemp_TqJ_D_ct";
        String fullnamepath "/SurfAirTemp_TqJ_D_ct";
    }
    SurfPres_Forecast_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_D_sdev";
        String fullnamepath "/SurfPres_Forecast_D_sdev";
    }
    Emis_MW_D_min {
        Float32 _FillValue -9999.000000;
        String origname "Emis_MW_D_min";
        String fullnamepath "/Emis_MW_D_min";
    }
    CoarseCloudTemp_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_TqJ_D_sdev";
        String fullnamepath "/CoarseCloudTemp_TqJ_D_sdev";
    }
    OLR_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "OLR_TqJ_A_sdev";
        String fullnamepath "/OLR_TqJ_A_sdev";
    }
    ClrOLR_A_min {
        Float32 _FillValue -9999.000000;
        String origname "ClrOLR_A_min";
        String fullnamepath "/ClrOLR_A_min";
    }
    SurfPres_Forecast_TqJ_D_min {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_TqJ_D_min";
        String fullnamepath "/SurfPres_Forecast_TqJ_D_min";
    }
    TotH2OVap_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_A_max";
        String fullnamepath "/TotH2OVap_A_max";
    }
    TotCH4_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "TotCH4_TqJ_A_min";
        String fullnamepath "/TotCH4_TqJ_A_min";
    }
    CloudTopTemp_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_A_min";
        String fullnamepath "/CloudTopTemp_A_min";
    }
    TotCldLiqH2O_MW_A_ct {
        Int16 _FillValue 0;
        String origname "TotCldLiqH2O_MW_A_ct";
        String fullnamepath "/TotCldLiqH2O_MW_A_ct";
    }
    FineCloudFrc_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "FineCloudFrc_TqJ_A_max";
        String fullnamepath "/FineCloudFrc_TqJ_A_max";
    }
    OLR_D {
        Float32 _FillValue -9999.000000;
        String origname "OLR_D";
        String fullnamepath "/OLR_D";
    }
    CO_VMR_TqJ_A_min {
        Float32 _FillValue -9999.000000;
        String origname "CO_VMR_TqJ_A_min";
        String fullnamepath "/CO_VMR_TqJ_A_min";
    }
    TotCldLiqH2O_MW_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TotCldLiqH2O_MW_D_sdev";
        String fullnamepath "/TotCldLiqH2O_MW_D_sdev";
    }
    H2O_MMR_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_TqJ_D_sdev";
        String fullnamepath "/H2O_MMR_TqJ_D_sdev";
    }
    Temperature_D_ct {
        Int16 _FillValue 0;
        String origname "Temperature_D_ct";
        String fullnamepath "/Temperature_D_ct";
    }
    TropPres_TqJ_D_sdev {
        Float32 _FillValue -9999.000000;
        String origname "TropPres_TqJ_D_sdev";
        String fullnamepath "/TropPres_TqJ_D_sdev";
    }
    CloudFrc_A_ct {
        Int16 _FillValue 0;
        String origname "CloudFrc_A_ct";
        String fullnamepath "/CloudFrc_A_ct";
    }
    CloudTopTemp_TqJ_D_err {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopTemp_TqJ_D_err";
        String fullnamepath "/CloudTopTemp_TqJ_D_err";
    }
    Topography {
        Float32 _FillValue -9999.000000;
        String origname "Topography";
        String fullnamepath "/Topography";
    }
    CH4_VMR_A_ct {
        Int16 _FillValue 0;
        String origname "CH4_VMR_A_ct";
        String fullnamepath "/CH4_VMR_A_ct";
    }
    CloudTopPres_D_max {
        Float32 _FillValue -9999.000000;
        String origname "CloudTopPres_D_max";
        String fullnamepath "/CloudTopPres_D_max";
    }
    RelHum_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "RelHum_A_sdev";
        String fullnamepath "/RelHum_A_sdev";
    }
    O3_VMR_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "O3_VMR_TqJ_D_ct";
        String fullnamepath "/O3_VMR_TqJ_D_ct";
    }
    TotH2OVap_MW_A_max {
        Float32 _FillValue -9999.000000;
        String origname "TotH2OVap_MW_A_max";
        String fullnamepath "/TotH2OVap_MW_A_max";
    }
    O3_VMR_TqJ_A {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_TqJ_A";
        String fullnamepath "/O3_VMR_TqJ_A";
    }
    O3_VMR_TqJ_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "O3_VMR_TqJ_A_sdev";
        String fullnamepath "/O3_VMR_TqJ_A_sdev";
    }
    TotCO_A_ct {
        Int16 _FillValue 0;
        String origname "TotCO_A_ct";
        String fullnamepath "/TotCO_A_ct";
    }
    SurfAirTemp_D_err {
        Float32 _FillValue -9999.000000;
        String origname "SurfAirTemp_D_err";
        String fullnamepath "/SurfAirTemp_D_err";
    }
    CloudTopTemp_TqJ_D_ct {
        Int16 _FillValue 0;
        String origname "CloudTopTemp_TqJ_D_ct";
        String fullnamepath "/CloudTopTemp_TqJ_D_ct";
    }
    SurfPres_Forecast_A_sdev {
        Float32 _FillValue -9999.000000;
        String origname "SurfPres_Forecast_A_sdev";
        String fullnamepath "/SurfPres_Forecast_A_sdev";
    }
    H2O_MMR_Lyr_A_err {
        Float32 _FillValue -9999.000000;
        String origname "H2O_MMR_Lyr_A_err";
        String fullnamepath "/H2O_MMR_Lyr_A_err";
    }
    CoarseCloudTemp_TqJ_A_max {
        Float32 _FillValue -9999.000000;
        String origname "CoarseCloudTemp_TqJ_A_max";
        String fullnamepath "/CoarseCloudTemp_TqJ_A_max";
    }
    CoarseCloudLayer {
        String long_name "Mid-layer pressure";
        String units "hPa";
        String format "F7.2";
        String origname "CoarseCloudLayer";
        String fullnamepath "/CoarseCloudLayer";
    }
    EmisFreqIR {
        String long_name "Wavenumber";
        String units "cm^-1";
        String format "F5.1";
        String origname "EmisFreqIR";
        String fullnamepath "/EmisFreqIR";
    }
    EmisFreqMW {
        String long_name "Frequency";
        String units "GHz";
        String format "F5.1";
        String origname "EmisFreqMW";
        String fullnamepath "/EmisFreqMW";
    }
    FineCloudLayer {
        String long_name "Mid-layer pressure";
        String units "hPa";
        String format "F7.2";
        String origname "FineCloudLayer";
        String fullnamepath "/FineCloudLayer";
    }
    H2OPressureLay {
        String long_name "Mid-layer Pressure";
        String units "hPa";
        String format "F7.2";
        String origname "H2OPressureLay";
        String fullnamepath "/H2OPressureLay";
    }
    H2OPressureLev {
        String long_name "Pressure";
        String units "hPa";
        String format "F7.2";
        String origname "H2OPressureLev";
        String fullnamepath "/H2OPressureLev";
    }
    Latitude {
        String long_name "Latitude";
        String units "degrees_north";
        String format "F5.1";
        String origname "Latitude";
        String fullnamepath "/Latitude";
    }
    Longitude {
        String long_name "Longitude";
        String units "degrees_east";
        String format "F6.1";
        String origname "Longitude";
        String fullnamepath "/Longitude";
    }
    StdPressureLev {
        String long_name "Pressure";
        String units "hPa";
        String format "F7.2";
        String origname "StdPressureLev";
        String fullnamepath "/StdPressureLev";
    }
}
