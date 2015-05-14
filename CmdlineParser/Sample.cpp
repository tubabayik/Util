int main(int argc, const char* argv[])
{  
    int op1, op2;

    CmdlineOpts opts = {
                            { &op1, "o1", "op1", ARG_MANDATORY, "does this and that..." },
                            { &op2, "o2", "op2", ARG_OPTIONAL, "does something else" },
                            CmdlineArgGroup{
                                    { &op1, "o5", "op1", ARG_MANDATORY, "does a lot..." },
                                    { &op2, "o2", "op2", ARG_MANDATORY }
                            }
    };

    if( !opts.Parse(0, argc, argv) )
    {
      return -1;
    }
    
    return 0;

}
