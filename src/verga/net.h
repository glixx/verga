/****************************************************************************
    This file is part of Verga++ Verilog simulator
    Copyright (C) 1987-2015 by Jeffery P. Hansen
    Copyright (C) 2015 by Andrey V. Skvortsov

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
****************************************************************************/
#ifndef __net_h
#define __net_h
/*****************************************************************************
 *
 * The wire-like verilog types are:
 *
 *     supply0
 *     supply1
 *     wire
 *     wand
 *     wor
 *     tri
 *     tri0
 *     tri1
 *     triand
 *     trior
 *     trireg (small)
 *     trireg (medium)
 *     trireg (large)
 *
 * The reg-like verilog types are:
 *
 *     reg
 *     integer
 *     time
 *     event		Implemented as 0-bit reg
 *
 *****************************************************************************/

/*****************************************************************************
 *
 * Propery table for generating net/reg types.
 *
 *****************************************************************************/
enum NetTypeProp
{
  NT_NONE = 0,
#define NT_NONE		NT_NONE	/* Base type is wire */
  NT_P_WIRE = 0x1,
#define NT_P_WIRE		NT_P_WIRE	/* Base type is wire */
  NT_P_REG = 0x2,
#define NT_P_REG		NT_P_REG	/* Base type is reg */
  NT_P_PARAMETER = 0x4,
#define NT_P_PARAMETER		NT_P_PARAMETER	/* Base type is parameter */
  NT_P_MEMORY = 0x8,
#define NT_P_MEMORY		NT_P_MEMORY	/* Base type is memory */
  NT_P_AND = 0x10,
#define NT_P_AND		NT_P_AND	/* Wire does implicit AND */
  NT_P_OR = 0x20,
#define NT_P_OR			NT_P_OR		/* Wire does implicit OR */
  NT_P_TRI = 0x40,
#define NT_P_TRI		NT_P_TRI	/* Wire can have multiple drivers */
  NT_P_TRIREG = 0x80,
#define NT_P_TRIREG		NT_P_TRIREG	/* Wire can store charge */
  NT_P_SMALL = 0x100,
#define NT_P_SMALL		NT_P_SMALL	/* Low charge */
  NT_P_MEDIUM = 0x200,
#define NT_P_MEDIUM		NT_P_MEDIUM	/* Medium charge */
  NT_P_LARGE = 0x400,
#define NT_P_LARGE		NT_P_LARGE	/* High charge */
  NT_P_SUPPLY = 0x800,
#define NT_P_SUPPLY		NT_P_SUPPLY	/* Reg is a "supply" */
  NT_P_INTEGER = 0x1000,
#define NT_P_INTEGER		NT_P_INTEGER	/* Reg is an integer */
  NT_P_TIME = 0x2000,
#define NT_P_TIME		NT_P_TIME	/* Reg is a time */
  NT_P_EVENT = 0x4000,
#define NT_P_EVENT		NT_P_EVENT	/* Reg is an event */
  NT_P_REAL = 0x8000,
#define NT_P_REAL		NT_P_REAL	/* Reg is a real */
#define NT_P_REGTYPE_MASK	0xf000		/* Mask to get the reg type */
#define NT_P_BASE_MASK		0xffff		/* Mask to get base type */

  NT_P_INPUT = 0x10000,
#define NT_P_INPUT		NT_P_INPUT	/* Input */
  NT_P_OUTPUT = 0x20000,
#define NT_P_OUTPUT		NT_P_OUTPUT	/* Output */
  NT_P_INOUT = 0x40000,
#define NT_P_INOUT		NT_P_INOUT	/* in/out */
#define NT_P_IO_MASK		0x70000		/* mask to get i/o properties */

  NT_P_HIGHZ0 = 0x80000,
#define NT_P_HIGHZ0		NT_P_HIGHZ0	/* 0 strength of Z */
  NT_P_WEAK0 = 0x100000,
#define NT_P_WEAK0		NT_P_WEAK0
  NT_P_PULL0 = 0x200000,
#define NT_P_PULL0		NT_P_PULL0
  NT_P_STRONG0 = 0x400000,
#define NT_P_STRONG0		NT_P_STRONG0
  NT_P_SUPPLY0 = 0x800000,
#define NT_P_SUPPLY0		NT_P_SUPPLY0
#define NT_P_0_STRENGTH_MASK	0xf80000

  NT_P_HIGHZ1 = 0x1000000,
#define NT_P_HIGHZ1		NT_P_HIGHZ1
  NT_P_WEAK1 = 0x2000000,
#define NT_P_WEAK1		NT_P_WEAK1
  NT_P_PULL1 = 0x4000000,
#define NT_P_PULL1		NT_P_PULL1
  NT_P_STRONG1 = 0x8000000,
#define NT_P_STRONG1		NT_P_STRONG1
  NT_P_SUPPLY1 = 0x10000000,
#define NT_P_SUPPLY1		NT_P_SUPPLY1
#define NT_P_1_STRENGTH_MASK	0x1f000000
#define NT_P_STRENGTH_MASK	(NT_P_0_STRENGTH_MASK|NT_P_1_STRENGTH_MASK)

  NT_P_SIGNED = 0x20000000,
#define NT_P_SIGNED		NT_P_SIGNED	/* Net designated as signed */
  NT_P_SCALAR = 0x40000000,
#define NT_P_SCALAR		NT_P_SCALAR	/* Net designated as scalar */
  NT_P_VECTORED = 0x80000000
#define NT_P_VECTORED		NT_P_VECTORED	/* Net designated as vectored */
};

/*****************************************************************************
 *
 * "wire"-like net types.
 *
 *****************************************************************************/
#define NT_WIRE			NT_P_WIRE
#define NT_PARAMETER		(NT_WIRE|NT_P_PARAMETER)
#define NT_WAND			(NT_P_WIRE|NT_P_AND)
#define NT_WOR			(NT_P_WIRE|NT_P_OR)
#define NT_TRI			(NT_P_WIRE|NT_P_TRI)
#define NT_TRI1			(NT_P_WIRE|NT_P_TRI|NT_P_PULL1)
#define NT_TRI0			(NT_P_WIRE|NT_P_TRI|NT_P_PULL0)
#define NT_TRIAND		(NT_P_WIRE|NT_P_TRI|NT_P_AND)
#define NT_TRIOR		(NT_P_WIRE|NT_P_TRI|NT_P_OR)
#define NT_TRIREG		(NT_P_WIRE|NT_P_TRI|NT_P_TRIREG)
#define NT_TRIREG_SMALL		(NT_P_WIRE|NT_P_TRI|NT_P_TRIREG|NT_P_SMALL)
#define NT_TRIREG_MEDIUM	(NT_P_WIRE|NT_P_TRI|NT_P_TRIREG|NT_P_MEDIUM)
#define NT_TRIREG_LARGE		(NT_P_WIRE|NT_P_TRI|NT_P_TRIREG|NT_P_LARGE)
#define NT_INPUT		(NT_P_WIRE|NT_P_INPUT)
#define NT_INOUT		(NT_P_WIRE|NT_P_INOUT)
#define NT_OUTPUT		(NT_P_WIRE|NT_P_OUTPUT)

/*****************************************************************************
 *
 * "reg"-like net types.
 *
 *****************************************************************************/
#define NT_REG			NT_P_REG
#define NT_MEMORY		(NT_P_REG|NT_P_MEMORY)
#define NT_INTEGER		(NT_P_REG|NT_P_INTEGER)
#define NT_EVENT		(NT_P_REG|NT_P_EVENT)
#define NT_TIME			(NT_P_REG|NT_P_TIME)
#define NT_REAL			(NT_P_REG|NT_P_REAL)
#define NT_REG_OUTPUT	(NT_P_REG|NT_P_OUTPUT)
#define NT_SUPPLY0		(NT_P_REG|NT_P_SUPPLY0)
#define NT_SUPPLY1		(NT_P_REG|NT_P_SUPPLY1)

/*
 * macros for getting parts of a net type code
 */
#define NT_GETBASE(n)		((n) & NT_P_BASE_MASK)
#define NT_GETIO(n)		((n) & NT_P_IO_MASK)
#define NT_GET_0STR(n)		((n) & NT_P_0_STRENGTH_MASK)
#define NT_GET_1STR(n)		((n) & NT_P_1_STRENGTH_MASK)

/*****************************************************************************
 *
 * Net attribute flags
 *
 *****************************************************************************/
typedef enum Net_attr_flags_str {
  NA_NONE = 0x0,
#define NA_NONE         NA_NONE
  NA_SIGNED = 0x1,
#define NA_SIGNED	NA_SIGNED	/* Net value is signed */
  NA_FIXED = 0x2,
#define NA_FIXED	NA_FIXED	/* Value is fixed */
  NA_CLOCK = 0x4,
#define NA_CLOCK	NA_CLOCK	/* Net is a clock */
  NA_INPATHDMOD = 0x8 
#define NA_INPATHDMOD	NA_INPATHDMOD	/* Net is in a path-delay module */
} NetAttrFlags;

/*****************************************************************************
 *
 * SNetMap - Mappings between SuperNet bits and Net bits.
 *
 *****************************************************************************/
typedef struct {
  Net			*snm_net;		/* Net connected to SuperNet */
  int			snm_netLsb;		/* Lsb on connected net */
  int			snm_superNetLsb;	/* Lsb on SuperNet */
  int			snm_nBits;		/* Number of connected bits */
} SNetMap;

/*****************************************************************************
 *
 * SuperNet - shared bits among nets connected with a tran()
 *
 *
 *
 *****************************************************************************/
typedef struct {
  Value			sn_value;		/* List of values */
  List/*Net*/		sn_nets;		/* List of nets affecting this supernet */
  List/*SNetMap*/	sn_maps;		/* Mappings between net bits and supernet bits */
} SuperNet;

union NetData
{
	Value	value;	/* Value of net */
	Memory	memory;	/* Memory object */		 
};

/*****************************************************************************
 *
 * Net - an instantiated net
 *
 * Allowed wire types:
 *
 *    			Stat.	Stat.	Asgn.	Asgn.
 *    Type		LHS	RHS	LHS	RHS
 *    -----		-----	-----	-----	-----
 *    WIRE		ok	ok	ok
 *    REG		ok	ok		ok
 *    PARAMETER 	ok		ok
 *    MEMORY		ok	ok		ok
 *
 * Note on drivers:  All of the driver Value objects must have the same bit
 * width as the net value.  In the case of a driver that only drives part of
 * a wire as in the statment:
 *
 *   assign w[3] = a & b;
 *
 * the bits that are not addressed in the LHS are set to floating.
 *
 *****************************************************************************/
class Net
{
public:
	Net(const char *name, nettype_t ntype, unsigned msb, unsigned lsb);
	Net(const char *name, unsigned msb, unsigned lsb, unsigned beginAddr,
	    unsigned endAddr);
	~Net();
	
	char *name()
	{
		return (n_name);
	}
	nettype_t		  n_type;	/* Type of wire */
	NetAttrFlags		  n_flags;		/* Attribute flags */
	unsigned		  n_msb, n_lsb;		/* Range of net */
	unsigned		  n_nbits;		/* Number of bits */
	unsigned		  n_numMonitors;		/* Number of monitors on this net */
	List/*Trigger*/		  n_posedgeNotify;	/* Triggers to notify on posedge */
	List/*Trigger*/		  n_negedgeNotify;	/* Triggers to notify on negedge */
	int			  n_numDrivers;		/* Number of drivers (if WIRE) */
	Value			**n_drivers;	/* Drivers (if WIRE) */
	wirefunc_f		 *n_wfunc;	/* Wire function */
	NetData		  	  n_data;
private:
	char			 *n_name;		/* Full path name */
};

/*****************************************************************************
 * SNetMap member functions
 *****************************************************************************/
SNetMap *new_SNetMap(Net *net,int netLsb,int snLsb,int width);

/*****************************************************************************
 * Net member functions
 *****************************************************************************/
void Net_print(Net*,FILE*);
void Net_posedgeListen(Net*n,Trigger*t);
void Net_negedgeListen(Net*n,Trigger*t);
void Net_makeUnknown(Net *n);
void Net_posedgeNotify(Net*n);
void Net_negedgeNotify(Net*n);
int Net_addDriver(Net *n);
void Net_driverChangeNotify(Net *n, int id);
void Net_reportValue(Net *n,const char *who,const char *name,Circuit *C);
const char *Net_getLocalName(Net*);
#define Net_getDriver(n, id)	((n)->n_drivers[(id)])
#define Net_nbits(n) 		(n)->n_nbits
#define Net_getValue(n)		(&(n)->n_data.value)
#define Net_getMemory(n)	(&(n)->n_data.memory)
#define Net_getType(n)		(n)->n_type
#define Net_getMsb(n) 		(n)->n_msb
#define Net_getLsb(n) 		(n)->n_lsb
#define Net_addMonitor(n)	(n)->n_numMonitors++
#define Net_removeMonitor(n)	(n)->n_numMonitors--
#define Net_memGetFlags(n)	Memory_getFlags(&(n)->n_data.memory)
#define Net_memSetFlags(n,f)	Memory_setFlags(&(n)->n_data.memory,(f))
#define Net_memClearFlags(n,f)	Memory_clearFlags(&(n)->n_data.memory,(f))

/*****************************************************************************
 * Net value assignment member functions
 *****************************************************************************/
void Net_set(Net *n, Value*s);
void Net_setRange(Net*n,int nlsb,Value*s,int smsb,int slsb);
void Net_memSet(Net *n, unsigned addr, Value*s);
void Net_memSetRange(Net *n, unsigned addr,int nlsb,Value*s,int smsb,int slsb);

#endif
