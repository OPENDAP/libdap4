# This should work (now, it used to generate an error because `attributes'
# is a reserved word.
attributes {
    a {
	string b c;
	int32 attributes 4;
    }
}
