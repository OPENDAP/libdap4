
Name: libdap
Summary: The C++ DAP2 library from OPeNDAP.
Version: 3.5.0
Release: 1

Source0: http://www.opendap.org/pub/%{name}-%{version}.tar.gz
URL: http://www.opendap.org/

Group: Development/Libraries
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
License: LGPL
Requires: curl >= 7.12.0
Requires: xml2 >= 2.5.7

Prefix: /usr/local

%description
The libdap++ library contains an implementation of DAP2. This package
contains the library, libdap-config, getdap and deflate. The script 
libdap-config simplifies using the library in other projects. The getdap 
utility is a simple command-line tool to read from DAP2 servers. It is
built using the library and demonstrates simple uses of it. The deflate
utility is used by the library when it returns compressed responses.

%prep
%setup -q

%build
%configure

%install
if echo $RPM_BUILD_ROOT | grep '/tmp/.*-buildroot'; then
    rm -rf $RPM_BUILD_ROOT
else
    echo "RPM_BUILD_ROOT not set!"
fi
%makeinstall

%clean
if echo $RPM_BUILD_ROOT | grep '/tmp/.*-buildroot'; then
    rm -rf $RPM_BUILD_ROOT
else
    echo "RPM_BUILD_ROOT not set!"
fi

%files
%{_libdir}/libdap++.a
%{_bindir}/libdap-config
%{_bindir}/getdap
%{_sbindir}/deflate
%dir 
%{_includedir}/%{name}

%defattr(-,root,root,0755)

%doc README NEWS

%changelog
* Wed May 11 2005 James Gallagher <jimg@zoey.opendap.org> 3.5.0-1
- Removed version numbers from .a and includes directory.

* Tues May 10 2005 James Gallagher <jimg@zoey.opendap.org> 
- Mostly works. Problems: Not sure if the %post script stuff works.
- Must also address the RHEL3 package deps issue (curl 7.12.0 isn't available;
  not sure about xml2 2.5.7). At least the deps fail when they are not present!

* Fri May  6 2005 James Gallagher <jimg@zoey.opendap.org> 
- Initial build.


