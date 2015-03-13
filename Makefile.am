## Process this file with automake to produce Makefile.in
#
#    Copyright (C) 1987-2015 by Jeffery P. Hansen
#    Copyright (C) 2015 by Andrey V. Skvortsov
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

AUTOMAKE_OPTIONS = foreign

#
# List of directories that contain data files necessary for tkgate to run.
#
VG_DATADIRS= doc test

#
# List of data which will be included in distribution
#
VG_DISTITEMS=$(VG_DATADIRS) pkg-comment pkg-descr

#
# Subdirectories in which more makefiles can be found
#
SUBDIRS = src

#
# List of data files in this directory that should be included with a tkgate distribution.
#
#dist_pkgdata_DATA = site-preferences

#
# List of man pages for tkgate
#
dist_man_MANS = verga.1

#
# List of common documentation files
#
dist_doc_DATA = README COPYING TODO AUTHORS ChangeLog

#
# List of header files that should not be automatically installed
#
noinst_HEADERS = options.h

#
# Update the release data for this compilation
#
release:
	echo 'char *release_date = "'`(LANG=; date "+%b %d %Y")`'";' > $(srcdir)/src/verga/release.cpp

dist-hook:
	@echo Packaging data files...
	@for d in $(VG_DISTITEMS); do \
	   cp -pR $(srcdir)/$$d $(distdir); \
	done

tests:
	cd test/verga;./runtests.sh
#
# Copy all the data files we need
#
install-data-hook:
	@echo Installing data files...
	@for d in $(TKG_DATADIRS); do \
	   cp -pR $(srcdir)/$$d $(DESTDIR)$(pkgdatadir); \
	done

uninstall-local:
	@echo Uninstalling data files...
	@for d in $(VG_DATADIRS); do \
	   chmod -R u+w $(DESTDIR)$(pkgdatadir)/$$d; \
	   rm -rf $(DESTDIR)$(pkgdatadir)/$$d; \
	done

