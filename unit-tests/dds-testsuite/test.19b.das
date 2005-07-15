# This DAS is used to test merging attributes into an existing DDS. It's
# designed to work with the DDS in test.19b.

Attributes {
    NC_GLOBAL {
        String long_name "Attribute merge test";
        Int32 primes 2, 3, 5, 7, 11;
    }

    a {
    }

    b#c {
        String long_name "b pound c";
    }

    c%20d {
        String long_name "c d with a WWW escape sequence";
	sub {
	    String about "Attributes inside attributes";
	    Float64 pi 3.1415;
        }
    }

    huh {
        String long_name "The Grid huh";
	colors {
	    String long_name "The color map vector";
        }
	Image#data {
	}
    }
}
