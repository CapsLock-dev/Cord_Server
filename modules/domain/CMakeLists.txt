add_library(domain) 

target_sources(domain 
    PRIVATE
        src/user.cpp
    PUBLIC
        FILE_SET HEADERS
		BASE_DIRS include
		FILES
			include/domain/user.hpp
			include/domain/domain_error.hpp
			include/domain/messages.hpp
			include/domain/auth.hpp
)

target_compile_features(domain PUBLIC cxx_std_23)
