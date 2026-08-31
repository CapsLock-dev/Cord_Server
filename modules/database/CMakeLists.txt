add_library(database STATIC)

target_sources(database
	PRIVATE
    	src/conn_pool.cpp
    	src/database.cpp
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS include
		FILES
			include/database/conn_pool.hpp
			include/database/database.hpp
			include/database/database_error.hpp
)

target_link_libraries(database PRIVATE 
    core
    libpqxx::pqxx 
)


target_compile_features(database PUBLIC cxx_std_23)
