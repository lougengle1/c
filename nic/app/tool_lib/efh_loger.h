#pragma once

#include <stdio.h>
#include <string>

enum efh_log_level
{
	E_EFH_LOG_NONE,
	E_EFH_LOG_ERROR,
	E_EFH_LOG_INFO
};

class efh_loger
{
public:

	void init(const char *file_pre);

	void set_log_level(efh_log_level);

	bool write_log_info(const char *str, ...);
	bool write_log_error(const char *str, ...);

private:
	bool open_file();

private:
	efh_log_level		m_level;
	FILE			*	m_log_file;
	std::string			m_filename_pre;
	int					m_lock;
};