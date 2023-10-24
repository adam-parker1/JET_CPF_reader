#ifndef JET_CPF_READER_PLUGIN_H
#define JET_CPF_READER_PLUGIN_H

#include <plugins/udaPlugin.h>
#include <clientserver/export.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THISPLUGIN_VERSION                  1
#define THISPLUGIN_MAX_INTERFACE_VERSION    1        // Interface versions higher than this will not be understood!
#define THISPLUGIN_DEFAULT_METHOD           "help"

LIBRARY_API int JETCPFReader(IDAM_PLUGIN_INTERFACE * idam_plugin_interface);

#ifdef __cplusplus
}
#endif

#endif // JET_CPF_READER_PLUGIN_H
