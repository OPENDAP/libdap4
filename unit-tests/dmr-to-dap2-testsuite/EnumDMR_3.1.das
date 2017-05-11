Attributes {
    b1 {
    }
    s {
        Int16 test 1, 2, 3;
        DAP4_switches {
            String public_key "some_large_hex_value";
            String colors "red", "green", "blue";
            Float32 temp 98.6;
        }
        dap4:dropped_members {
            i64 {
                String dap4:type "Int64";
            }
            ui64 {
                String dap4:type "UInt64";
            }
        }
        i8 {
        }
        ui8 {
            Int16 test 1, 2, 3;
            DAP4_switches {
                String public_key "some_large_hex_value";
            }
        }
        i16 {
        }
        ui16 {
        }
        i32 {
        }
        ui32 {
        }
        f32 {
        }
        f64 {
        }
        s {
        }
        u {
        }
    }
}
