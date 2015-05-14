#pragma once

#include <memory>
#include <vector>
#include <initializer_list>

using namespace std;


enum ArgType
{
    ARG_MANDATORY,
    ARG_OPTIONAL
};

struct CmdlineArg
{
public:
    const char*   m_cmdTag;
    const char*   m_cmdValueName;
    const char*   m_helpString;
    ArgType       m_argType;
protected:
    bool          m_isParsed;

public:
    CmdlineArg()
    {
        m_argType = ARG_MANDATORY;
        m_isParsed   = false;
        m_cmdTag = nullptr;
    }

    virtual ~CmdlineArg()
    {
    }

    virtual bool Parse(int idx, int argc, const char* argv[]) = 0;
    virtual void PrintHelpCmdUsage();
    virtual void PrintHelpCmdExp();
    virtual int GetArgCount() = 0;
    virtual bool Validate();
};


template <typename T>
struct CmdlineArgTyped
    : public CmdlineArg
{
public:
    T*       m_pData;

    CmdlineArgTyped(
        T* pData,
        const char* tag,
        const char* name,
        ArgType argType,
        const char* help = nullptr
        )
    {
        m_pData         = pData;
        m_cmdTag        = tag;
        m_cmdValueName  = name;
        m_argType       = argType;
        m_helpString    = help;
    }

    virtual bool Parse(int idx, int argc, const char* argv[]) override;

    virtual int GetArgCount() override
    {
        return 1;
    }
};


struct CmdlineArgGroup;
struct CmdlineArgWrapper
{
    shared_ptr<CmdlineArg> m_cmdArg;

    template<typename T>
    CmdlineArgWrapper(
        T* pData,
        const char* tag,
        const char* name,
        ArgType argType,
        const char* help = nullptr
        )
    {
        m_cmdArg = shared_ptr<CmdlineArg>(
            new CmdlineArgTyped<T>(pData, tag, name, argType, help)
            );
    }

    CmdlineArgWrapper(const CmdlineArgGroup& group);
};


struct CmdlineArgGroup : public CmdlineArg
{
protected:
    CmdlineArg* FindMatchingArg(const char* arg);

public:
    vector<CmdlineArgWrapper> m_args;

    CmdlineArgGroup(initializer_list<CmdlineArgWrapper> il, ArgType argType = ARG_MANDATORY)
        : m_args(il)
    {
        m_cmdTag = "@";
        m_argType = argType;
    }

    virtual bool Parse(int idx, int argc, const char* argv[]) override;
    virtual void PrintHelpCmdUsage() override;
    virtual void PrintHelpCmdExp() override;
    virtual int GetArgCount() override
    {
        return 0;
    }
};


typedef CmdlineArgGroup CmdlineOpts;
