#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <CLI/CLI.hpp>
#include <filesystem>
#include <xyz/openbmc_project/Common/error.hpp>
#include <xyz/openbmc_project/CustomErr/Example/error.hpp>

using namespace phosphor::logging;
auto bus = sdbusplus::bus::new_default();
bool displayPEL(std::string);

int main(int argc, char* argv[])
{
    CLI::App app{"PEL Test Tool"};
    bool countPEL = false;
    bool internalPEL = false;
    bool dBusPEL = false;
    bool customPEL = false;
    bool deleteAllPELs = false;
	std::string getID;

    app.set_help_flag("-h,--help", "Print this help message");
    app.add_flag("-p", countPEL, "Print total PEL count");
	app.add_option("-g", getID, "Get the content of PEL with its ID");
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
	else if (!getID.empty())
	{
		displayPEL(getID);
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
                     ErrorFour::CALLOUT_INVENTORY_PATH("/xyz/openbmc_project/inventory/system/board/AST2600_EVB"));
    }

    if (deleteAllPELs)
    {
        auto method = bus.new_method_call("xyz.openbmc_project.Logging", "/xyz/openbmc_project/logging",
                                            "xyz.openbmc_project.Collection.DeleteAll", "DeleteAll");
        auto reply = bus.call(method);
    }

    return 0;
}

bool displayPEL(std::string pelID)
{
	using Value = std::variant<bool, uint32_t, uint64_t, std::string, std::vector<std::string>>;
	std::map<std::string, Value> properties;
	std::string entry = "/xyz/openbmc_project/logging/entry/"+pelID;
	auto method = bus.new_method_call("xyz.openbmc_project.Logging", entry.c_str(), "org.freedesktop.DBus.Properties", "GetAll");
	method.append("xyz.openbmc_project.Logging.Entry");
	try
	{
		auto reply = bus.call(method);
		reply.read(properties);
	}
	catch (const sdbusplus::exception::SdBusError& e)
	{
		if ( std::string(e.name()) == "org.freedesktop.DBus.Error.UnknownObject")
		{
			std::cout<<"No PEL ID "<<pelID<<" entry!!!"<<std::endl;
			return false;
		}
	}
	auto id = std::get<uint32_t>(properties["Id"]);
	auto severity = std::get<std::string>(properties["Severity"]);
	auto message = std::get<std::string>(properties["Message"]);
	auto additionalData = std::get<std::vector<std::string>>(properties["AdditionalData"]);
	auto resolved = std::get<bool>(properties["Resolved"])?"true":"false";
	auto timestamp = std::get<uint64_t>(properties["Timestamp"]);
	auto updateTimestamp = std::get<uint64_t>(properties["UpdateTimestamp"]);

	std::cout<<"xyz.openbmc_project.Logging:"<<std::endl;
	std::cout<<"    Id = "<<id<<std::endl;
	std::cout<<"    Message = \""<<message<<"\""<<std::endl;
	std::cout<<"    Severity = \""<<severity<<"\""<<std::endl;
	std::cout<<"    AdditionalData = \"";
	for (auto it = additionalData.begin(); it != additionalData.end(); ++it)
		std::cout<<*it<<" ";
	std::cout<<"\""<<std::endl<<"    Resolved = \""<<resolved<<"\" ";
	std::cout<<"Timestamp = "<<timestamp<<" UpdateTimestamp = "<<updateTimestamp<<std::endl;

	auto method1 = bus.new_method_call("xyz.openbmc_project.Logging", entry.c_str(), "org.freedesktop.DBus.Properties", "Get");
	method1.append("xyz.openbmc_project.Association.Definitions", "Associations");
	auto reply1 = bus.call(method1);
	using AssociationList = std::vector<std::tuple<std::string, std::string, std::string>>;
	std::variant<AssociationList> list;
	reply1.read(list);
	auto& assocs = std::get<AssociationList>(list);

	std::cout<<"xyz.openbmc_project.Association.Definitions:\n"<<"    Associations = ";
	for (const auto& item : assocs)
		std::cout<<"\""<<std::get<0>(item)<<" "<<std::get<1>(item)<<" "<<std::get<2>(item)<<"\""<<" ";

	std::cout<<std::endl;

	return true;
}
