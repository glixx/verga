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
#ifndef __channel_h
#define __channel_h

#include <string>

/**
 *
 * @brief Data Channel Data Structures
 *
 * Data queues are used to implement communication channels.  These are accessed
 * through the system tasks $tkg$write and $tkg$read.  It is also possible for
 * the GUI write to a channel through command $write.
 */
class Channel
{
public:
	/**
	 * @brief Constructor
	 * @param name Name for new channel
	 */
	Channel(const char*);
	~Channel();
	
	void wait(VGThread*);
	int queueLen() const;
	int read(Value *data);
	int write(Value *data);
	int setWatch(bool, const char *format);
	/**
	 * @brief Name of channel
	 */
	std::string	 _name;		
	List/*Value*/	 _queue;	/* Queue of channel */
	List/*Event*/	 c_wake;	/* Events to be executed on wake up */
	bool		 _isWatched;	/* Flag to indicated if this is a "watched" channel */
	char		*c_format;	/* Format for watched data */
};

#endif
