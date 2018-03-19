Attributes {
    s {
        dap4:dropped_members {
            i64 {
                String dap4:type "Int64";
            }
            ui64 {
                String dap4:type "UInt64";
            }
            o {
                String dap4:type "Opaque";
            }
        }
        e {
            d4:enum_def {
                Byte red 1;
                Byte green 2;
                Byte blue 3;
            }
        }
    }
}
