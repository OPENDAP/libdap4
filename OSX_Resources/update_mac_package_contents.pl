#!/usr/bin/perl
#

use strict 'vars';
my $debug = 0;

unix2mac("README", "OSX_Resources/ReadMe.txt");

my $version_number = get_version_number();
print "version number: $version_number\n" if $debug ge 1;

my $package_size = get_package_size("mac_osx/usr");
print "Package_size: $package_size\n" if $debug ge 1;

substitute_values("OSX_Resources/Info.plist", $version_number, $package_size);

###################################################################

# Substitute values into the named plist file. This probably only works 
# with an Info.plist file.
sub substitute_values {
  my ($info_plist, $version_number, $package_size) = @_;

  my $get_info = 0;
  my $short_version = 0;
  my $installed_size = 0;

  open INFO_PLIST, $info_plist or die("Could not open $info_plist\n");
  open OUT, ">$info_plist.out"  or die("Could not open $info_plist.out\n");

  while (<INFO_PLIST>) {
    if ( /CFBundleGetInfoString/ ) {
      $get_info = 1;
      print OUT $_ ;
    } elsif ( /CFBundleShortVersionString/ ) {
      $short_version = 1;
      print OUT $_ ;
    } elsif ( /IFPkgFlagInstalledSize/ ) {
      $installed_size = 1;
      print OUT $_ ;
    } elsif ( $get_info == 1 && /^(\s*)<string>[0-9.]+, (.*)<\/string>/ ) {
      print OUT "$1<string>$version_number, $2</string>\n" ;
      $get_info = 0;
    } elsif ( $short_version == 1 && /^(\s*)<string>[0-9.]+<\/string>/ ) {
      print OUT "$1<string>$version_number</string>\n" ;
      $short_version = 0;
    } elsif ( $installed_size == 1 && /^(\s*)<integer>.*/ ) {
      print OUT "$1<integer>$package_size</integer>\n" ;
      $installed_size = 0;
    } else {
      print OUT $_ ;
    }
  }

  close INFO_PLIST;
  close OUT;

  rename "$info_plist.out", $info_plist 
    or die("Could not rename $info_plist\n");
}

# Find the size in kilobytes of the given directory
sub get_package_size {
  my ($package_dir) = @_;
  my $result = `du -ks $package_dir`;

  my ($package_size) = $result =~ m/(\s*[0-9]+).*/
    or die("Could not figure out the package size!\n");
  return $package_size;
}

# Look for the version number of libdap in configure.ac
sub get_version_number {
  my $infile_name = "configure.ac";
  my $version_number = "0.0.0";

  open IN, $infile_name or die("Could not open $infile_name\n");

  while (<IN>) {
    if ( /AC_INIT\(libdap, *([0-9.]+)/ ) {
      $version_number = $1;
    }
  }

  close IN;

  return $version_number;
}

# Read a textfile where each line is terminated by a newline and
# paragraphs are terminated by an otherwise blank line. Write the text
# out without those pesky line-terminating newlines.
sub unix2mac {
  my ($infile_name, $outfile_name) = @_;

  open IN, $infile_name or die("Could not open $infile_name!\n");
  open OUT, ">$outfile_name" 
    or die("Could not open output for $outfile_name!\n");

  my $code = 0;

  while (<IN>) {
    if ( /^<code>\s*$/ ) {
      $code = 1;
    } elsif ( /^<\/code>\s*$/ ) {
      $code = 0;
    } elsif ( $code eq 1 ) {
      print OUT $_ ;
    } elsif ( /^\s*$/ ) {
      print OUT "\n\n" ;	# Blank line
    } else {
      chomp $_ ; print OUT $_ ; # Character line
    }
  }

  close IN;
  close OUT;
}
