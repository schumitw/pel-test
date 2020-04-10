#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <CLI/CLI.hpp>
#include <filesystem>
#include <xyz/openbmc_project/Common/error.hpp>
#include "xyz/openbmc_project/CustomErr/Example/error.hpp"

using namespace phosphor::logging;

int main(int argc, char* argv[])
{
    CLI::App app{"PEL Test Tool"};
    bool countPEL = false;
    bool internalPEL = false;
    bool dBusPEL = false;
    bool customPEL = false;
    bool deleteAllPELs = false;
    auto bus = sdbusplus::bus::new_default();

    app.set_help_flag("-h,--help", "Print this help message");
    app.add_flag("-p", countPEL, "Print total PEL count");
    app.add_flag("-i", internalPEL, "Raise an internal PEL");
    app.add_flag("-d", dBusPEL, "Raise a D-Bus PEL");
    app.add_flag("-c", customPEL, "Raise a custom PEL");
    app.add_flag("-D", deleteAllPELs, "Delete All PELs");
    
    if (argc == 1)
        std::cerr<<app.help("", CLI::AppFormatMode::All)<<std::endl;

    CLI11_PARSE(app, argc, argv);

    if (countPEL)
    {
        std::vector <unsigned int> pelID;

        // PELs are stored in the /var/lib/phosphor-logging/errors/
        for (auto it = std::filesystem::directory_iterator("/var/lib/phosphor-logging/errors/");
                it != std::filesystem::directory_iterator(); ++it)
        {
            if (!std::filesystem::is_regular_file((*it).path()))
                continue;
            pelID.push_back(std::stoi((*it).path().filename()));
            //std::cout<<"PEL ID : "<<std::stoi((*it).path().filename())<<std::endl;
        }

        std::cout<<"Total PEL count : "<<pelID.size()<<std::endl;
        std::sort(pelID.begin(), pelID.end());
        std::cout<<"PEL ID :";
        for (const auto &i: pelID)
            std::cout<<' '<<i;
        std::cout<<std::endl;
    }

    if (internalPEL)
    {
        // Method 1
        using pelA = sdbusplus::xyz::openbmc_project::Common::Error::InternalFailure;
        try
        {
            elog<pelA>();
        }
        catch (pelA& e)
        {
            commit<pelA>();
        }

        // Method 2
        using pelB = sdbusplus::xyz::openbmc_project::Common::Error::NotAllowed;
        report<pelB>(xyz::openbmc_project::Common::NotAllowed::REASON("Unknown"));
    }

    if (dBusPEL)
    {
        auto method = bus.new_method_call("xyz.openbmc_project.Logging", "/xyz/openbmc_project/logging",
                                          "xyz.openbmc_project.Logging.Create", "Create");
        method.append("xyz.openbmc_project.Common.Error.TestDbusError");
        method.append("xyz.openbmc_project.Logging.Entry.Level.Error");
        std::map<std::string, std::string> additionalData;
        additionalData["KEY"] = "VALUE";
        method.append(additionalData);
        auto reply = bus.call(method);
    }

    if (customPEL)
    {
        using pelC = sdbusplus::xyz::openbmc_project::CustomErr::Example::Error::ErrorOne;
        report<pelC>();

        using pelD = sdbusplus::xyz::openbmc_project::CustomErr::Example::Error::ErrorTwo;
        using namespace xyz::openbmc_project::CustomErr::Example;
        report<pelD>(ErrorTwo::COMMAND_NAME(argv[0]));

        using pelE = sdbusplus::xyz::openbmc_project::CustomErr::Example::Error::ErrorThree;
		using namespace xyz::openbmc_project::CustomErr::Example;
		report<pelE>(ErrorThree::COMMAND_NAME(argv[0]),
					 ErrorThree::CALLOUT_ERRNO(0),
					 ErrorThree::CALLOUT_DEVICE_PATH("/sys/devices/test"));

        using pelF = sdbusplus::xyz::openbmc_project::CustomErr::Example::Error::ErrorFour;
		using namespace xyz::openbmc_project::CustomErr::Example;
		report<pelF>(ErrorFour::COMMAND_NAME(argv[0]),
                     ErrorFour::CALLOUT_INVENTORY_PATH("/xyz/openbmc_project/inventory/system/chassis/bmc"));
    }

    if (deleteAllPELs)
    {
        auto method = bus.new_method_call("xyz.openbmc_project.Logging", "/xyz/openbmc_project/logging",
                                            "xyz.openbmc_project.Collection.DeleteAll", "DeleteAll");
        auto reply = bus.call(method);
    }

    return 0;
}
