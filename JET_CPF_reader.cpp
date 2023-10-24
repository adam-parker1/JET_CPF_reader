/*---------------------------------------------------------------
* v1 UDA Plugin Template: Standardised plugin design template, just add ...
*
* Input Arguments:    IDAM_PLUGIN_INTERFACE *interface
*
* Returns:        0 if the plugin functionality was successful
*            otherwise a Error Code is returned
*
* Standard functionality:
*
*    help    a description of what this plugin does together with a list of functions available
*
*    reset    frees all previously allocated heap, closes file handles and resets all static parameters.
*        This has the same functionality as setting the housekeeping directive in the plugin interface
*        data structure to TRUE (1)
*
*    init    Initialise the plugin: read all required data and process. Retain staticly for
*        future reference.
*
*---------------------------------------------------------------------------------------------------------------*/
#include "JET_CPF_reader.h"
#include <clientserver/udaStructs.h>
#include <clientserver/udaTypes.h>
#include <logging/logging.h>
#include <plugins/udaPlugin.h>

#ifdef __GNUC__
#  include <strings.h>
#endif

#include <clientserver/stringUtils.h>
#include <clientserver/initStructs.h>
#include <fstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "nlohmann/json.hpp"
#include "utils/uda_plugin_helpers.hpp"

class JETCPFReaderPlugin {
public:
    void init(IDAM_PLUGIN_INTERFACE* plugin_interface)
    {
        REQUEST_DATA* request = plugin_interface->request_data;
        if (!init_
                || STR_IEQUALS(request->function, "init")
                || STR_IEQUALS(request->function, "initialise")) {
            reset(plugin_interface);
            // Initialise plugin
            init_ = true;
        }
    }
    void reset(IDAM_PLUGIN_INTERFACE* plugin_interface)
    {
        if (!init_) {
            // Not previously initialised: Nothing to do!
            return;
        }
        // Free Heap & reset counters
        init_ = false;
    }

    int help(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int version(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int build_date(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int default_method(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int max_interface_version(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int get(IDAM_PLUGIN_INTERFACE* plugin_interface);
    int get_time(IDAM_PLUGIN_INTERFACE* plugin_interface);

private:
    bool init_ = false;
};

int JETCPFReaderPlugin::get_time(IDAM_PLUGIN_INTERFACE* interface) {

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    DATA_BLOCK* data_block = interface->data_block;
    REQUEST_DATA* request_data = interface->request_data;

    initDataBlock(data_block);
    data_block->rank = 0;
    data_block->dims = nullptr;

    int pulse{0};
    FIND_REQUIRED_INT_VALUE(request_data->nameValueList, pulse);
    std::string pulse_str{std::to_string(pulse)};

    // (0) parse needed arguments
    // (1) access experiment data
    // (2) deduce rank + type (if applicable)
    // (3) set return data (may be dependent on time or data)

    // All data is floats
    std::vector<float> output_time;
    try {
		// That's all that is needed to do cleanup of used resources (RAII style).
		curlpp::Cleanup myCleanup;

		// Our request to be sent.
		curlpp::Easy myRequest;

        std::ostringstream url;
        url << "http://data-devel.jet.uk/cpf/extension/api/data?";

        // std::string pulse = "99267";
        // std::string signal = "MAGN/IPLA";
        std::string signal_repl = "MAGN/IPLA";
        std::replace( signal_repl.begin(), signal_repl.end(), '/', ','); // replace all '/' to ','
 
		// Set the URL.
        url << "signal=" << signal_repl << "&pulse=" << pulse_str;

		myRequest.setOpt<curlpp::options::Url>(url.str());

        std::ostringstream output;
        output << myRequest;
        nlohmann::json temp = nlohmann::json::parse(output.str());
        output_time = temp["times"][pulse_str].get<std::vector<float>>();

    } catch(curlpp::RuntimeError & e) {
        log_file << e.what() << std::endl;
    } catch(curlpp::LogicError & e) {
        log_file << e.what() << std::endl;
    }

    return imas_json_plugin::uda_helpers::setReturnDataArrayType_Vec(interface->data_block, output_time);
}
int JETCPFReaderPlugin::get(IDAM_PLUGIN_INTERFACE* interface) {

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    DATA_BLOCK* data_block = interface->data_block;
    REQUEST_DATA* request_data = interface->request_data;

    initDataBlock(data_block);
    data_block->rank = 0;
    data_block->dims = nullptr;

    int pulse{0};
    FIND_REQUIRED_INT_VALUE(request_data->nameValueList, pulse);
    const char* signal{nullptr};
    FIND_REQUIRED_STRING_VALUE(request_data->nameValueList, signal);
    std::string signal_str{signal};
    std::string pulse_str{std::to_string(pulse)};

    // if (signal_str == "time") {
    //     return get_time(interface);
    // }
    // (0) parse needed arguments
    // (1) access experiment data
    // (2) deduce rank + type (if applicable)
    // (3) set return data (may be dependent on time or data)

    // std::ofstream log_file;
    // log_file.open("/home/uda/IMAS/JSON-mapping-plugin/JETCPFOut.log", std::ios_base::app);
    // log_file << "Hello World (ofstream)" << std::endl;
    
    // All data is floats
    std::vector<float> output_data;
    try {
		// That's all that is needed to do cleanup of used resources (RAII style).
		curlpp::Cleanup myCleanup;

		// Our request to be sent.
		curlpp::Easy myRequest;

        std::ostringstream url;
        url << "http://data-devel.jet.uk/cpf/extension/api/data?";

        // std::string pulse = "99267";
        // std::string signal = "MAGN/IPLA";
        std::string signal_repl{signal};
        std::replace( signal_repl.begin(), signal_repl.end(), '/', ','); // replace all '/' to ','
 
		// Set the URL.
        url << "signal=" << signal_repl << "&pulse=" << pulse_str;

		myRequest.setOpt<curlpp::options::Url>(url.str());

        std::ostringstream output;
        output << myRequest;
        nlohmann::json temp = nlohmann::json::parse(output.str());
        output_data = temp["signals"][signal]["data"][pulse_str].get<std::vector<float>>();

    } catch(curlpp::RuntimeError & e) {
        log_file << e.what() << std::endl;
    } catch(curlpp::LogicError & e) {
        log_file << e.what() << std::endl;
    }

    return imas_json_plugin::uda_helpers::setReturnDataArrayType_Vec(interface->data_block, output_data);
}

int JETCPFReader(IDAM_PLUGIN_INTERFACE* plugin_interface) {
    //----------------------------------------------------------------------------------------
    // Standard v1 Plugin Interface

    if (plugin_interface->interfaceVersion > THISPLUGIN_MAX_INTERFACE_VERSION) {
        RAISE_PLUGIN_ERROR("Plugin Interface Version Unknown to this plugin: Unable to execute the request!");
    }

    plugin_interface->pluginVersion = THISPLUGIN_VERSION;
    REQUEST_DATA* request = plugin_interface->request_data;

    //----------------------------------------------------------------------------------------
    // Heap Housekeeping

    // Plugin must maintain a list of open file handles and sockets: loop over and close all files and sockets
    // Plugin must maintain a list of plugin functions called: loop over and reset state and free heap.
    // Plugin must maintain a list of calls to other plugins: loop over and call each plugin with the housekeeping request
    // Plugin must destroy lists at end of housekeeping

    // A plugin only has a single instance on a server. For multiple instances, multiple servers are needed.
    // Plugins can maintain state so recursive calls (on the same server) must respect this.
    // If the housekeeping action is requested, this must be also applied to all plugins called.
    // A list must be maintained to register these plugin calls to manage housekeeping.
    // Calls to plugins must also respect access policy and user authentication policy

    try {
        static JETCPFReaderPlugin plugin = {};
        auto* const plugin_func = request->function;

        if (plugin_interface->housekeeping || STR_IEQUALS(plugin_func, "reset")) {
            plugin.reset(plugin_interface);
            return 0;
        }

        //----------------------------------------------------------------------------------------
        // Initialise
        plugin.init(plugin_interface);
        if (STR_IEQUALS(plugin_func, "init")
            || STR_IEQUALS(plugin_func, "initialise")) {
            return 0;
        }

        //----------------------------------------------------------------------------------------
        // Plugin Functions
        //----------------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------------
        // Standard methods: version, builddate, defaultmethod, maxinterfaceversion

        if (STR_IEQUALS(plugin_func, "help")) {
            return plugin.help(plugin_interface);
        } else if (STR_IEQUALS(plugin_func, "version")) {
            return plugin.version(plugin_interface);
        } else if (STR_IEQUALS(plugin_func, "builddate")) {
            return plugin.build_date(plugin_interface);
        } else if (STR_IEQUALS(plugin_func, "defaultmethod")) {
            return plugin.default_method(plugin_interface);
        } else if (STR_IEQUALS(plugin_func, "maxinterfaceversion")) {
            return plugin.max_interface_version(plugin_interface);
        } else if (STR_IEQUALS(plugin_func, "get")) {
            return plugin.get(plugin_interface);
        } else {
            RAISE_PLUGIN_ERROR("Unknown function requested!");
        } 
    } catch (const std::exception& ex) {
        RAISE_PLUGIN_ERROR(ex.what());
    }
}

/**
 * Help: A Description of library functionality
 * @param interface
 * @return
 */
int JETCPFReaderPlugin::help(IDAM_PLUGIN_INTERFACE* interface) {
    const char* help = "\nJETCPFReaderPlugin: Add Functions Names, Syntax, and Descriptions\n\n";
    const char* desc = "JETCPFReaderPlugin: help = description of this plugin";

    return setReturnDataString(interface->data_block, help, desc);
}

/**
 * Plugin version
 * @param interface
 * @return
 */
int JETCPFReaderPlugin::version(IDAM_PLUGIN_INTERFACE* interface) {
    return setReturnDataIntScalar(interface->data_block, THISPLUGIN_VERSION, "Plugin version number");
}

/**
 * Plugin Build Date
 * @param interface
 * @return
 */
int JETCPFReaderPlugin::build_date(IDAM_PLUGIN_INTERFACE* interface) {
    return setReturnDataString(interface->data_block, __DATE__, "Plugin build date");
}

/**
 * Plugin Default Method
 * @param interface
 * @return
 */
int JETCPFReaderPlugin::default_method(IDAM_PLUGIN_INTERFACE* interface) {
    return setReturnDataString(interface->data_block, THISPLUGIN_DEFAULT_METHOD, "Plugin default method");
}

/**
 * Plugin Maximum Interface Version
 * @param interface
 * @return
 */
int JETCPFReaderPlugin::max_interface_version(IDAM_PLUGIN_INTERFACE* interface) {
    return setReturnDataIntScalar(interface->data_block, THISPLUGIN_MAX_INTERFACE_VERSION, "Maximum Interface Version");
}
