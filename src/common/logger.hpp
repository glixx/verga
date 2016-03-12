/****************************************************************************
    Copyright (C) 1987-2015 by Jeffery P. Hansen
    Copyright (C) 2015,2016 by Andrey V. Skvortsov

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

#ifndef LOGGER_HPP
#define LOGGER_HPP

/*
 * Following macros control the logging subsystem:
 *     NO_DEBUG_LOGGING - switch off the debug log
 *     NO_INFO_LOGGING -  switch off the info log
 *     NO_ERROR_LOGGING -  switch off the errors log
 *     NO_LOGGING - completely disable logging
 *     CPS_LOG_TRACE_INOUT - enable the option, when PUT_LOG_CALL_TRACE
 *         will output both enter and exit of the function/method
 */

#ifndef CPS_LOGGER_HPP
#define	CPS_LOGGER_HPP

#include <ostream>
#include <cassert>

#ifdef NO_LOGGING

#ifndef NO_DEBUG_LOGGING
#define NO_DEBUG_LOGGING
#endif

#ifndef NO_INFO_LOGGING
#define NO_INFO_LOGGING
#endif

#ifndef NO_ERROR_LOGGING
#define NO_ERROR_LOGGING
#endif

#endif

#if __cplusplus >= 201103L

#include "posix_threads_mutex.hpp"
#include "posix_threads_criticalsection.hpp"
#include "posix_time_clock.hpp"
#include "posix_time_timespec.hpp"

#ifndef NO_INFO_LOGGING
#define PUT_LOG_INFO(...) cps::Logger::log(cps::Logger::Target_t::INFO, __VA_ARGS__)
#else
#define PUT_LOG_INFO(...)
#endif

#ifndef NO_DEBUG_LOGGING
#define PUT_LOG_DEBUG(...) cps::Logger::log(cps::Logger::Target_t::DEBUG, __VA_ARGS__)
#else
#define PUT_LOG_DEBUG(...)
#endif

#ifndef NO_ERROR_LOGGING
#define PUT_LOG_ERROR(...) cps::Logger::log(cps::Logger::Target_t::ERROR, __VA_ARGS__)
#else
#define PUT_LOG_ERROR(...)
#endif

/**
 * Макрос PUT_LOG_CALL_TRACE выводит в лог отладки сигнатуру функции в которой
 * вызван с наивысшим требованием к подробности лога
 */
#ifdef CPS_LOG_TRACE_INOUT
#define PUT_LOG_CALL_TRACE cps::Logger::_SubprogramTracer __subprogram_tracer__(__PRETTY_FUNCTION__)
#else
#define PUT_LOG_CALL_TRACE PUT_LOG_DEBUG(cps::Logger::Verbosity_t::VERBOSE, \
	__PRETTY_FUNCTION__)
#endif
// Неотключаемые макросы вывода
#define PUT_LOG_CRITICAL_INFO(...) cps::Logger::log(cps::Logger::Target_t::INFO, __VA_ARGS__)
#define PUT_LOG_CRITICAL_ERROR(...) cps::Logger::log(cps::Logger::Target_t::ERROR, __VA_ARGS__)

using namespace posix::time;

namespace cps
{

/**
 * @class Logger
 * @brief Класс компонента логгирования работы приложений
 */
class Logger
{
public:
	
	/**
	 * Вспомогательный класс, реализующий идиому RAII для обозначения
	 * точек входа и выхода для функции
	 */
	class _SubprogramTracer final
	{
		POSIXCPP_NOT_COPYABLE(_SubprogramTracer)
	public:
		
		_SubprogramTracer(const char *signature) :
		_signature(signature)
		{
			PUT_LOG_DEBUG(cps::Logger::Verbosity_t::VERBOSE, \
			    _signature, _in);
		}
		
		~_SubprogramTracer()
		{
			PUT_LOG_DEBUG(cps::Logger::Verbosity_t::VERBOSE, \
			    _signature, _out);
		}
		
	private:
		
		const char *_signature;
		
		static const char _in[], _out[];
	};
	
	/**
	 * Перечисление документов логгирования приложения
	 */
	POSIXCPP_SIZED_ENUM(Target_t, uint8_t)
	{
		/**
		 * Информация о ходе работы приложения
		 */
		INFO = 0,
		/**
		 * Отладочная информация
		 */
		DEBUG = 1,
		/**
		 * Информация об ошибках в работе программы
		 */
		ERROR = 2,
	};

	/**
	 * Перечисление количества информации в документе лога
	 */
	POSIXCPP_SIZED_ENUM(Verbosity_t, uint8_t)
	{
		/**
		 * Не выводить информацию
		 */
		NONE = 0,
		/**
		 * Минимальный уровень вывода
		 */
		MINIMAL = 1,
		/**
		 * Средний уровень вывода
		 */
		MEDIUM = 2,
		/**
		 * Значительный уровень вывода
		 */
		MORE = 3,
		/**
		 * Максимальный уровень вывода
		 */
		VERBOSE = 4,
		    
		VERBOSITY_SIZE
	};

	friend std::ostream & operator <<(
	  std::ostream &stream,
	  Logger::Target_t target);

	friend std::ostream & operator <<(
	  std::ostream &stream,
	  Logger::Verbosity_t verbosity);
	
	static Verbosity_t verbosityFromString(const std::string &str);

	/**
	 * @brief Инициализация компонента
	 * 
	 * Должна быть вызвана до использования
	 * @param infoStream поток информационных сообщений
	 * @param debugStream поток отладочных сообщений
	 * @param erroeStream поток сообщений об ошибках
	 */
	static void Init(
	  std::ostream &infoStream,
	  Verbosity_t infoLevel,
	  std::ostream &debugStream,
	  Verbosity_t debugLevel,
	  std::ostream &errorStreamm,
	  Verbosity_t errorLevel);
	
	static void init();
	
	static void setLevel(
	  Target_t target,
	  Verbosity_t level);
	
	/**
	 * @brief завершение компонента
	 * @obsolete
	 */
	static void Destroy();
	
	/**
	 * @brief уничтожение компонента
	 */
	static void destroy();
	
	/**
	 * @brief Послать сообщение для логгирования
	 * @param target
	 * @param args
	 */
	template <typename... Args>
	static void log(
	  Target_t target,
	  Verbosity_t verbosity,
	  const Args&... args)
	{
		if (verbosity != Verbosity_t::NONE &&
		    *m_logs[uint8_t(target)] != NULL)
			(*m_logs[uint8_t(target)])->log(verbosity, args...);
	}
	
private:

	Logger() = delete;
	
	Logger(const Logger & orig) = delete;
	
	Logger & operator = (const Logger &orug) = delete;

	friend class Log;
	
	/**
	 * Документ логгирования
	 */
	class Log
	{
	public:
		/**
		 * Конструктор 
		 * @param stream используемый поток вывода
		 */
		Log(
		  std::ostream &stream,
		  Target_t target,
		  Verbosity_t level,
		  char delimiter = ' ');
		
		/**
		 * 
		 * @param verbosity уровень сообщения
		 * @param args аргументы для вывода
		 */
		template <typename... Args>
		void log(
		  Verbosity_t verbosity,
		  const Args&... args)
		{
			if ((m_verbosityLevel >= verbosity)) {
				assert(Logger::m_lock != NULL);
				posix::threads::CriticalSection
				    cs(*Logger::m_lock);
				posix::time::Timespec currentTime =
				    Logger::m_clock.getTime();

				m_stream << (currentTime -= Logger::m_startTime)
				    << ":[" << m_target << "](" << verbosity
				    << ")\t";
				__log(args...);
				m_stream << '\n';
			}
		}
		
		void setDelimiter(char newVal)
		{
			m_delimiter = newVal;
		}
		
		void setLevel(Verbosity_t newVal)
		{
			m_verbosityLevel = newVal;
		}

	private:

		/**
		 * Финальный метод вывода параметра в поток
		 * @param value значение для вывода
		 */
		template <typename T>
		void __log(const T& value)
		{
			m_stream << value;
		}

		/**
		 * Метод вывода набора значений в поток
		 */
		template <typename U, typename... T>
		void __log(const U& head, const T&... tail)
		{
			m_stream << head;
			if (m_delimiter != 0)
				m_stream << m_delimiter;
			__log(tail...);
		}
		
		/**
		 * Ссылка на используемый поток вывода
		 */
		std::ostream &m_stream;
		
		/**
		 * Уровень выводимых сообщений
		 */
		Verbosity_t m_verbosityLevel;
		
		/**
		 * Установленный тип логгирования
		 */
		Target_t m_target;
		
		/**
		 * Флаг установки
		 */
		char	m_delimiter;
	};
	
	class StaticInitializer
	{
	public:
		StaticInitializer();
	};
	
	static std::string m_verbosityStrings[];
	
	static StaticInitializer	m_staticInitializer;
	
	/**
	 * Документ логгирования информационных сообщений
	 */
	static Log *m_infoLog;
	/**
	 * Документ логгирования отладочных сообщений
	 */
	static Log *m_debugLog;
	/**
	 * Документ логгирования сообщений об ошибках
	 */
	static Log *m_errorLog;

	static Log **m_logs[];
	/**
	 * Мьютекс, защищающий поток вывода
	 */
	static posix::threads::Mutex *m_lock;
	/**
	 * Часы измерения времени высокого разрешения
	 */
	static posix::time::Clock m_clock;
	/**
	 * Время запуска системы логгирования
	 */
	static posix::time::Timespec m_startTime;
};
}

#else

#ifndef NO_INFO_LOGGING
#define PUT_LOG_INFO(...)
#else
#define PUT_LOG_INFO(...)
#endif

#ifndef NO_DEBUG_LOGGING
#define PUT_LOG_DEBUG(...)
#else
#define PUT_LOG_DEBUG(...)
#endif

#ifndef NO_ERROR_LOGGING
#define PUT_LOG_ERROR(...)
#else
#define PUT_LOG_ERROR(...)
#endif

#ifndef PUT_LOG_CALL_TRACE
#define PUT_LOG_CALL_TRACE
#endif
#define PUT_LOG_CRITICAL_ERROR(...)

#endif // __cplusplus

#endif	/* CPS_	LOGGER_HPP */


#endif
