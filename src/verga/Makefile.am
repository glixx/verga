#
#    Copyright (C) 1987-2015 by Jeffery P. Hansen
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

DISTCLEANFILES=vgrammar.hpp vgrammar.cpp luthor.cpp

BUILT_SOURCES=vgrammar.hpp
AM_CFLAGS=@TOP_IPATH@ @COMMON_IPATH@
AM_CXXFLAGS=@TOP_IPATH@ @COMMON_IPATH@
AM_YFLAGS=-d

bin_PROGRAMS=verga++
verga___SOURCES=bytecode.cpp channel.cpp circuit.cpp commands.cpp directive.cpp \
	error.cpp evqueue.cpp expr.cpp io.cpp memory.cpp mitem.cpp module.cpp \
	multint.cpp net.cpp operators.cpp pathmod.cpp paux.cpp specify.cpp \
	statement.cpp systask.cpp task.cpp verga.cpp trigger.cpp value.cpp \
	verilog.cpp vgrammar.ypp luthor.lpp yywrap.c release.cpp bytecode.h \
	directive.h evqueue.h memory.h multint.h specify.h task.h trigger.h \
	channel.h dynitem.h expr.h mitem.h net.h  statement.h verga.hpp value.h \
	yybasic.h circuit.h error.h io.h module.h operators.h systask.h \
	thyme_config.h verilog.h

verga___LDFLAGS=@COMMON_LPATH@
verga___LDADD=-lcommon
