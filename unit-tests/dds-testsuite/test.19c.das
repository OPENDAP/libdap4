# This DAS is used to test merging attributes into an existing DDS. It's
# designed to work with the DDS in test.19b.

Attributes {
    NC_GLOBAL {
        String long_name "Attribute merge test";
        Int32 primes 2, 3, 5, 7, 11;
    }

    a {
    }
}
