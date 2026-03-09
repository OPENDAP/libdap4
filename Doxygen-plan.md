**Documentation Plan Prompt**

You are working in `libdap4`. Document public methods in stages, with review after each stage.
Target baseline is **1,918 undocumented public methods** (from prior scan).
Use existing Doxygen settings from `doxy.conf` exactly as-is.

**Doxygen Constraints To Respect**

1. Use `doxygen doxy.conf` for every stage.
2. Keep these config assumptions intact:
   - `EXTRACT_ALL = YES`
   - `EXTRACT_PRIVATE = NO`
   - `WARN_IF_UNDOCUMENTED = YES`
   - `WARN_LOGFILE = doxygen_warnings.txt`
   - `INPUT = .`, `RECURSIVE = YES`
   - `EXCLUDE_PATTERNS` already excludes `*/tests/*`, `*/unit-tests/*`, `*/gl/*`, `*/unused/*`, generated lexer/parser `.tab`/`lex` files, and legacy dirs.
3. Treat completion as warning reduction in `doxygen_warnings.txt` for the stage’s files.
4. Do not start the next stage until user review/approval.

**Documentation Style Rules**

1. Add Doxygen comments in headers (`.h`, `.hh`) directly above public method declarations. Document in header by default; use implementation comments only for complex algorithm/side-effect details.
2. Prefer this form:
   - `@brief` one-sentence behavior summary.
   - Follow with a paragraph-level detailed description (behavior, constraints, side effects, preconditions/postconditions as needed).
   - `@param` for each parameter.
   - `@return` when non-void.
   - `@throw` when exceptions are part of method contract.
3. Keep wording behavior-focused, not implementation-focused.
4. For overloads, document each overload’s distinct contract.
5. For inherited overrides with identical behavior, use concise comments and rely on inherited docs only where it is semantically correct.

**Per-Stage Workflow (repeat for every stage)**

1. Edit only files in the current stage.
2. Run:
   - `doxygen doxy.conf`
   - `rg -n "warning:.*not documented" doxygen_warnings.txt`
3. Produce stage report:
   - files touched
   - methods documented
   - remaining undocumented warnings for those files
4. Stop and request user review before proceeding.

**Stage Breakdown**

| Stage | Focus                                           | Files (primary)                                                                                                                                                                                                                                                                                                         | Estimated undocumented methods |
| ----- | ----------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | -----------------------------: |
| 0     | Baseline + conventions                          | no code edits; generate baseline warnings                                                                                                                                                                                                                                                                               |                              0 |
| 1     | Scalar/primitive value types                    | `Byte.h`, `Int8.h`, `Int16.h`, `Int32.h`, `Int64.h`, `UInt16.h`, `UInt32.h`, `UInt64.h`, `Float32.h`, `Float64.h`, `Str.h`, `Url.h`                                                                                                                                                                                     |                           ~241 |
| 2     | Core containers and composite types             | `Vector.h`, `Array.h`, `Constructor.h`, `Sequence.h`, `Structure.h`, `Grid.h`                                                                                                                                                                                                                                           |                           ~328 |
| 3     | DAP2 metadata/model surface                     | `BaseType.h`, `BaseTypeFactory.h`, `AttrTable.h`, `DAS.h`, `DDS.h`, `DataDDS.h`, `DMR.h`, `DapObj.h`, `Type.h`, `ObjectType.h`                                                                                                                                                                                          |                           ~279 |
| 4     | DAP4 model and attributes                       | `D4Attributes.h`, `D4Dimensions.h`, `D4Enum.h`, `D4EnumDefs.h`, `D4Maps.h`, `D4Group.h`, `D4Sequence.h`, `D4Opaque.h`, `D4BaseTypeFactory.h`                                                                                                                                                                            |                           ~266 |
| 5     | Serialization, marshalling, streams             | `Marshaller.h`, `MarshallerFuture.h`, `MarshallerThread.h`, `UnMarshaller.h`, `D4StreamMarshaller.h`, `D4StreamUnMarshaller.h`, `XDR*.h`, `XMLWriter.h`, `chunked_istream.h`, `chunked_ostream.h`, `fdiostream.h`                                                                                                       |                           ~210 |
| 6     | Constraint evaluation and server functions      | `Clause.h`, `ConstraintEvaluator.h`, `D4ConstraintEvaluator.h`, `D4FilterClause.h`, `RValue.h`, `D4RValue.h`, `D4FunctionEvaluator.h`, `ServerFunction.h`, `ServerFunctionsList.h`, `DODSFilter.h`, `D4Function.h`                                                                                                      |                           ~198 |
| 7     | Client/connectivity and HTTP cache stack        | `Connect.h`, `RCReader.h`, `D4Connect.h`, `DAPCache3.h`, `http_dap/HTTPConnect.h`, `http_dap/HTTPResponse.h`, `http_dap/HTTPCache.h`, `http_dap/HTTPCacheTable.h`, `http_dap/HTTPCacheResponse.h`, `http_dap/ResponseTooBigErr.h`, `http_dap/save/mp_lock_guard.h`                                                      |                           ~265 |
| 8     | AIS, parsing helpers, errors, runtime utilities | `AIS*.h`, `Error.h`, `InternalErr.h`, `DDXExceptions.h`, `DDXParserSAX2.h`, `D4ParserSax2.h`, `D4ParseError.h`, `D4AsyncUtil.h`, `SignalHandler*.h`, `EventHandler.h`, `AlarmHandler.h`, `TempFile.h`, `Resource.h`, `Response.h`, `StdinResponse.h`, `DapIndent.h`, `DapXmlNamespaces.h`, `Keywords2.h`, `Ancillary.h` |                           ~131 |
| 9     | Geo + parser interface leftovers + final sweep  | `geo/GSEClause.h`, `geo/gse_parser.h`, `ce_parser.h`, `parser.h`, plus any remaining headers with warnings                                                                                                                                                                                                              |                          ~0-50 |

**Review Gates**

1. After each stage, create one commit or patch set.
2. User reviews comments for correctness, clarity, and consistency.
3. Only after approval, proceed to the next stage.

**Completion Criteria**

1. All planned stages completed and reviewed.
2. `doxygen doxy.conf` runs cleanly enough that undocumented-method warnings are reduced to agreed target for in-scope files.
3. Final summary includes:
   - total methods documented
   - remaining undocumented methods (if any) and rationale
   - list of files still intentionally undocumented.
