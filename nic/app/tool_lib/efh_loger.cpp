#include "efh_loger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "spin_lock.h"

void efh_loger::init(const char * file_pre)
{
	return;
	m_log_file = NULL;
	m_filename_pre = file_pre;
	m_level = E_EFH_LOG_INFO;

	spin_unlock(&m_lock);
}

void efh_loger::set_log_level(efh_log_level level)
{
	return;
	m_level = level;
}

bool efh_loger::write_log_info(const char * str, ...)
{
	return true;
	va_list ap;
	struct tm t_m;
	va_start(ap, str);

	if (m_level < E_EFH_LOG_INFO)
		return true;


	time_t t_t = time(NULL);

	localtime_r(&t_t, &t_m);

	spin_lock(&m_lock);
	open_file();
	fprintf(m_log_file, "[info][%d_%02d_%02d-%02d:%02d:%02d]", 
		t_m.tm_year + 1900,
		t_m.tm_mon + 1,
		t_m.tm_mday,

		t_m.tm_hour,
		t_m.tm_min,
		t_m.tm_sec
		);

	vfprintf(m_log_file, str, ap);
	va_end(ap);

	fprintf(m_log_file, "\n");

	fflush(m_log_file);
	spin_unlock(&m_lock);

	return true;
}

bool efh_loger::write_log_error(const char * str, ...)
{
	return true;
	va_list ap;
	va_start(ap, str);

	if (m_level < E_EFH_LOG_ERROR)
		return true;

	struct tm t_m;
	time_t t_t = time(NULL);

	localtime_r(&t_t, &t_m);

	spin_lock(&m_lock);
	open_file();
	fprintf(m_log_file, "[error][%d_%02d_%02d-%02d:%02d:%02d]",
		t_m.tm_year + 1900,
		t_m.tm_mon + 1,
		t_m.tm_mday,

		t_m.tm_hour,
		t_m.tm_min,
		t_m.tm_sec
		);

	vfprintf(m_log_file, str, ap);
	va_end(ap);

	fprintf(m_log_file, "\n");

	fflush(m_log_file);
	spin_unlock(&m_lock);

	return true;
}

bool efh_loger::open_file()
{
	return true;
	if (m_log_file && ftell(m_log_file) < 30 * 1024 * 1024)
		return true;

	if (m_log_file)
	{
		fclose(m_log_file);
		m_log_file = NULL;
	}

	char file_name[256];
	struct tm t_m;
	time_t t_t = time(NULL);

	localtime_r(&t_t, &t_m);
	snprintf(file_name, sizeof(file_name), "%s_%d_%02d_%02d-%02d_%02d_%02d.log",
		m_filename_pre.c_str() ,
		t_m.tm_year + 1900,
		t_m.tm_mon + 1,
		t_m.tm_mday,

		t_m.tm_hour,
		t_m.tm_min,
		t_m.tm_sec
		);

	m_log_file = fopen(file_name, "w");

	return true;
}
