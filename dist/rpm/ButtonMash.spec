%global debug_package %{nil}
Name:     ButtonMash
Version:  beta13
Release:  1%{?dist}
Summary:  websocket server protocol for accessing hardware/software that act like a SNES (or are a SNES)
License:  GPLv3
# URL:      https://skarsnik.github.io/%{name}/
Source:   https://github.com/Skarsnik/Button-Mash/archive/refs/tags/v%{version}.tar.gz
# BuildRequires: qt6-qtbase-devel
# BuildRequires: qt6-qtwebsockets-devel
# BuildRequires: qt6-qtserialport-devel
BuildRequires: git

# Requires: qt6-qtbase
# Requires: qt6-qtwebsockets
# Requires: qt6-qtserialport

%description
Button Mash is an Input Display written mainly to display SNES controller input.
It currently support SNES Classic hacked with Hakchi2 CE, Arduino based solution for real hardware (using Nintendo Spy firmware), Usb2Snes and regular controllers.

%prep
%autosetup -n %{name}

%conf
qmake %{name}.pro CONFIG+='release'

%build
%make_build

%install
mkdir -p %{buildroot}/usr/bin/
install -m 755 %{name} %{buildroot}/usr/bin/%{name}
mkdir -p %{buildroot}/usr/share/pixmaps
install -m 644 icon128x128.png %{buildroot}/usr/share/pixmaps/%{name}.png
mkdir -p %{buildroot}/usr/share/applications
install -m 644 dist/rpm/%{name}.desktop %{buildroot}/usr/share/applications/%{name}.desktop
mkdir -p %{buildroot}/etc/%{name}
cp -a Skins %{buildroot}/etc/%{name}


%files
/usr/bin/%{name}
/usr/share/pixmaps/%{name}.png
/usr/share/applications/%{name}.desktop
/etc/%{name}

%license License-GPL3.txt
%doc Readme.md

%check
make check

%changelog
* Thu Jan 19 2023 Thibault Delattre
- Initial version of the package
