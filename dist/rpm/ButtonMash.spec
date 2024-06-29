%global debug_package %{nil}
Name:     ButtonMash
Version:  beta13
Release:  1%{?dist}
Summary:  websocket server protocol for accessing hardware/software that act like a SNES (or are a SNES)
License:  GPLv3
URL:      https://github.com/Skarsnik/Button-Mash
Source:   https://github.com/Skarsnik/Button-Mash/archive/refs/tags/v%{version}.tar.gz
BuildRequires: qt5-qtbase-devel
BuildRequires: qt5-qtwebsockets-devel
BuildRequires: qt5-qtserialport-devel
BuildRequires: qt5-qtgamepad-devel
BuildRequires: git

Requires: qt5-qtbase
Requires: qt5-qtwebsockets
Requires: qt5-qtserialport
Requires: qt5-qtgamepad

%description
Button Mash is an Input Display written mainly to display SNES controller input.
It currently support SNES Classic hacked with Hakchi2 CE, Arduino based solution for real hardware (using Nintendo Spy firmware), Usb2Snes and regular controllers.

%prep
%autosetup -n Button-Mash

%conf
qmake-qt5 %{name}.pro CONFIG+='release'

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
