// **********************************************************************
//
// Copyright (c) 2003-2004 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

public sealed class IB1I : Test.MB._IB1Disp
{
    public IB1I()
    {
    }
    
    public override Test.MA.IAPrx iaop(Test.MA.IAPrx p, Ice.Current current)
    {
        return p;
    }
    
    public override Test.MB.IB1Prx ib1op(Test.MB.IB1Prx p, Ice.Current current)
    {
        return p;
    }
}
