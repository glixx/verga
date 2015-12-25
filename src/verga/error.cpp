/****************************************************************************
    Copyright (C) 1987-2015 by Jeffery P. Hansen

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

    Last edit by hansen on Sun Feb  8 19:30:29 2009
****************************************************************************/
#include <cstdlib>

#include "verga.hpp"

/*****************************************************************************
 *
 * When warning_mode is 2, we want to display warnings only if there were
 * errors.  This means we need to defer display of warnings until we see an
 * error.  The following structure is used for this purpose.
 *
 *****************************************************************************/
typedef struct DeferedWarning_str DeferedWarning;
struct DeferedWarning_str {
  char			*dw_msg;
  DeferedWarning	*dw_next;
};
DeferedWarning *wdefer_head = 0;
DeferedWarning *wdefer_tail = 0;

extern Place curPlace;
static placemode_t placeMode = (placemode_t)(PM_FILE|PM_LINE|PM_PRETTY);
char *current_script = 0;
int warning_mode = 3;

/*****************************************************************************
 *
 * Table of error messages.
 *
 * Each error message has a tag, and a text message associated with it.  When
 * thyme is running in interactive mode, the tags are used, and when running
 * in stand alone mode, the full messages are used.  The tags are used in interactive
 * mode to allow translation through a tkgate messages file.  The English message
 * table in messages format can be generated by running the simulator with the -e switch.
 *
 *****************************************************************************/
static ErrorDescriptor errorTable[] = {
  {ERR_OK,		0,	"OK",		"Don't worry, be happy."},
  {WRN_INPORT,		0,	"INPORT",	"Port size mismatch on input port '%s'."},
  {WRN_OUTPORT,		0,	"OUTPORT",	"Port size mismatch on output port '%s'."},
  {WRN_FLOATNET,	0,	"FLOATNET",	"Net '%s' has no drivers (floating net)."},
  {WRN_DIRECTCONN,	0,	"DIRECTCONN",	"Direct connect operator '=>' unsupported.  Treated as '*>'."},
  {ERR_MEMADDR,		1,	"MEMADDR",	"Attempt to write to memory %s with unknown address."},
  {ERR_MEMBITS,		1,	"MEMBITS",	"Attempt to write to memory %s with unknown bitrange."},
  {ERR_BADARRAYUSG,	1,	"BADARRAYUSG",	"Array '%s' used in expression without index."},
  {ERR_BADARRAYLHS,	1,	"BADARRAYLHS",	"Array '%s' used without index on left-hand-side."},
  {ERR_BADCLOSE,	1,	"BADCLOSE",	"Attempt to close non-open descriptor in task '%s'."},
  {ERR_BADAUTORNG,	1,	"BADAUTORNG",	"Auto range [*] is only valid with 'wire' declaration."},
  {ERR_BADGATERNG,	1,	"BADGATERNG",	"Bad gate instance range expression."},
  {ERR_DIVZERO,		1,	"DIVZERO",	"Divide by zero."},
  {ERR_BADEDGEEVENT,	1,	"BADEDGEEVENT",	"Event on multi-bit net '%s' can not have posedge/negedge."},
  {ERR_NEEDEDGE,	1,	"NEEDEDGE",	"Must specify posedge or negedge on event for %s."},
  {ERR_ASGNEVENT,	1,	"ASGNEVENT",	"Event wait on assign is illegal."},
  {ERR_PROTTASK,	1,	"PROTTASK",	"Execution of protected system task '%s' blocked."},
  {ERR_NEEDIDENT,	1,	"NEEDIDENT",	"Expecting identifier for argument %s of task '%s'."},
  {ERR_BADOP,		1,	"BADOP",	"Expression operator error in '%s'."},
  {ERR_NOREAD,		1,	"NOREAD",	"Failed to load source file '%s'."},
  {ERR_MEMFILE,		1,	"MEMFILE",	"Failed to open memory file '%s'."},
  {ERR_WRONGMOD,	1,	"WRONGMOD",	"Found module '%s' when expecting '%s'."},
  {ERR_NOTPARM,		1,	"NOTPARM",	"Identifier '%s' in constant expression is not a parameter."},
  {ERR_BADADDR,		1,	"BADADDR",	"Illegal address range on port '%s'."},
  {ERR_BADADDRSPEC,	1,	"BADADDRSPEC",	"Illegal address range specification '%s'."},
  {ERR_BADCHAR,		1,	"BADCHAR",	"Illegal character (%s) '%s'."},
  {ERR_BADEVENT,	1,	"BADEVENT",	"Illegal event control expression."},
  {ERR_BADCONSTOP,	1,	"BADCONSTOP",	"Illegal operator in constant expression."},
  {ERR_BADXOP,		1,	"BADXOP",	"Illegal operator in expression."},
  {ERR_NETREDEF,	1,	"NETREDEF",	"Illegal redefinition of net '%s'."},
  {ERR_BADINOUT,	1,	"BADINOUT",	"Inout connections must be net-to-net on port '%s'."},
  {ERR_MODUNDEF,	1,	"MODUNDEF",	"Instance of undefined module '%s'."},
  {ERR_BADASGNLHS,	1,	"BADASGNLHS",	"Invalid left-hand-side in 'assign'."},
  {ERR_LHSNOTREG,	1,	"LHSNOTREG",	"Illegal use of '%s' in left-hand-side of assignment."},
  {ERR_BADLHS,		1,	"BADLHS",	"Invalid left-hand-side in assignment."},
  {ERR_BADOUT,		1,	"BADOUT",	"Invalid output assignment."},
  {ERR_NOTREG,		1,	"NOTREG",	"Memories must be declared as register."},
  {ERR_PORTMIX,		1,	"PORTMIX",	"Mixed named and unnamed ports on interface '%s' of '%s'."},
  {ERR_REPCASE,		1,	"REPCASE",	"More than one default: in case statement."},
  {ERR_NOMEM,		1,	"NOMEM",	"No current memory in memory file read."},
  {ERR_BADCMD,		1,	"BADCMD",	"No such command '%s'."},
  {ERR_CMDNOTNET,	1,	"CMDNOTNET",	"No such net '%s' in '%s' command."},
  {ERR_MEMNONBLK,	1,	"MEMNONBLK",	"Non-blocking assignments to memories not implemented."},
  {ERR_NOTPPORT,	1,	"NOTPPORT",	"Parameter '%s' is not declared as a port."},
  {ERR_REDEFP,		1,	"REDEFP",	"Parameter redefines identifier '%s'."},
  {ERR_PORTNOTDEF,	1,	"PORTNOTDEF",	"Port '%s' on interface '%s' is not defined in module '%s'."},
  {ERR_NOCONN,		1,	"NOCONN",	"Port '%s' has no connections on interface '%s' of '%s'."},
  {ERR_MULTCONN,	1,	"MULTCONN",	"Port '%s' has multiple connections on interface '%s' of '%s'."},
  {ERR_PORTCOUNT,	1,	"PORTCOUNT",	"Port count does not match definition on interface '%s' of '%s'."},
  {ERR_BADPRTRANGE,	1,	"BADPRTRANGE",	"Range on port '%s' is not numeric."},
  {ERR_BADARRAYRNG,	1,	"BADARRAYRNG",	"Range specification not allowed for memory reference of '%s'."},
  {ERR_CMDMODREDEF,	1,	"CMDMODREDEF",	"Redefinition of dynamic module '%s'."},
  {ERR_REDEF,		1,	"REDEF",	"Redefinition of identifier '%s'."},
  {ERR_PROTTASKSTOP,	1,	"PROTTASKSTOP",	"Simulation stopped on attempted execution of protected system task '%s'."},
  {ERR_NOTMEM,		1,	"NOTMEM",	"Specified net '%s' is not a memory."},
  {ERR_CMDNOTMEM,	1,	"CMDNOTMEM",	"Specified net '%s' is not a memory in '%s' command."},
  {ERR_SYNTAX,		1,	"SYNTAX",	"Syntax error."},
  {ERR_BADHIER,		1,	"BADHIER",	"Hierarchical variable '%s' referenced in illegal context."},
  {ERR_TOOFEWPP,	1,	"TOOFEWPP",	"Too few parameter ports on instance %s."},
  {ERR_OPENTOOMANY,	1,	"OPENTOOMANY",	"Too many files open in task '%s'."},
  {ERR_TOOMANYPP,	1,	"TOOMANYPP",	"Too many parameter ports on instance %s."},
  {ERR_NOTOP,		1,	"NOTOP",	"Top-module '%s' not defined."},
  {ERR_BADOPEN,		1,	"BADOPEN",	"Unable to open output file '%s' in task '%s'."},
  {ERR_CMDNOMOD,	1,	"CMDNOMOD",	"Undefined dynamic module '%s' in '%s' command."},
  {ERR_BADEVENTNET,	1,	"BADEVENTNET",	"Undefined net '%s' in event control expression."},
  {ERR_NOTASK,		1,	"NOTASK",	"Undefined task '%s'."},
  {ERR_NOTDEF,		1,	"NOTDEF",	"Undefined variable '%s'."},
  {ERR_BADRANGE,	1,	"BADRANGE",	"Unsupported bit range [%s] on net %s (must be of form [n:0])."},
  {ERR_GATEUNIMP,	1,	"GATEUNIMP",	"Unimplemented primitive component type on instance '%s'."},
  {ERR_USAGE,		1,	"USAGE",	"Usage: verga [options][files...]"},
  {ERR_CLSDWRITE,	1,	"CLSDWRITE",	"Write to closed descriptor."},
  {ERR_PRIMPTCOUNT,	1,	"PRIMPTCOUNT",	"Wrong number of ports on primitive gate instance '%s'."},
  {ERR_CMDARGS,		1,	"CMDARGS",	"Wrong number of arguments in '%s' command."},
  {ERR_YYERROR,		1,	"YYERROR",	"YYError - %s."},
  {ERR_TASKARGS,	1,	"TASKARGS",	"Task '%s' called with wrong number of arguments."},
  {ERR_BADSTART,	1,	"BADSTART",	"Illegal start value in task '%s'."},
  {ERR_BADSTOP,		1,	"BADSTOP",	"Illegal stop value in $readmemb."},
  {ERR_SPECTASKUSG,	1,	"SPECTASKUSG",	"Task %s must be used in a specify block."},
  {ERR_BADSPECTASK,	1,	"BADSPECTASK",	"Task %s can not be used in a specify block."},
  {ERR_TIMING,		1,	"TIMING",	"Timing violation in %s[%s] %s."},
  {ERR_NOIFDEF,		1,	"NOIFDEF",	"No matching `ifdef/`ifndef for %s declaration."},
  {ERR_BADSPECLVAL,	1,	"BADSPECLVAL",	"Bit-ranges on path delay specifiers unsupported."},
  {ERR_PATHDMEM,	1,	"PATHDMEM",	"Illegal declaration of memory '%s' in path-delay module - unsupported."},
  {ERR_PATHDINOUT,	1,	"PATHDINOUT",	"Use of 'inout' in module with path-delay specification is unsupported."},
  {ERR_PATHDLOOP,	1,	"PATHDLOOP",	"Loops in path-delay modules are unsupported."},
  {ERR_PATHDREASSGN,	1,	"PATHDREASSGN",	"Multiple assignment of net '%s' in path-delay module is unsupported."},
  {ERR_PATHDNOTIN,	1,	"PATHDNOTIN",	"Net '%s' has no driver and is not an input in path-delay module."},
  {ERR_PATHDCOND,	1,	"PATHDCOND",	"Delay and trigger expressions not allowed in path-delay modules."},
  {ERR_TASKREDEF,	1,	"TASKREDEF",	"Redefinition of task or function '%s' in module '%s'."},
  {ERR_TASKASFUNC,	1,	"TASKASFUNC",	"Task '%s' used as function."},
  {ERR_FUNCASTASK,	1,	"FUNCASTASK",	"Function '%s' used as task."},
  {ERR_TASKBADTYPE,	1,	"TASKBADTYPE",	"Non-register type used in task or function."},
  {ERR_TASKBADPORT,	1,	"TASKBADPORT",	"Only input ports are allowed on functions."},
  {ERR_TIMESCALEU,	1,	"TIMESCALEU",	"Invalid units '%s' in `timescale declaration."},
  {ERR_TIMESCALEN,	1,	"TIMESCALEN",	"Invalid scale '%s' in `timescale declaration (must be 1, 10 or 100)."},
  {ERR_TIMESCALES,	1,	"TIMESCALES",	"Invalid syntax in `timescale declartion."},
  {ERR_TIMESCALEX,	1,	"TIMESCALEX",	"Units must be larger than precision in `timescale declartion."},
  {ERR_TIMESCALEAN,	1,	"TIMESCALEAN",	"Design contains some modules with `timescale and some without."},
  {ERR_PARAMMIX,	1,	"PARAMMIX",	"Mixed named and unnamed parameters on interface '%s' of '%s'."},
  {ERR_BADPORTTYPE,	1,	"BADPORTTYPE",	"Invalid type declaration used on port '%s'."},
  {ERR_BADVALUE,	1,	"BADVALUE",	"Bad numeric value '%s'."},
  {ERR_NONEXPCTL,	1,	"NONEXPCTL",	"Event control @(*) applied to non-expression."},
  {ERR_NONSTATCTL,	1,	"NONSTATCTL",	"Event control @(*) applied to non-statement."},

  {ERR_MODREDEF,	1,	"MODREDEF",	"Redefinition of module '%s'."},
  {ERR_IE_TASK,		1,	"IE_TASK",	"Task definition '%s' found outside module - internal error."},
  {ERR_IE_NONET,	1,	"IE_NONET",	"Failed to find net '%s' - internal error."},
  {ERR_IE_NOOP,		1,	"IE_NOOP",	"Can not find operator description -- internal error."},
  {ERR_IE_BADEXP,	1,	"IE_BADEXP",	"Unexpected expression type %s - internal error."},
  {ERR_IE_BADVAR,	1,	"IE_BADVAR",	"Undefined variable or unknown net - internal error."},
  {ERR_IE_BADSTATE,	1,	"IE_BADSTATE",	"Unexpected internal state at %s  - internal error."},
  {ERR_IE_RETURN,	1,	"IE_RETURN",	"Executed BCReturn bytecode with empty return stack - internal error."},
  {ERR_IE_UNIMP,	1,	"IE_UNIMP",	"Unimplemented feature."},
};
static int errorTableLen = sizeof(errorTable)/sizeof(errorTable[0]);

static ErrorDescriptor unknownError = {
  ERR_UNKNOWN,		1,	"UNKNOWN",	"Unknown error message."
};

#define EL_SCRIPTERR 2
static char const *errLevelText[] = {"warning", "error", "scripterror"};
static char const *errLevelTextCap[] = {"Warning", "Error", "ScriptError"};

/*****************************************************************************
 *
 * Array for used to temporarily store arguments for an error message.  The
 * array is initialized by initErrorMessages() and has a length equal to the
 * maximum number of arguments in any error message in the error message table.
 *
 *****************************************************************************/
static char **errArgs;

/*****************************************************************************
 *
 * Initialize the error table by counting the number of arguments to each message.
 *
 *****************************************************************************/
void initErrorMessages()
{
  int i;
  int maxCount = 0;

  for (i = 0;i < errorTableLen;i++) {
    ErrorDescriptor *ed = &errorTable[i];
    char const *p;
    int count = 0;


    for (p = ed->ed_text;*p;p++) {
      if (*p == '%') {
	if (p[1] == '%')
	  p++;
	else
	  count++;
      }
    }

    if (count > maxCount) maxCount = count;

    ed->ed_numArgs = count;
  }

  errArgs = (char**)malloc(sizeof(char*)*maxCount);
}

void flushDefered()
{
  while (wdefer_head) {
    DeferedWarning *dw = wdefer_head;
    wdefer_head = wdefer_head->dw_next;
    free(dw->dw_msg);
    free(dw);
  }

  wdefer_head = 0;
  wdefer_tail = 0;
}

void flushErrors()
{
  extern int errCount;
  extern int warnCount;

  errCount = 0;
  warnCount = 0;
  flushDefered();
}

void deferWarning(const char *msg)
{
  DeferedWarning *dw = (DeferedWarning*) malloc(sizeof(DeferedWarning));

  dw->dw_msg = strdup(msg);
  dw->dw_next = 0;

  if (wdefer_tail) {
    wdefer_tail->dw_next = dw;
    wdefer_tail = dw;
  } else {
    wdefer_head = wdefer_tail = dw;
  }
}

void reportDeferedWarnings()
{
  DeferedWarning *dw;

  for (dw = wdefer_head;dw;dw = dw->dw_next)
    vgio_printf("%s\n",dw->dw_msg);
  flushDefered();
}

/*****************************************************************************
 *
 * Dump the list of error messages in tkgate messages format.
 *
 *****************************************************************************/
void dumpErrorMessages()
{
  int i;

  for (i = 0;i < errorTableLen;i++) {
    ErrorDescriptor *ed = &errorTable[i];
    int l = strlen(ed->ed_tag) + 10;

    printf("verga.err.%s",ed->ed_tag);

    l = l/8 + 1;

    for (;l < 4;l++)
      printf("\t");

    printf("\t%s\n",ed->ed_text);
  }
}

/*****************************************************************************
 *
 * Find the error message descriptor for the error with the given code.
 *
 * Parameters:
 *      ecode		Code number of message to find.
 *
 * Returns:		Error descriptor corresponding to the given error code.
 *			Returns a special "unknown" error descriptor if the
 *			specified code is not found.
 *
 *****************************************************************************/
static ErrorDescriptor *findError(errorcode_t ecode)
{
  int i;

  for (i = 0;i < errorTableLen;i++)
    if (errorTable[i].ed_code == ecode)
      return &errorTable[i];

  return &unknownError;
}
/*
Place::Place(const char *fileName)
{
	this->init(fileName);
}*/

/*****************************************************************************
 *
 * Initialize the a place to the beginning of a file.
 *
 * Parameters:
 *      p			Place to be initialized
 *      fileName		Name of file.
 *
 *****************************************************************************/
void
Place::init(const char *fileName)
{
	this->p_fileName = fileName ? strdup(fileName) : 0;
	this->p_moduleName = 0;
	this->p_mitem = 0;
	this->p_lineNo = 1;
	this->p_modLineNo = 0;
	this->p_mtag = 0;
}

/*****************************************************************************
 *
 * Copy a place.
 *
 * Parameters:
 *      dstP			Destination place
 *      srcP			Source place
 *
 *****************************************************************************/
void Place_copy(Place *dstP, Place *srcP)
{
	*dstP = *srcP;
}

/*****************************************************************************
 *
 * Increment the line number of a place
 *
 * Parameters:
 *      p			Place to be updated
 *      delta			Number of lines to add to place.
 *
 *****************************************************************************/
void Place_incLineno(Place *p, int delta)
{
	p->p_lineNo += delta;
	p->p_modLineNo += delta;
}

/*****************************************************************************
 *
 * Indicate current place is in a module
 *
 * Parameters:
 *      p			Place to be updated
 *      modName			Name of module
 *
 *****************************************************************************/
void Place_startModule(Place *p, const char *modName)
{
  if (!p->p_mtag) {
    p->p_moduleName = modName ? strdup(modName) : 0;
    p->p_modLineNo = 1;
  }
}

/*****************************************************************************
 *
 * Indicate current place is outside a module
 *
 * Parameters:
 *      p			Place to be updated
 *
 *****************************************************************************/
void
Place::endModule()
{
	if (!this->p_mtag) {
		this->p_moduleName = 0;
		this->p_modLineNo = 1;
	}
}

/*****************************************************************************
 *
 * Indicate current place is inside a tag defined module
 *
 * Parameters:
 *      p			Place to be updated
 *
 *****************************************************************************/
void Place_startMTag(Place *p, const char *modName)
{
  p->p_moduleName = modName ? strdup(modName) : 0;
  p->p_modLineNo = 1;
  p->p_mtag = 1;
}

/*****************************************************************************
 *
 * Indicate current place is outside a tag defined module
 *
 * Parameters:
 *      p			Place to be updated
 *
 *****************************************************************************/
void Place_endMTag(Place *p)
{
  p->p_moduleName = 0;
  p->p_modLineNo = 1;
  p->p_mtag = 0;
}

/*****************************************************************************
 *
 * Generate string encoding a "place".
 *
 * Parameters:
 *     p		Place to encode
 *     etype		Error message type
 *     s		Buffer to write encoding to.
 *
 * Returns:		Pointer to end of encoded string in buffer.
 *
 *
 *****************************************************************************/
char *Place_report(Place *p, const char *etype, const char *netName,char *s)
{
  int did_first = 0;

  if ((placeMode & PM_PRETTY)) {
    if (strcmp(etype,"warning") == 0)
      s += sprintf(s,"Warning: ");

    *s++ = '<';
    if ((placeMode & PM_FILE)) {
      s += sprintf(s, "\"%s\"",p->p_fileName);
      did_first = 1;
    }
    if ((placeMode & PM_LINE)) {
      if (did_first) s += sprintf(s, ", ");
      s += sprintf(s, "%d",p->p_lineNo);
      did_first = 1;
    }
    if ((placeMode & PM_MODULE)) {
      if (did_first) s += sprintf(s, ", ");
      s += sprintf(s, "[%s]",(p->p_moduleName ? p->p_moduleName : "-"));
    }
    if ((placeMode & PM_MODLINE))
      s += sprintf(s, "+%d",p->p_modLineNo);

    *s++ = '>';
    *s++ = ' ';
  } else {
    s += sprintf(s,"%s file",etype);
    if ((placeMode & PM_FILE))
      s += sprintf(s, " %s",p->p_fileName);
    if ((placeMode & PM_LINE))
      s += sprintf(s, " %d",p->p_lineNo);
    if ((placeMode & PM_MODULE))
      s += sprintf(s, " %s",(p->p_moduleName ? p->p_moduleName : "-"));
    if ((placeMode & PM_MODLINE))
      s += sprintf(s, " %d",p->p_modLineNo);
    if (p->p_mitem && ModuleItem_getType(p->p_mitem) == IC_INSTANCE && !(placeMode & PM_PRETTY))
      s += sprintf(s," %s",p->p_mitem->mi_inst.mii_instName);
    else
      s += sprintf(s," -");

    if (netName)
      s += sprintf(s," %s",netName);
    else
      s += sprintf(s," -");

    s += sprintf(s," : ");
  }

  *s = 0;
  return s;
}

/*****************************************************************************
 *
 * Set the mode for place display
 *
 * Parameters:
 *      pm		Place display mode.
 *
 *****************************************************************************/
void Place_setMode(placemode_t pm)
{
  placeMode = pm;
}

/*****************************************************************************
 *
 * Reset the module line number
 *
 * Parameters:
 *      p		Place to update
 *
 *****************************************************************************/
void Place_resetModLine(Place *p)
{
  p->p_modLineNo = 0;
}


/*****************************************************************************
 *
 * Set the "current" place.
 *
 * Parameters:
 *      p		Place to use as current place
 *
 *****************************************************************************/
void Place_setCurrent(Place *p)
{
  curPlace = *p;
}

/*****************************************************************************
 *
 * Get the "current" place.
 *
 *****************************************************************************/
Place *
Place::getCurrent()
{
	return (&curPlace);
}

/*****************************************************************************
 *
 * Report built-in parser errors
 *
 * Parameters:
 *      err		Text of error message
 *
 * This function must be supported by programs using yacc.  It is only called
 * from within built-in yacc code.  The most common message is "syntax error"
 * and if we see this message, we translate it to ERR_SYNTAX to allow localization
 * if necessary.
 *
 *****************************************************************************/
int
yyerror(char *err)
{
	if (strncmp(err,"syntax",6) == 0)
		errorFile(Place::getCurrent(), ERR_SYNTAX);
	else
		errorFile(Place::getCurrent(), ERR_YYERROR,err);

	return (0);
}

/*****************************************************************************
 *
 * Marshal a string so that it can be read as a tcl string.
 *
 * Parameters:
 *      p		Marshaled string
 *      s		String to be marshaled
 *
 *****************************************************************************/
static int marshalString(char *p,char *s)
{
  char *start = p;		// Remeber start of marshaled string

  /* Leading quote */
  *p++ = '"';

  for (;*s;s++) {
    if (strchr("\\\"",*s))
      *p++ = '\\';
    *p++ = *s;
  }

  /* Trailing quote */
  *p++ = '"';
  return p-start;
}

/*****************************************************************************
 *
 * Format an error message
 *
 *****************************************************************************/
static int
formatErrorMsg(char *p,ErrorDescriptor *ed,va_list ap)
{
	if (vgsim.interactive()) {
		int i;
		char *q = p;

		p += sprintf(p,"`%s",ed->ed_tag);
		for (i = 0;i < ed->ed_numArgs;i++) {
			p += sprintf(p," ");
			p += marshalString(p,va_arg(ap,char*));
		}
		*p = 0;
		return (p-q);
	} else
		return (vsprintf(p,ed->ed_text,ap));

	return (0);
}

/*****************************************************************************
 *
 * Handle the output of error (or warning) message text.
 *
 * Parameters:
 *      ed			Error type descriptor
 *      msg			Text of the error message.
 *
 *****************************************************************************/
static void doErrorOutput(ErrorDescriptor *ed,const char *msg)
{
  extern int errCount;
  extern int warnCount;

  if (ed->ed_level > 0) {
    errCount++;
    reportDeferedWarnings();		/* Report any defered warnings */
    vgio_printf("%s\n",msg);
  } else {
    if (warning_mode > 1) {
      warnCount++;
      if (warning_mode == 2 && errCount == 0)
	deferWarning(msg);
      else
	vgio_printf("%s\n",msg);
    }
  }

}

/*****************************************************************************
 *
 * Generate a command error
 *
 * Parameters:
 *      ecode			Error code value
 *      ...			Error-specific arguments
 *
 *****************************************************************************/
void
errorCmd(errorcode_t ecode,...)
{
	ErrorDescriptor *ed = findError(ecode);
	char buf[STRMAX],*p;
	va_list ap;
	extern int errCount;

	p = buf;
	if (vgsim.interactive())
		p += sprintf(p,"%s command ",errLevelText[ed->ed_level]);
	else
		p += sprintf(p,"Command %s: ",errLevelTextCap[ed->ed_level]);

	va_start(ap,ecode);
	p += formatErrorMsg(p,ed,ap);
	va_end(ap);

	doErrorOutput(ed,buf);

	errCount++;
}

/*****************************************************************************
 *
 * Generate a runtime error
 *
 * Parameters:
 *      ecode			Error code value
 *      ...			Error-specific arguments
 *
 *****************************************************************************/
void
errorRun(errorcode_t ecode,...)
{
	ErrorDescriptor *ed = findError(ecode);
	char buf[STRMAX],*p;
	va_list ap;
	EvQueue *Q = Circuit_getQueue(&vgsim.circuit());
	simtime_t curTime = EvQueue_getCurTime(Q);

	p = buf;
	if (vgsim.interactive())
		p += sprintf(p,"%s run %llu : ",errLevelText[ed->ed_level],curTime);
	else
		p += sprintf(p,"Runtime %s at %llu: ",errLevelTextCap[ed->ed_level],curTime);

	va_start(ap,ecode);
	p += formatErrorMsg(p,ed,ap);
	va_end(ap);

	doErrorOutput(ed,buf);
}

/*****************************************************************************
 *
 * Generate a file error
 *
 * Parameters:
 *      place			Place identifier for location of the error.
 *      ecode			Error code value
 *      ...			Error-specific arguments
 *
 *****************************************************************************/
void errorFile(Place *place,errorcode_t ecode,...)
{
  ErrorDescriptor *ed = findError(ecode);
  char message[STRMAX],*p;
  va_list ap;
  extern int errCount;
  extern int warnCount;

  p = message;

  if (current_script)
    p += sprintf(p,"scripterror %s %s %d : ",current_script,place->p_fileName,place->p_lineNo+1);
  else
    p = Place_report(place,errLevelText[ed->ed_level],0,message);

  va_start(ap,ecode);
  p += formatErrorMsg(p,ed,ap);
  va_end(ap);
  doErrorOutput(ed,message);

#if 0
  if (!script_error_reporting) {
    p = Place_report(place,errLevelText[ed->ed_level],0,message);
    va_start(ap,ecode);
    p += formatErrorMsg(p,ed,ap);
    va_end(ap);
    doErrorOutput(ed,message);
  } else {
    if (ed->ed_level > 0)
      errCount++;
    else
      warnCount++;
  }
#endif
}

/*****************************************************************************
 *
 * Generate a module error
 *
 * Parameters:
 *      m			Module in which error occured.
 *      place			Place identifier for location of the error.
 *      ecode			Error code value
 *      ...			Error-specific arguments
 *
 *****************************************************************************/
void errorModule(ModuleDecl *m,Place *place,errorcode_t ecode,...)
{
  ErrorDescriptor *ed = findError(ecode);
  char buf[STRMAX],*p;
  va_list ap;

  if (m->m_errorsDone) return;

  p = Place_report(place,errLevelText[ed->ed_level],0,buf);

  va_start(ap,ecode);
  p += formatErrorMsg(p,ed,ap);
  va_end(ap);

  doErrorOutput(ed,buf);
}

/*****************************************************************************
 *
 * Generate a net error
 *
 * Parameters:
 *      m			Module in which error occured.
 *      place			Place identifier for location of the error.
 *      ecode			Error code value
 *      ...			Error-specific arguments
 *
 *****************************************************************************/
void errorNet(ModuleDecl *m,const char *net,Place *place,errorcode_t ecode,...)
{
  ErrorDescriptor *ed = findError(ecode);
  char buf[STRMAX],*p;
  va_list ap;

  if (m->m_errorsDone) return;

  p = Place_report(place,errLevelText[ed->ed_level],net,buf);

  va_start(ap,ecode);
  p += formatErrorMsg(p,ed,ap);
  va_end(ap);

  doErrorOutput(ed,buf);
}
