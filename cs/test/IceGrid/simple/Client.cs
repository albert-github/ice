// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using Test;

public class Client
{
    private static int
    run(string[] args, Ice.Communicator communicator)
    {
	bool withDeploy = false;
	for(int i = 1; i < args.Length; i++)
	{
	    if(args[i].Equals("--with-deploy"))
	    {
		withDeploy = true;
		break;
	    }
	}

	TestIntfPrx obj;

	if(!withDeploy)
	{
	    obj = AllTests.allTests(communicator);
	}
	else
	{
	    obj = AllTests.allTestsWithDeploy(communicator);
	}
	
	System.Console.Out.Write("shutting down server... ");
	System.Console.Out.Flush();
	obj.shutdown();
	System.Console.Out.WriteLine("ok");

	return 0;
    }

    public static void Main(string[] args)
    {
        int status = 0;
        Ice.Communicator communicator = null;

        try
        {
            communicator = Ice.Util.initialize(ref args);
            status = run(args, communicator);
        }
        catch (Ice.LocalException ex)
        {
	    System.Console.Error.WriteLine(ex);
            status = 1;
        }

        if (communicator != null)
        {
            try
            {
                communicator.destroy();
            }
            catch (Ice.LocalException ex)
            {
                System.Console.Error.WriteLine(ex);
                status = 1;
            }
        }

	System.Environment.Exit(status);
    }
}
