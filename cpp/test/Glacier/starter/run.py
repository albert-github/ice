#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2001
# MutableRealms, Inc.
# Huntsville, AL, USA
#
# All Rights Reserved
#
# **********************************************************************

import os, sys

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "TestUtil.py")):
        break
else:
    raise "can't find toplevel directory!"

sys.path.append(os.path.join(toplevel, "config"))
import TestUtil

starter = os.path.join(toplevel, "bin", "glacierstarter")
router = os.path.join(toplevel, "bin", "glacier")

updatedServerOptions = TestUtil.serverOptions.replace("TOPLEVELDIR", toplevel)
updatedClientOptions = TestUtil.clientOptions.replace("TOPLEVELDIR", toplevel)
updatedClientServerOptions = TestUtil.clientServerOptions.replace("TOPLEVELDIR", toplevel)

print "starting glacier starter...",
command = starter + updatedClientServerOptions + \
          r' --Glacier.Starter.RouterPath=' + router + \
          r' --Glacier.Starter.Endpoints="default -p 12346 -t 2000"' + \
          r' --Glacier.Router.Endpoints="default"' + \
          r' --Glacier.Client.Endpoints="default"' + \
          r' --Glacier.Server.Endpoints="default"'

starterPipe = os.popen(command)
TestUtil.getServerPid(starterPipe)
TestUtil.getAdapterReady(starterPipe)
print "ok"

name = os.path.join("Glacier", "starter")
TestUtil.clientServerTest(toplevel, name)

print "shutting down glacier starter...",
TestUtil.killServers() # TODO: Graceful shutdown
print "ok"

sys.exit(0)
