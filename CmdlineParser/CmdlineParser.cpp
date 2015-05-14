#include "CmdlineParser.h"


CmdlineArgWrapper::CmdlineArgWrapper(const CmdlineArgGroup& group)
{
    m_cmdArg = shared_ptr<CmdlineArg>(new CmdlineArgGroup(group));
}


void CmdlineArg::PrintHelpCmdExp()
{
    if (m_helpString)
    {
        printf("   -%-8s %s\n", m_cmdTag, m_helpString);
    }
}


void CmdlineArg::PrintHelpCmdUsage()
{
    if (m_argType == ARG_OPTIONAL)
    {
        printf("[");
    }

    if(m_cmdValueName == nullptr)
    {
        printf("-%s", m_cmdTag);
    }
    else
    {
        printf("-%s <%s>", m_cmdTag, m_cmdValueName);
    }

    if (m_argType == ARG_OPTIONAL)
    {
        printf("]");
    }
    else
    {
        printf(" ");
    }
}


bool CmdlineArg::Validate()
{
    return (m_argType == ARG_OPTIONAL) || m_isParsed;
}


void CmdlineArgGroup::PrintHelpCmdExp()
{
    for (int i = 0; i < m_args.size(); ++i)
    {
        m_args[i].m_cmdArg->PrintHelpCmdExp();
    }
}


void CmdlineArgGroup::PrintHelpCmdUsage()
{
    bool static fPrintAppName = true;
    bool endUsage = false;

    if (fPrintAppName)
    {
        fPrintAppName = false;
        endUsage = true;
        char appFullPath[MAX_PATH];
        char appName[MAX_PATH];
        GetModuleFileName(nullptr, appFullPath, _countof(appFullPath));
        _splitpath_s(appFullPath, nullptr, 0, nullptr, 0, appName, _countof(appName), nullptr, 0);
        printf("%s ", appName);
    }

    if (m_argType == ARG_OPTIONAL)
    {
        printf("[");
    }

    for (int i = 0; i < m_args.size(); ++i)
    {
        m_args[i].m_cmdArg->PrintHelpCmdUsage();
    }

    if (m_argType == ARG_OPTIONAL)
    {
        printf("]");
    }
    if (endUsage)
    {
        printf("\n");
        for (int i = 0; i < m_args.size(); ++i)
        {
            m_args[i].m_cmdArg->PrintHelpCmdExp();
        }
    }
}


CmdlineArg* CmdlineArgGroup::FindMatchingArg(const char* arg)
{
    for (int i = 0; i < m_args.size(); ++i)
    {
        CmdlineArgGroup* pGroup = dynamic_cast<CmdlineArgGroup*>(m_args[i].m_cmdArg.get());
        if (pGroup != nullptr)
        {
            CmdlineArg* pResult = pGroup->FindMatchingArg(arg);
            if (pResult != nullptr)
            {
                return pResult;
            }
        }
        else if ((arg[0] == '-' || arg[0] == '/')
            && _stricmp(arg+1, m_args[i].m_cmdArg->m_cmdTag) == 0)
        {
            return m_args[i].m_cmdArg.get();
        }
    }

    return nullptr;
}


bool CmdlineArgGroup::Parse(int /*idx*/, int argc, const char* argv[])
{
    if (argc == 1)
    {
        return true;
    }

    if (strcmp(argv[1], "-?") == 0
        || strcmp(argv[1], "/?") == 0)
    {
        PrintHelpCmdUsage();
        return false;
    }

    for (int i = 1; i < argc; ++i)
    {
        CmdlineArg* pArg = FindMatchingArg(argv[i]);
        if (pArg == nullptr)
        {
            printf("Unknown option: %s\n"
                "Type -? for help.\n",
                argv[i]);
            return false;
        }

        int argCount = pArg->GetArgCount();
        if (i + argCount >= argc)
        {
            printf("Missing arguments for option: %s\n"
                "Type -? for help.\n",
                argv[i]);
            return false;
        }

        if (!pArg->Parse(i+1, argc, argv))
        {
            printf("Invalid arguments for option: %s\n"
                "Type -? for help.\n",
                argv[i]);
            return false;
        }

        i += argCount;
    }
    return true;
}


bool CmdlineArgTyped<int>::Parse(int idx, int /*argc*/, const char* argv[])
{
    *m_pData = strtol(argv[idx], nullptr, 10);
    return true;
}


bool CmdlineArgTyped<unsigned short>::Parse(int idx, int /*argc*/, const char* argv[])
{
    *m_pData = static_cast<unsigned short>(strtoul(argv[idx], nullptr, 10));
    return true;
}


bool CmdlineArgTyped<BYTE>::Parse(int idx, int /*argc*/, const char* argv[])
{
    *m_pData = static_cast<BYTE>(strtoul(argv[idx], nullptr, 10));
    return true;
}


bool CmdlineArgTyped<const char*>::Parse(int idx, int /*argc*/, const char* argv[])
{
    *m_pData = argv[idx];
    return true;
}


bool CmdlineArgTyped<unsigned int>::Parse(int idx, int /*argc*/, const char* argv[])
{
    *m_pData = strtoul(argv[idx], nullptr, 10);
    return true;
}


bool CmdlineArgTyped<GUID>::Parse(int idx, int /*argc*/, const char* argv[])
{
    auto res = UuidFromStringA(const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(argv[idx])), m_pData);
    return res == RPC_S_OK;
}


int CmdlineArgTyped<bool>::GetArgCount()
{
    return 0;
}


bool CmdlineArgTyped<bool>::Parse(int /*idx*/, int /*argc*/, const char* /*argv*/[])
{
    *m_pData = true;
    return true;
}
