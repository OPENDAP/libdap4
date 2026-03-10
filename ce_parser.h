
#ifndef CE_PARSER_H_
#define CE_PARSER_H_

namespace libdap {

/** @brief Carries inputs and outputs for the CE parser. */
struct ce_parser_arg {
    /// Constraint evaluator used during parse.
    ConstraintEvaluator *eval;
    /// DDS used to resolve variable names.
    DDS *dds;

    ce_parser_arg() : eval(0), dds(0) {}

    /** @brief Build parser arguments from evaluator and DDS pointers.
     * @param e Constraint evaluator.
     * @param d DDS used by the parser.
     */
    ce_parser_arg(ConstraintEvaluator *e, DDS *d) : eval(e), dds(d) {}
    virtual ~ce_parser_arg() {}

    /** @brief Get the configured constraint evaluator.
     * @return Evaluator pointer, or null.
     */
    ConstraintEvaluator *get_eval() { return eval; }

    /** @brief Set the constraint evaluator.
     * @param obj Evaluator pointer.
     */
    void set_eval(ConstraintEvaluator *obj) { eval = obj; }

    /** @brief Get the configured DDS.
     * @return DDS pointer, or null.
     */
    DDS *get_dds() { return dds; }

    /** @brief Set the DDS used by the parser.
     * @param obj DDS pointer.
     */
    void set_dds(DDS *obj) { dds = obj; }
};

} // namespace libdap

#endif // CE_PARSER_H_
