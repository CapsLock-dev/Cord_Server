add_library(network STATIC)

target_sources(network
	PRIVATE
    	src/server.cpp
        src/session.cpp
        src/task_group.cpp
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS include
		FILES
			include/network/server.hpp
			include/network/session.hpp
			include/network/task_group.hpp
)

target_link_libraries(network PRIVATE 
    core
    OpenSSL::SSL
    Boost::system
)

target_compile_features(network PUBLIC cxx_std_23)
