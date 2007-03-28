struct ce_parser_arg
{
    ConstraintEvaluator *eval;
    DDS *dds;

    ce_parser_arg() : eval(0), dds(0)
    {}
    ce_parser_arg(ConstraintEvaluator *e, DDS *d) : eval(e), dds(d)
    {}
    virtual ~ce_parser_arg()
    {}

    ConstraintEvaluator *get_eval()
    {
        return eval;
    }
    void set_eval(ConstraintEvaluator *obj)
    {
        eval = obj;
    }

    DDS *get_dds()
    {
        return dds;
    }
    void set_dds(DDS *obj)
    {
        dds = obj;
    }
};

