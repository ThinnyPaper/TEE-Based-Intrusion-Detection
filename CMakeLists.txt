project (optee_idtt C)

set (SRC host/idtt.c host/genfile.c host/list.c host/readconf.c host/util.c)

add_executable (${PROJECT_NAME} ${SRC})

find_package(config)
find_package(gcrypt)

target_include_directories(${PROJECT_NAME}
			   PRIVATE ta/include
			   PRIVATE host/include
			   PRIVATE include)

target_link_libraries (${PROJECT_NAME} 
						PRIVATE teec
						PRIVATE config
						PRIVATE gcrypt)

install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
