# This DAS is used to test merging attributes into an existing DDS. It's
# designed to work with the DDS in test.19b.

Attributes {

    b#c {
        String long_name "b pound c";
    }
    b#c_dim_0 {
        Float64 add_offset 0.125;
    }
    huh {
        String long_name "The Grid huh";
		colors {
	    	String long_name2 "The color map vector";
        }
		Image#data {
		}
    }
    huh_dim_0 {
        String units "m/s";
    }
}
