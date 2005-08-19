
Name: libdap
Summary: The C++ DAP2 library from OPeNDAP
Version: 3.5.2
Release: 2

Source0: http://www.opendap.org/pub/3.5/source/%{name}-%{version}.tar.gz
URL: http://www.opendap.org/

Group: Development/Libraries
BuildRoot:  %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
License: LGPL/W3C
# Mandrake
#BuildRequires: libcurl3-devel >= 7.10.6 libxml2-devel >= 2.5.7
# fedora
BuildRequires: curl-devel >= 7.10.6 libxml2-devel >= 2.5.7

# This package could be relocatable. In that case uncomment the following
# line
Prefix: %{_prefix}

%description
The libdap++ library contains an implementation of DAP2. This package
contains the library, dap-config, getdap and deflate. The script dap-config
simplifies using the library in other projects. The getdap utility is a
simple command-line tool to read from DAP2 servers. It is built using the
library and demonstrates simple uses of it. The deflate utility is used by
the library when it returns compressed responses.

%package devel
Summary: Static libraries and header files from libdap
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
#
%description devel
This package contains all the files needed to develop applications that
will use libdap.

%prep
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
rm -f $RPM_BUILD_ROOT/%{_libdir}/*.la

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_bindir}/getdap
%{_sbindir}/deflate
%{_libdir}/libdap.so.*
%doc README NEWS COPYING COPYRIGHT_URI README.AIS README.dodsrc
%doc COPYRIGHT_W3C

%files devel
%defattr(-,root,root,-)
%{_libdir}/libdap.a
%{_libdir}/libdap.so
%{_bindir}/dap-config
%{_includedir}/libdap/
%{_datadir}/aclocal/*

%changelog
* Sat Jul  2 2005 Patrice Dumas <dumas@centre-cired.fr> - 3.5.1-2
- Support for shared libraries
- Add COPYING
- Update with fedora template

* Thu May 12 2005 James Gallagher <jimg@comet.opendap.org> - 3.5.0-1
- Changed: Requires xml2 to libxml2

* Wed May 11 2005 James Gallagher <jimg@zoey.opendap.org> 3.5.0-1
- Removed version numbers from .a and includes directory.

* Tue May 10 2005 James Gallagher <jimg@zoey.opendap.org> 
- Mostly works. Problems: Not sure if the %post script stuff works.
- Must also address the RHEL3 package deps issue (curl 7.12.0 isn't available;
  not sure about xml2 2.5.7). At least the deps fail when they are not present!

* Fri May  6 2005 James Gallagher <jimg@zoey.opendap.org> 
- Initial build.
