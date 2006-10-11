The complete DDS:
Dataset {
    Grid {
      Array:
        Byte SST[lon = 11][lat_reversed = 13];
      Maps:
        Float32 lon[11];
        Float32 lat_reversed[13];
    } SST;
} SST_grid;
The data:
Grid {
  Array:
    Byte SST[lon = 2][lat_reversed = 4];
  Maps:
    Float32 lon[2];
    Float32 lat_reversed[4];
} SST = {  Array: {{1, 2},{5, 6}}  Maps: {-51, -19}, {41, 54} };
