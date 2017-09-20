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
#include <cstdlib>

#include "verga.hpp"

/*****************************************************************************
 *
 * Create a new channel with the specified name
 *
 * Parameters:
 *     name		Name for new channel
 *
 * Returns:		Newly created channel
 *
 *****************************************************************************/
Channel::Channel(const char *name) :
_name(name)
{
	List_init(&this->_queue);
	List_init(&this->c_wake);
	this->c_isWatched = 0;
	this->c_format = NULL;
}

Channel::~Channel()
{
}

/*****************************************************************************
 *
 * Report a value from a channel to the watcher
 *
 *****************************************************************************/
static void
Channel_reportWatched(Channel *c,Value *v)
{
	EvQueue *Q = vgsim.circuit().c_evQueue;
	char buf[STRMAX];

	if (!c->c_format)
		c->c_format = strdup("%h");
	Value_format(v, c->c_format, buf);
	vgio_printf("tell $queue %s %s @ %llu\n",c->_name.c_str(), buf,
	    EvQueue_getCurTime(Q));
}

/*****************************************************************************
 *
 * Make the specified thread wait for data on a channel
 *
 * Parameters:
 *      c		Channel to wait for
 *      thread		Thread to suspend until data is ready.
 *
 *****************************************************************************/
void
Channel::wait(VGThread *thread)
{
	List_addToTail(&this->c_wake, new_EvThread(thread));
	VGThread_suspend(thread);
}

/*****************************************************************************
 *
 * Return the length of the data queue for a channel.
 *
 * Parameters:
 *      c		Channel object
 *
 *****************************************************************************/
int
Channel::queueLen() const
{
	return List_numElems(&this->_queue);
}

/*****************************************************************************
 *
 * Set a watch on a channel
 *
 * Parameters:
 *      c		Channel object
 *      isWatched	Flag to indicate if a channel should be watched.
 *      format		Format in which to report values
 *
 *****************************************************************************/
int Channel::setWatch(int isWatched, const char *format)
{
  this->c_isWatched = isWatched;
  if (this->c_format)
    free(this->c_format);
  this->c_format = format ? strdup(format) : strdup("%h");
  if (isWatched) {
    while (this->queueLen() > 0) {
      Value *v = (Value*) List_popHead(&this->_queue);
      Channel_reportWatched(this, v);
    }
  }
  return 0;
}

int
Channel::read(Value *data)
{
	Value *v;

	if (this->queueLen() == 0)
		return -1;

	v = (Value*)List_popHead(&this->_queue);

	Value_copy(data, v);
	delete_Value(v);

	return 0;
}

int Channel::write(Value *data)
{
  Value *v;

  if (this->c_isWatched) {
    Channel_reportWatched(this, data);
    return 0;
  }

  v = new_Value(Value_nbits(data));

  Value_copy(v, data);
	List_addToTail(&this->_queue, v);

#if 0
  {
    char buf[STRMAX];

    Value_getstr(data,buf);
    vgio_echo("Channel_write(%s, %s)\n",this->_name,buf);
  }
#endif

  while (List_numElems(&this->c_wake) > 0) {
    Event *e = (Event*) List_popHead(&this->c_wake);
    EvQueue_enqueueAfter(Circuit_getQueue(&vgsim.circuit()), e, 0);
  }

  return 0;
}

