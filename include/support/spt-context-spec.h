/* -*- Mode: C; fill-column: 70 -*- */
/** @file support/spt-context-spec.h
 *
 * Easy runtime control of context state.
 */
#ifndef support_spt_context_spec_h
#define support_spt_context_spec_h 1

#ifdef __cplusplus
extern "C"
{
#endif

  /** @defgroup context_spec Parsed context-activation specifications
   *  @ingroup context
   *
   * @brief Easy, quick 'n clean control of context state via string
   * parameters.
   *
   * Using the functions below, a parse spec is created from a
   * user-supplied string and applied to a @ref context "context"
   * hierarchy.
   *
   * The format for string input is as follows, in <a
   * href="http://www.rfc-editor.org/std/std68.txt">ABNF</a> notation:
   * @verbatim 
spec			= single_spec *("," single_spec)

single-spec		= [channel-identifier] state-flag context-identifier

state-flag		= "+" / "-"

channel-identifier	= "fatal" / ( "err" / "error" ) / ( "warn" / "warning" ) / "debug" / "info"

context-identifier	= context-name *("." context-name)

context-name		= <any CHAR excluding "." and ",">
@endverbatim
   *
   * For example, one would enable a context named @c context_name
   * using the string <code>+context_name</code>.
   *
   * To enable a certain (single) child context with a non-unique
   * name: <code>+parent_name.child_name</code>
   *
   *@{
   */

  /** Object interface for parsed context-activation state
   * specifications.
   * @ingroup context_spec
   */
  typedef struct __spt_context_parse_spec spt_context_parse_spec_t;

  /** Use a string value to enable or disable multiple contexts.
   * Parses the passed string, and searches for similarly-named
   * contexts under the given root context.
   *
   * @param __ispec A string specifying the contexts to enable or
   * disable, in the above-described format.
   *
   * @return A spt_context_parse_spec_t pointer.
   */
  spt_context_parse_spec_t*
  spt_context_parse_specs(const char* __ispec);

  void
  spt_context_parse_specs_append(spt_context_parse_spec_t* to, spt_context_parse_spec_t* what);

  /** Free the memory used by a parse specification.
   *
   * @param pspec A pointer to the parse specification object to free.
   */
  void
  spt_context_parse_spec_destroy(spt_context_parse_spec_t* pspec);

  /** Free a list of parse specification objects.
   *
   * @param pspec_list A list of parse specification object pointers.
   *
   * @deprecated Use spt_context_parse_spec_destroy instead.
   */
  void __attribute__ (( __deprecated__ ))
  spt_context_parse_spec_destroy_list(spt_context_parse_spec_t* pspec_list);


  /** Apply matching parse specifications to a context.  Each parse
   * specification in the list will be tested, and if it matches the
   * context, it will be applied to the context.
   *
   * @param context The context to apply the parse specifications to.
   *
   * @param pspec_list A list of parse specifications to apply.
   *
   */
  void
  spt_context_apply_parse_specs(spt_context_t* context,
				spt_context_parse_spec_t* pspec_list);

  /**@}*/

#ifdef __cplusplus
}
#endif

#endif	/* defined(support_spt_context_spec_h) */
