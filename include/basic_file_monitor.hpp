//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace filemonitor {

struct file_monitor_event
{
	enum event_type
	{
		null,
		removed,		// file removed
		added,			// file added
		modified,		// file changed
		renamed_old,	// file renamed, old name
		renamed_new		// file renamed, new name
	};
	
	file_monitor_event()
	: type( null ) { }
	
	file_monitor_event( const boost::filesystem::path &p, event_type t )
	: path( p ), type( t ) { }
	
	boost::filesystem::path path;
	event_type type;
};

inline std::ostream& operator << ( std::ostream& os, const file_monitor_event &ev )
{
	os << "file_monitor_event "
	<< []( int type ) {
		switch( type ) {
			case file_monitor_event::removed: return "REMOVED";
			case file_monitor_event::added: return "ADDED";
			case file_monitor_event::modified: return "MODIFIED";
			case file_monitor_event::renamed_old: return "RENAMED_OLD";
			case file_monitor_event::renamed_new: return "RENAMED_NEW";
			default: return "UNKNOWN";
		}
	} ( ev.type ) << " " << ev.path;
	return os;
}

template <typename Service>
class basic_file_monitor
	: public boost::asio::basic_io_object<Service>
{
public:
	explicit basic_file_monitor( boost::asio::io_service &io_service )
		: boost::asio::basic_io_object<Service>( io_service )
	{
	}
	
	void add_file( const boost::filesystem::path &path )
	{
		this->service.add_file( this->implementation, path );
	}

	void remove_file( const boost::filesystem::path &path )
	{
		this->service.remove_file( this->implementation, path );
	}
	
	file_monitor_event monitor()
	{
		boost::system::error_code ec;
		file_monitor_event ev = this->service.monitor( this->implementation, ec );
		boost::asio::detail::throw_error( ec );
		return ev;
	}
	
	file_monitor_event monitor( boost::system::error_code &ec )
	{
		return this->service.monitor( this->implementation, ec );
	}
	
	template <typename Handler>
	void async_monitor( Handler handler )
	{
		this->service.async_monitor( this->implementation, handler );
	}
};
	
}