Name: libdap
Summary: The C++ DAP2 library from OPeNDAP
Version: 3.7.10
Release: 1

Source0: http://www.opendap.org/pub/source/libdap-%{version}.tar.gz
URL: http://www.opendap.org/

Group: Development/Libraries
BuildRoot:  %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
License: LGPLv2+ and W3C

# Mandrake
# BuildRequires: libcurl3-devel >= 7.10.6 libxml2-devel >= 2.5.7
# fedora
BuildRequires: curl-devel >= 7.10.6 libxml2-devel >= 2.5.7
BuildRequires: doxygen graphviz
BuildRequires: zlib-devel
BuildRequires: pkgconfig

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
Summary: Development and header files from libdap
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: curl-devel >= 7.10.6 libxml2-devel >= 2.5.7
Requires: pkgconfig
# for the /usr/share/aclocal directory ownership
Requires: automake

%description devel
This package contains all the files needed to develop applications that
will use libdap.


%package doc
Summary: Documentation of the libdap library
Group: Documentation

%description doc
Documentation of the libdap library.


%prep
%setup -q


%build
%configure --disable-static --disable-dependency-tracking
make %{?_smp_mflags}

make docs


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT INSTALL="%{__install} -p"
rm $RPM_BUILD_ROOT/%{_libdir}/*.la

rm -rf __fedora_docs
cp -pr docs __fedora_docs
# those .map and .md5 are of dubious use, remove them
rm -f __fedora_docs/html/*.map __fedora_docs/html/*.md5
# use the ChangeLog timestamp to have the same timestamps for the doc files 
# for all arches
touch -r ChangeLog __fedora_docs/html/*


%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%{_bindir}/getdap
%{_sbindir}/deflate
%{_libdir}/libdap.so.*
%{_libdir}/libdapclient.so.*
%{_libdir}/libdapserver.so.*
%doc README NEWS COPYING COPYRIGHT_URI README.AIS README.dodsrc
%doc COPYRIGHT_W3C

%files devel
%defattr(-,root,root,-)
%{_libdir}/libdap.so
%{_libdir}/libdapclient.so
%{_libdir}/libdapserver.so
%{_libdir}/pkgconfig/libdap*.pc
%{_bindir}/dap-config
%{_includedir}/libdap/
%{_datadir}/aclocal/*

%files doc
%defattr(-,root,root,-)
%doc __fedora_docs/html/


%changelog
* Wed Nov 28 2007 James Gallagher <jgallagher@opendap.org> - 3.7.10-1
- Update for 3.7.10

* Tue Jun 26 2007 James Gallagher <jgallagher@opendap.org> - 3.7.8-1
- Update for 3.7.8

* Thu Feb  8 2007 James Gallagher <jgallagher@opendap.org> - 3.7.7-1
- Update for 3.7.7

* Thu Feb  8 2007 James Gallagher <jgallagher@opendap.org> - 3.7.5-1
- Update for 3.7.5

* Tue Jan 02 2007 Patrick West <pwest@ucar.edu> - 3.7.4-1
- Update for 3.7.4

* Fri Nov 24 2006 James Gallagher <jgallagher@opendap.org> - 3.7.3-1
- Update for 3.7.3-1

* Mon Sep 15 2006 James Gallagher <jgallagher@opendap.org> - 3.7.2-1
- Update to 3.7.2

* Mon Aug 21 2006 James Gallagher <jgallagher@opendap.org> - 3.7.1
- Update to 3.7.1

* Mon Feb 27 2006 James Gallagher <jgallagher@opendap.org> - 3.6.0-1
- Update to 3.6.0

* Mon Nov 21 2005 Patrice Dumas <pertusus@free.fr> - 3.5.3-2
- fix Source0

* Tue Aug 30 2005 Patrice Dumas <pertusus@free.fr> - 3.5.2-3
- Add missing Requires

* Sat Jul  2 2005 Patrice Dumas <pertusus@free.fr> - 3.5.1-2
- Support for shared libraries
- Add COPYING
- Update with fedora template

* Thu May 12 2005 James Gallagher <jimg@comet.opendap.org> - 3.5.0-1
- Changed: Requires xml2 to libxml2

* Wed May 11 2005 James Gallagher <jimg@zoey.opendap.org> 3.5.0-1
- Removed version numbers from .a and includes directory.

* Tue May 10 2005 James Gallagher <jimg@zoey.opendap.org> 
- Mostly works. Problems: Not sure if the %%post script stuff works.
- Must also address the RHEL3 package deps issue (curl 7.12.0 isn't available;
  not sure about xml2 2.5.7). At least the deps fail when they are not present!

* Fri May  6 2005 James Gallagher <jimg@zoey.opendap.org> 
- Initial build.
