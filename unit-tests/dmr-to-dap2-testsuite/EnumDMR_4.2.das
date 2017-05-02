Attributes {
    e {
        d4:enum_def {
            Byte red 1;
            Byte green 2;
            Byte blue 3;
        }
    }
    b1 {
    }
    /g1/e {
        d4:enum_def {
            Byte red 1;
            Byte green 2;
            Byte blue 3;
        }
    }
    /g2/e {
        d4:enum_def {
            UInt16 white 1;
            UInt16 light 2;
            UInt16 med 3;
            UInt16 dark 4;
            UInt16 black 5;
        }
    }
    /g2/gg2/e {
        d4:enum_def {
            UInt16 white 1;
            UInt16 light 2;
            UInt16 med 3;
            UInt16 dark 4;
            UInt16 black 5;
        }
    }
    g1 {
        Int16 test 1, 2, 3;
    }
    g2 {
        gg2 {
            Int16 test 1, 2, 3;
        }
    }
}
