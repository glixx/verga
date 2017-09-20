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

    Last edit by hansen on Sun Dec  7 20:20:00 2008
****************************************************************************/
#ifndef __circuit_h
#define __circuit_h

#include "module.h"
#include "channel.h"

#include <cassert>
#include <string>

#if __cplusplus >= 201103
#include <unordered_map>
typedef std::unordered_map<std::string, Net*> NetHash;
typedef std::unordered_map<std::string, Channel*> ChannelHash;
typedef std::unordered_map<std::string, ModuleInst*> ModuleInstHash;
#else
#include <map>
typedef std::map<std::string, Net*> NetHash;
typedef std::map<std::string, Channel*> ChannelHash;
typedef std::map<std::string, ModuleInst*> ModuleInstHash;
#endif
typedef std::pair<std::string, Net*> NetHashElement;
typedef std::pair<std::string, Channel*> ChannelHashElement;
typedef std::pair<std::string, ModuleInst*> ModuleInstHashElement;

/**
 * @brief Class to represent final instantiated circuit
 */
class Circuit
{
public:
	
	Circuit();
	
	~Circuit();
	/**
	 * @brief Execute an interactive command from tkgate
	 * 
	 * See README file for a summary of command recognized by the simulator.
	 * 
	 * @param cmd Command to execute (assumed in-place modifiable)
	 */
	void exec(char*);
	
	void build(ModuleDecl*);
	/**
	 * @brief Begin simulating a circuit
	 */
	void run();
	/**
	 * @brief Check a circuit for potential problems
	 *
	 * This function does a final check for errors/warnings in the circuit.  The
	 * major check is for floating wires (wires with no driving signal).  When
	 * reporting floating nets, we need to be sure to report them only once for
	 * each module type, and not for each module instance.  We do this by reporting
	 * the error in the first instance of a type that we find.
	 */
	void check();
	
	void sortThreads();
	
	Channel *channel(const char*);
	
	ModuleInst *findModuleInst(const char *name);
	/**
	 * @brief Get the scope for the next higher context.
	 * 
	 * @return Scope instance or null if this is a top-level scope.
	 */
	Scope *getUpScope(Scope*);
	
	void finishModuleInst(ModuleInst *mi, CodeBlock *codeBlock);
	
	void installScript(ModuleDecl *m, DynamicModule *dm);
	
	ModuleInst &root()
	{
		assert(_root != NULL);
		
		return (*_root);
	}
	
	NetHash		 c_nets;		/* Global table of nets */
	NHash/*Trigger*/ c_triggers;		/* Triggers in this circuit */
	EvQueue		*c_evQueue;		/* Global event queue */
	/**
	 * @brief Dynamicly loaded modules
	 */
	SHash		 c_dynamicModules;
private:
	ModuleInst *buildNets(ModuleDecl*, MIInstance*, ModuleInst*, char*);
	void buildHier(ModuleInst *mi,ModuleInst *parent,char *path);
	int buildHierInstance(ModuleDecl*, ModuleInst*, MIInstance*, ModuleInst*,
	    char*, CodeBlock*);
	/**
	 * @brief Communication channels
	 */
	ChannelHash	 _channels;
	/**
	 * @brief Root module instance
	 */
	ModuleInst		*_root;
	/**
	 * @brief Module instances
	 */
	ModuleInstHash	 _moduleInsts;
};

/*****************************************************************************
 * DynamicModule member functions
 *****************************************************************************/
void DynamicModule_addMItem(DynamicModule*,ModuleItem*);
void DynamicModule_addThread(DynamicModule*,VGThread*);
#define DynamicModule_setModuleInst(dm,mi) ((dm)->dm_moduleInst = (mi))
void DynamicModule_killNotify(DynamicModule*);

/*****************************************************************************
 * Circuit member functions
 *****************************************************************************/
void Circuit_buildPathDelayMod(Circuit *c,ModuleInst *mi,ModuleInst *parent,char *path);
Net *Circuit_findNet(Circuit *c,const char *name);
Net *Circuit_findMemoryNet(Circuit *c,const char *name);
Trigger *Circuit_getTrigger(Circuit *c,List *posedge,List *negedge);
Trigger *Circuit_getNetTrigger(Circuit *c,Net*,transtype_t);
#define Circuit_getQueue(c) (c)->c_evQueue
#define Circuit_update(c) EvQueue_update((c)->c_evQueue)
void Circuit_readMemory(Circuit *c, const char *fileName, Net *net, unsigned start, unsigned stop, unsigned flags);
int Circuit_writeMemory(Circuit *c, const char *fileName, Net *net, unsigned start, unsigned stop, unsigned flags);

void Circuit_unloadDynamicModule(Circuit *c,DynamicModule *dm);
void Circuit_enableDynamicModule(Circuit *c,DynamicModule *dm);
void Circuit_disableDynamicModule(Circuit *c,DynamicModule *dm);
void Circuit_declareNet(Circuit *c,Scope *scope,NetDecl *nd,ModuleDecl *md,Place *place);
void Circuit_execScript(Circuit*c,int argc,char *argv[]);

#endif
