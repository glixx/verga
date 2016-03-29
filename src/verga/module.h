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
****************************************************************************/
#ifndef __module_h
#define __module_h

#include "mitem.h"
#include "verilog.h"
#include "circuit.h"
#include "bytecode.h"

#if __cplusplus >= 201103
#include <unordered_map>
#include <string>
typedef std::unordered_map<std::string, NetDecl*> NetDeclHash;
typedef std::pair<std::string, NetDecl*> NetDeclHashElement;
#else
#include <map>
typedef std::map<const char*, NetDecl*, StringCompare> NetDeclHash;
typedef std::pair<const char*, NetDecl*> NetDeclHashElement;
#endif

#define SDF_LOCAL_ONLY	0x1	/* Lookup variable only in the local scope */

/*****************************************************************************
 *
 * ScopeDecl - Scope in which variables can be declared
 *
 *****************************************************************************/
class ScopeDecl
{
public:
	ScopeDecl()
	{
		sd_parent = NULL;
	}
	ScopeDecl	*sd_parent;	/* Parent scope */
	NetDeclHash sd_nets;		/* Net declarations */
};


/*****************************************************************************
 *
 * Scope - Scope in which instantiated variables are declared
 *
 *****************************************************************************/
class Scope
{
public:
	char		*s_path;	/* Path name of scope */
	ModuleInst	*s_instance;	/* Module instance this scope is in */
	Scope		*s_parent;	/* Parent scope */
	Scope		*s_peer;	/* "Peer" scope */
	SHash/*Net*/	 s_nets;	/* Local scope table of nets/parameters */
	SHash/*UserTask*/s_tasks;	/* User tasks */
};

/*****************************************************************************
 *
 * FaninNode - Used to build a fanin tree for a path-delay module.  Fanin nodes
 * are only used if a module contains a "specify" block with a path delay.
 *
 *****************************************************************************/
typedef struct FaninNode_str {
  ModuleItem	*fn_item;	/* Module item represented */
  char		**fn_out;	/* Names of fanouts */
  char		**fn_in;	/* Names of fanins */
  Net		**fn_outNets;	/* Nets associated with outputs */
  Net		**fn_inNets;	/* Nets associated with inputs */
  int		  fn_flag;	/* Flag used in module generation */
} FaninNode;


/*****************************************************************************
 *
 * ModuleDecl - Declaration of a module
 *
 *****************************************************************************/
class ModuleDecl
{
public:
	ModuleDecl(const char *name = NULL);
	~ModuleDecl();
	
	char *name() const
	{
		return (_name);
	}
	
	void setName(char *newVal)
	{
		_name = newVal;
	}
	
	void addPort(const char*);
	
	void printData();
	
	void print(FILE*);
	
	ScopeDecl &getScope()
	{
		return (_scope);
	}
	
	void defNet(NetDecl*);
	
	NetDecl *findNet(const char*);
	
	NetDeclHash *getNets()
	{
		return (&this->_scope.sd_nets);
	}
	
	Place			 m_place;	/* Place of declarations */
	List/*char**/		 m_ports;	/* Port names */
	List/*char**/		 m_parmPorts;	/* Parameter ports */
	SHash/*UserTaskDecl*/	 m_tasks;	/* User tasks */
	List			 m_items;	/* Module "items" */
	unsigned		 m_errorsDone;	/* We are done doing error reporting */
	Specify			*m_specify;	/* Specify block of module */
	SHash/*FaninNode*/	*m_faninnodes;	/* Fanin nodes (only used for path-delay modules) */
	Timescale		 m_timescale;	/* Timescale of this module */
private:
	/* Module name */
	char		*_name;
	/* Variable declaration scope */
	ScopeDecl	 _scope;	
};

/**
 * @brief Context information for current module being expanded
 */
class ModuleInst
{
public:
	ModuleInst(ModuleDecl*, Circuit*, ModuleInst*, const char*);
	~ModuleInst();
	
	const Circuit *circuit() const
	{	
		return (this->_circuit);
	}
	Circuit *circuit()
	{	
		return (this->_circuit);
	}
	
	void init(ModuleDecl*, Circuit*, ModuleInst*,const char *path);
	
	void setCodeBlock(CodeBlock *newVal)
	{
		this->_codeBlock = newVal;
	}
	
	char		*mc_path;	/* Path for this context */
	ModuleInst	*mc_peer;	/* Peer module (used for simulation scripts) */
	ModuleDecl	*mc_mod;	/* Module definition */
	/**
	 * @brief Parent instance
	 */
	ModuleInst	*_parent;	
	List/*VGThread*/ _threads;	/* Threads in the module instance */
	Scope		 mc_scope;	/* Scope in which varaibles/tasks are defined */
private:
	/**
	 * @brief Circuit we are building
	 */
	Circuit		*_circuit;
	/**
	 * @brief Code block for this module instance
	 */
	CodeBlock	*_codeBlock;
};

/*****************************************************************************
 *
 * Dynamic modules are used to keep track of mitems that are loaded as part
 * of a script or breakpoint so that it can be dynamically unloaded on request.
 *
 *****************************************************************************/
class DynamicModule
{
public:
	DynamicModule(const char*);
	~DynamicModule();
	
	char		*dm_name;		/* tag name of dynamic module */
	List		 dm_mitems;		/* MItems in the dynamc module */

	ModuleInst	*dm_moduleInst;		/* Module instance */
	List		 dm_threads;		/* Threads corresponding to module */
	int		 dm_aliveThreads;	/* Number of threads currently alive */
};

/*****************************************************************************
 * ScopeDecl methods
 *****************************************************************************/
void ScopeDecl_init(ScopeDecl *s,ScopeDecl *parent);
void ScopeDecl_uninit(ScopeDecl *s);
NetDecl *ScopeDecl_findNet(ScopeDecl *s,const char *name,unsigned flags);
void ScopeDecl_defNet(ScopeDecl *s,NetDecl*n);


/*****************************************************************************
 * Scope methods
 *****************************************************************************/
Scope *new_Scope(const char *path,Scope *parent,ModuleInst *mi);
void delete_Scope(Scope *s);
void Scope_init(Scope *s,const char *path,Scope *parent,ModuleInst *mi);
void Scope_uninit(Scope *s);
void Scope_setPeer(Scope *s,Scope *peer);
Net *Scope_findNet(Scope *s,const char *name,unsigned flags);
void Scope_defNet(Scope *s,const char *name,Net *n);
void Scope_replaceLocalNet(Scope *s,const char *name,Net *n);
UserTask *Scope_findTask(Scope *mi,const char *name);
int Scope_defineTask(Scope *mi,const char *name,UserTask *ut);
Value *Scope_findParm(Scope *scope,const char *name);
#define Scope_getModuleInst(s) (s)->s_instance
#define Scope_getPath(s) (s)->s_path
#define Scope_getParent(s) (s)->s_parent

/*****************************************************************************
 * ModuleDecl methods
 *****************************************************************************/
void ModuleDecl_defineParm(ModuleDecl *m,const char *name,Expr *e,int isPort);
int ModuleDecl_isParm(ModuleDecl *m,const char *name);
#define ModuleDecl_getSpecify(m) (m)->m_specify
#define ModuleDecl_getPlace(m) (&(m)->m_place)
void ModuleDecl_startSpecify(ModuleDecl*m);
int ModuleDecl_numPorts(ModuleDecl *m,nettype_t ptype);
UserTaskDecl *ModuleDecl_findTask(ModuleDecl *m,const char *name);
int ModuleDecl_defineTask(ModuleDecl *m,const char *name,UserTaskDecl *utd);
int ModuleDecl_makeFaninTree(ModuleDecl *m);
void ModuleDecl_clearFaninFlags(ModuleDecl *m);
#define ModuleDecl_findFanin(m,name) ((FaninNode*)SHash_find((m)->m_faninnodes, name))
#define ModuleDecl_isPathDelayMod(m) ((m)->m_specify && Specify_numStats((m)->m_specify) > 0)
void ModuleDecl_makeSpecify(ModuleDecl *m);
#define ModuleDecl_getTimescale(m) (&(m)->m_timescale)

/*****************************************************************************
 * ModuleInst member functions
 *****************************************************************************/
void ModuleInst_uninit(ModuleInst *mc);
Value *ModuleInst_findParm(ModuleInst *mc,const char *name);
Net *ModuleInst_findNet(ModuleInst *mc,const char *name);
void ModuleInst_defParm(ModuleInst *mc,const char *name,Value *value);
void ModuleInst_defNet(ModuleInst *mc,const char *name,Net *n);
const char *ModuleInst_findLocalNetName(ModuleInst *mc,Net *n);
#define ModuleInst_getPath(mc) (mc)->mc_path

#define ModuleInst_getModDecl(mc) (mc)->mc_mod
UserTask *ModuleInst_findTask(ModuleInst *m,const char *name);
int ModuleInst_defineTask(ModuleInst *m,const char *name,UserTask *ut);
#define ModuleInst_getScope(mc) (&(mc)->mc_scope)
#define ModuleInst_isPathDelayMod(mi) ModuleDecl_isPathDelayMod((mi)->mc_mod)
#define ModuleInst_addThread(mi, t) List_addToTail(&(mi)->_threads,(t))
#define ModuleInst_getTimescale(mi) ModuleDecl_getTimescale((mi)->mc_mod)

FaninNode *new_FaninNode(ModuleItem *item);

#endif
