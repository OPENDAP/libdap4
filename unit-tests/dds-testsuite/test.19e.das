# This DAS is used to test merging attributes into an existing DDS. It's
# designed to work with the DDS in test.19b.

Attributes {
    c%20d {
        String long_name "c d with a WWW escape sequence";
	    sub {
	        String about "Attributes inside attributes";
	        Float64 pi 3.1415;
        }
    }
}
