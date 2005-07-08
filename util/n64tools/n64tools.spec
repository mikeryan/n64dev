Summary: Linux tools for the N64
Name: n64tools
Version: 1
Release: 1
Copyright: GPL 
Group:  Development/Tools
Source0: http://n64dev.50megs.com/n64tools.tar.gz
BuildRoot: /tmp/n64tools-root
Url: http://n64dev.50megs.com
Prefix: /usr

%description
A collection of linux tools for the N64

cd64comm - Communication with CD64
drjr     - Communcation with V64JR
insertfs - Insert a filing system into a Nintendo 64 ROM
n64term  - Terminal to communiate with N64
nifty    - Edit boot address, rom name and fix checksum for N64 ROMs
raw2s    - Convert a binary file into a GCC .S asm file
renamer  - Rename and split ROMs across CDs. Makes HTML list of each CD
tiff2rgb - Convert a TIFF image into N64 format RGB

%prep
%setup -n n64tools

%build
make all

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make ROOT=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
/usr/bin/cd64comm
/usr/bin/drjr
/usr/bin/insertfs
/usr/bin/n64term
/usr/bin/nifty
/usr/bin/raw2s
/usr/bin/renamer
/usr/bin/tiff2rgb

