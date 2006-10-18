The complete DDS:
Dataset {
    Grid {
      Array:
        Byte SST[lat_reversed = 13][lon = 11];
      Maps:
        Float32 lat_reversed[13];
        Float32 lon[11];
    } SST;
} SST_grid;
The data:
Grid {
  Array:
    Byte SST[lat_reversed = 2][lon = 3];
  Maps:
    Float32 lat_reversed[2];
    Float32 lon[3];
} SST = {  Array: {{0, 1, 2},{3, 4, 5}}  Maps: {41, 54}, {-83, -51, -19} };
