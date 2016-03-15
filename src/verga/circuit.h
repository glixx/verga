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
typedef std::unordered_map<const char*, Channel*> ChannelHash;
typedef std::unordered_map<const char*, ModuleInst*> ModuleInstHash;
#else
#include <map>
typedef std::map<std::string, Net*> NetHash;
typedef std::map<const char*, Channel*, StringCompare> ChannelHash;
typedef std::map<const char*, ModuleInst*, StringCompare> ModuleInstHash;
#endif
typedef std::pair<std::string, Net*> NetHashElement;
typedef std::pair<const char*, Channel*> ChannelHashElement;
typedef std::pair<const char*, ModuleInst*> ModuleInstHashElement;

/*****************************************************************************
 *
 * Circuit - Class to represent final instantiated circuit
 *
 *****************************************************************************/
class Circuit
{
public:
	Circuit();
	~Circuit();
	
	void build(ModuleDecl*);
	void run();
	void check();
	void sortThreads();
	ModuleInst &root()
	{
		assert(c_root != NULL);
		return (*c_root);
	}
	
	NetHash		 c_nets;	/* Global table of nets */
	NHash/*Trigger*/ c_triggers;		/* Triggers in this circuit */
	ChannelHash	 c_channels;		/* Communication channels */
	ModuleInstHash	 c_moduleInsts;		/* Module instances */
	EvQueue		*c_evQueue;		/* Global event queue */
	/* Dynamicly loaded modules */
	SHash		 c_dynamicModules;
private:
	ModuleInst		*c_root;		/* Root module instance */
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
//void Circuit_build(Circuit *c,ModuleDecl *m);
void Circuit_buildPathDelayMod(Circuit *c,ModuleInst *mi,ModuleInst *parent,char *path);
void Circuit_installScript(Circuit *c,ModuleDecl *m,DynamicModule *dm);
Net *Circuit_findNet(Circuit *c,const char *name);
Net *Circuit_findMemoryNet(Circuit *c,const char *name);
ModuleInst *Circuit_findModuleInst(Circuit *c, const char *name);
Trigger *Circuit_getTrigger(Circuit *c,List *posedge,List *negedge);
Trigger *Circuit_getNetTrigger(Circuit *c,Net*,transtype_t);
#define Circuit_getQueue(c) (c)->c_evQueue
#define Circuit_update(c) EvQueue_update((c)->c_evQueue)
void Circuit_exec(Circuit*c,char *cmd);
void Circuit_readMemory(Circuit *c, const char *fileName, Net *net, unsigned start, unsigned stop, unsigned flags);
int Circuit_writeMemory(Circuit *c, const char *fileName, Net *net, unsigned start, unsigned stop, unsigned flags);
Channel *Circuit_getChannel(Circuit *c, const char*name);
void Circuit_unloadDynamicModule(Circuit *c,DynamicModule *dm);
void Circuit_enableDynamicModule(Circuit *c,DynamicModule *dm);
void Circuit_disableDynamicModule(Circuit *c,DynamicModule *dm);
void Circuit_declareNet(Circuit *c,Scope *scope,NetDecl *nd,ModuleDecl *md,Place *place);
void Circuit_execScript(Circuit*c,int argc,char *argv[]);
void Circuit_finishModuleInst(Circuit *c, ModuleInst *mi, CodeBlock *codeBlock);
Scope *Circuit_getUpScope(Circuit *c,Scope *s);

#endif
