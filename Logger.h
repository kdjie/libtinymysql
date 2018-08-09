//============================================================================
// Name        : Logger.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2012
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <syslog.h>

namespace tinymysql
{
	// 日志输出级别定义
	// 保持与<sys/syslog.h>级别定义一致
	enum LogLevel
	{
		Error = 3,		// LOG_ERR
		Warn = 4,		// LOG_WARNING
		Notice = 5,		// LOG_NOTICE
		Info = 6,		// LOG_INFO
		Debug = 7		// LOG_DEBUG
	};

	class ILogReport
	{
	public:
		virtual ~ILogReport() {}

		virtual void log(int iLevel, const char* szFormat, ...) = 0;
	};

	class ILogReportAware
	{
	protected:
		ILogReport* m_pLogReport;
	public:
		ILogReportAware() : m_pLogReport(NULL) {}
		virtual ~ILogReportAware() {}

		void setLogReport(ILogReport* p) { m_pLogReport = p; }
		ILogReport* getLogReport() { return m_pLogReport; }
	};

	class CSyslogReport
		: public ILogReport
	{
	public:
		CSyslogReport()
		{
			openlog(NULL, LOG_PID, LOG_USER);
		}
		virtual ~CSyslogReport()
		{
			closelog();
		}

		virtual void log(int iLevel, const char* szFormat, ...)
		{
			va_list va;
			va_start(va, szFormat);

			vsyslog(iLevel, szFormat, va);

			va_end(va);
		}
	};
}
