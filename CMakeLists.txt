option(ENABLE_DEBUG_LOGGING "Enable the 'debug' and 'all' log-channels" TRUE)

define_module(util OBJECT LINKS boost curl hdf5 INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/..)
