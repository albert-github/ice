// **********************************************************************
//
// Copyright (c) 2004 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/Options.h>
#include <Ice/Service.h>
#include <IcePatch2/FileServerI.h>
#include <IcePatch2/Util.h>

#ifdef _WIN32
#   include <direct.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace Ice;
using namespace IcePatch2;

namespace IcePatch2
{

class AdminI : public Admin
{
public:
    
    AdminI(const CommunicatorPtr& communicator) :
	_communicator(communicator)
    {
    }

    virtual void
    shutdown(const Current&)
    {
	_communicator->shutdown();
    }

private:

    const CommunicatorPtr _communicator;
};

class PatcherService : public Service
{
public:

    PatcherService();

protected:

    virtual bool start(int, char*[]);
    virtual bool stop();
    virtual CommunicatorPtr initializeCommunicator(int&, char*[]);

private:

    void usage(const std::string&);
};

};

IcePatch2::PatcherService::PatcherService()
{
}

bool
IcePatch2::PatcherService::start(int argc, char* argv[])
{
    string dataDir;

    IceUtil::Options opts;
    opts.addOpt("h", "help");
    opts.addOpt("v", "version");
    
    vector<string> args;
    try
    {
    	args = opts.parse(argc, argv);
    }
    catch(const IceUtil::Options::BadOpt& e)
    {
        cerr << e.reason << endl;
	usage(argv[0]);
	return EXIT_FAILURE;
    }

    if(opts.isSet("h") || opts.isSet("help"))
    {
	usage(argv[0]);
	return EXIT_SUCCESS;
    }
    if(opts.isSet("v") || opts.isSet("version"))
    {
	cout << ICE_STRING_VERSION << endl;
	return EXIT_SUCCESS;
    }

    if(args.size() > 1)
    {
	cerr << argv[0] << ": too many arguments" << endl;
	usage(argv[0]);
	return false;
    }
    if(args.size() == 1)
    {
        dataDir = args[0];
    }

    PropertiesPtr properties = communicator()->getProperties();

    if(dataDir.empty())
    {
	dataDir = properties->getProperty("IcePatch2.Directory");
	if(dataDir.empty())
	{
	    cerr << argv[0] << ": no data directory specified" << endl;
	    usage(argv[0]);
	    return false;
	}
    }

    if(!isDir(dataDir))
    {
	throw "`" + dataDir + "' is not a directory";
    }

    FileInfoSeq infoSeq;

    try
    {
	dataDir = normalize(dataDir);
	loadFileInfoSeq(dataDir, infoSeq);
    }
    catch(const string& ex)
    {
        cerr << argv[0] << ": " << ex << endl;
        return false;
    }
    catch(const char* ex)
    {
        cerr << argv[0] << ": " << ex << endl;
        return false;
    }
    
    const char* endpointsProperty = "IcePatch2.Endpoints";
    if(properties->getProperty(endpointsProperty).empty())
    {
	cerr << argv[0] << ": property `" << endpointsProperty << "' is not set" << endl;
	return false;
    }
    ObjectAdapterPtr adapter = communicator()->createObjectAdapter("IcePatch2");

    const char* adminEndpointsProperty = "IcePatch2.Admin.Endpoints";
    ObjectAdapterPtr adminAdapter;
    if(!properties->getProperty(adminEndpointsProperty).empty())
    {
	adminAdapter = communicator()->createObjectAdapter("IcePatch2.Admin");
    }

    const char* idProperty = "IcePatch2.Identity";
    Identity id = stringToIdentity(properties->getPropertyWithDefault(idProperty, "IcePatch2/server"));
    adapter->add(new FileServerI(dataDir, infoSeq), id);

    if(adminAdapter)
    {
	const char* adminIdProperty = "IcePatch2.AdminIdentity";
	Identity adminId = stringToIdentity(properties->getPropertyWithDefault(adminIdProperty, "IcePatch2/admin"));
	adminAdapter->add(new AdminI(communicator()), adminId);
    }

    adapter->activate();
    if(adminAdapter)
    {
	adminAdapter->activate();
    }

    return true;
}

bool
IcePatch2::PatcherService::stop()
{
    return true;
}

CommunicatorPtr
IcePatch2::PatcherService::initializeCommunicator(int& argc, char* argv[])
{
    return Service::initializeCommunicator(argc, argv);
}

void
IcePatch2::PatcherService::usage(const string& appName)
{
    string options =
	"Options:\n"
	"-h, --help           Show this message.\n"
	"-v, --version        Display the Ice version.";
#ifdef _WIN32
    if(checkSystem())
    {
        options.append(
	"\n"
	"\n"
	"--service NAME       Run as the Windows service NAME.\n"
	"\n"
	"--install NAME [--display DISP] [--executable EXEC] [args]\n"
	"                     Install as Windows service NAME. If DISP is\n"
	"                     provided, use it as the display name,\n"
	"                     otherwise NAME is used. If EXEC is provided,\n"
	"                     use it as the service executable, otherwise\n"
	"                     this executable is used. Any additional\n"
	"                     arguments are passed unchanged to the\n"
	"                     service at startup.\n"
	"--uninstall NAME     Uninstall Windows service NAME.\n"
	"--start NAME [args]  Start Windows service NAME. Any additional\n"
	"                     arguments are passed unchanged to the\n"
	"                     service.\n"
	"--stop NAME          Stop Windows service NAME."
        );
    }
#else
    options.append(
        "\n"
        "\n"
        "--daemon             Run as a daemon.\n"
        "--noclose            Do not close open file descriptors.\n"
        "--nochdir            Do not change the current working directory."
    );
#endif
    cerr << "Usage: " << appName << " [options] DIR" << endl;
    cerr << options << endl;
}

int
main(int argc, char* argv[])
{
    IcePatch2::PatcherService svc;
    return svc.main(argc, argv);
}
