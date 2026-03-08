**Deep Dive Summary**
- This repo is a mature C++ DAP stack with clear layering: core protocol/data model (`libdap`), HTTP/client transport (`libdapclient`), and server filter helpers (`libdapserver`).
- It implements both DAP2 and DAP4 in one codebase, with shared type infrastructure and protocol-specific parsers/serializers.
- The implementation is production-oriented and heavily tested, but there are a few explicit DAP4 “not yet implemented” paths you should treat as known gaps.

**Library Boundaries**
- `libdap` is the core and includes DAP2 + DAP4 model, parsers, marshalling, CE logic, and common utilities: [CMakeLists.txt](/Users/jimg/src/opendap/hyrax_git/libdap4/CMakeLists.txt#L275), [Makefile.am](/Users/jimg/src/opendap/hyrax_git/libdap4/Makefile.am#L64), [libdap.pc.in](/Users/jimg/src/opendap/hyrax_git/libdap4/libdap.pc.in).
- `libdapclient` adds client-facing connect/config/HTTP layers: [CMakeLists.txt](/Users/jimg/src/opendap/hyrax_git/libdap4/CMakeLists.txt#L299), [Makefile.am](/Users/jimg/src/opendap/hyrax_git/libdap4/Makefile.am#L71), [libdapclient.pc.in](/Users/jimg/src/opendap/hyrax_git/libdap4/libdapclient.pc.in).
- `libdapserver` contains server filter primitives for CGI-style handlers: [CMakeLists.txt](/Users/jimg/src/opendap/hyrax_git/libdap4/CMakeLists.txt#L324), [Makefile.am](/Users/jimg/src/opendap/hyrax_git/libdap4/Makefile.am#L76), [libdapserver.pc.in](/Users/jimg/src/opendap/hyrax_git/libdap4/libdapserver.pc.in).

**DAP2 Implementation Path**
- DAP2 client workflow centers on `Connect`: request DAS/DDS/DDX/DataDDS, parse MIME metadata, parse DDS text, then XDR-unmarshal data: [Connect.h](/Users/jimg/src/opendap/hyrax_git/libdap4/Connect.h#L127), [Connect.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/Connect.cc#L71), [Connect.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/Connect.cc#L250).
- DAP2 CE selection/function flow is parser-driven (`ce_expr.yy/.lex`) into `ConstraintEvaluator` clauses: [ConstraintEvaluator.h](/Users/jimg/src/opendap/hyrax_git/libdap4/ConstraintEvaluator.h#L41), [ConstraintEvaluator.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/ConstraintEvaluator.cc#L333).
- Server-side response shaping is `DODSFilter` + CE + marshalling: [DODSFilter.h](/Users/jimg/src/opendap/hyrax_git/libdap4/DODSFilter.h#L174), [DODSFilter.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/DODSFilter.cc#L89).

**DAP4 Implementation Path**
- DAP4 client workflow is `D4Connect`: build DAP4 query keys (`dap4.ce`, `dap4.checksum`), fetch `.dmr` or `.dap`, parse DMR first chunk, then stream-unmarshal payload: [D4Connect.h](/Users/jimg/src/opendap/hyrax_git/libdap4/D4Connect.h#L54), [D4Connect.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/D4Connect.cc#L359), [D4Connect.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/D4Connect.cc#L374).
- DMR is the DAP4 root object and supports DAP2↔DAP4 transforms (`build_using_dds`, `getDDS`): [DMR.h](/Users/jimg/src/opendap/hyrax_git/libdap4/DMR.h#L48), [DMR.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/DMR.cc#L156), [DMR.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/DMR.cc#L202).
- DMR parsing uses libxml2 SAX with strict/permissive map handling mode: [D4ParserSax2.h](/Users/jimg/src/opendap/hyrax_git/libdap4/D4ParserSax2.h#L75), [D4ParserSax2.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/D4ParserSax2.cc#L310).

**Data Model Core**
- `BaseType` is the type-invariant root used across DAP2 and DAP4; it tracks projection state, attrs, parentage, transform hooks: [BaseType.h](/Users/jimg/src/opendap/hyrax_git/libdap4/BaseType.h#L118).
- Type enum includes classic DAP2 and DAP4 additions (`Int64`, `UInt64`, `Enum`, `Opaque`, `Group`): [Type.h](/Users/jimg/src/opendap/hyrax_git/libdap4/Type.h#L94).

**Transport and I/O**
- HTTP transport is libcurl-based `HTTPConnect`, with DAP header parsing, optional cache integration, cookies/proxy/no_proxy, and C++ stream mode for DAP4: [HTTPConnect.h](/Users/jimg/src/opendap/hyrax_git/libdap4/http_dap/HTTPConnect.h#L52), [HTTPConnect.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/http_dap/HTTPConnect.cc#L557).
- DAP4 payload transfer uses chunked framing and receiver-makes-right byte handling: [chunked_stream.h](/Users/jimg/src/opendap/hyrax_git/libdap4/chunked_stream.h#L11), [chunked_istream.h](/Users/jimg/src/opendap/hyrax_git/libdap4/chunked_istream.h#L42), [chunked_istream.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/chunked_istream.cc#L94).

**Serialization**
- DAP2 serialization path uses XDR stream/file marshallers: [XDRStreamMarshaller.h](/Users/jimg/src/opendap/hyrax_git/libdap4/XDRStreamMarshaller.h#L53).
- DAP4 serialization path uses `D4StreamMarshaller` with optional checksums and stream-first encoding: [D4StreamMarshaller.h](/Users/jimg/src/opendap/hyrax_git/libdap4/D4StreamMarshaller.h#L58), [D4StreamMarshaller.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/D4StreamMarshaller.cc#L118).

**Constraint/Function Engines**
- DAP4 CE parser/evaluator (`d4_ce`) supports array slicing, shared-dim behavior, map pruning, and filter clauses; includes XXS-aware error redaction: [D4ConstraintEvaluator.h](/Users/jimg/src/opendap/hyrax_git/libdap4/d4_ce/D4ConstraintEvaluator.h#L48), [D4ConstraintEvaluator.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/d4_ce/D4ConstraintEvaluator.cc#L42), [D4ConstraintEvaluator.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/d4_ce/D4ConstraintEvaluator.cc#L181).
- Server function registry is a singleton (`ServerFunctionsList`) used by DAP2 and DAP4 function paths: [ServerFunctionsList.h](/Users/jimg/src/opendap/hyrax_git/libdap4/ServerFunctionsList.h#L46), [ServerFunctionsList.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/ServerFunctionsList.cc#L65), [D4FunctionEvaluator.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/d4_function/D4FunctionEvaluator.cc#L104).

**Tooling and Quality**
- Scale: ~207 `.cc`, ~234 `.h`, 7 parser grammars, 7 lexer files.
- Strong testing surface: autotest integration suites and ~60 CppUnit-style unit tests, including DAP4 round-trips and DMR↔DAP2 translation: [tests/Makefile.am](/Users/jimg/src/opendap/hyrax_git/libdap4/tests/Makefile.am#L27), [unit-tests/Makefile.am](/Users/jimg/src/opendap/hyrax_git/libdap4/unit-tests/Makefile.am#L83).
- Pre-commit and CI hooks are in place for formatting/lint sanity: [pre-commit.yml](/Users/jimg/src/opendap/hyrax_git/libdap4/.github/workflows/pre-commit.yml#L1), [.pre-commit-config.yaml](/Users/jimg/src/opendap/hyrax_git/libdap4/.pre-commit-config.yaml#L1).

**Notable Gaps/Risks**
- DAP4 error-body handling is explicitly unfinished in client paths (`FIXME`/throw): [D4Connect.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/D4Connect.cc#L131), [D4Connect.cc](/Users/jimg/src/opendap/hyrax_git/libdap4/D4Connect.cc#L404).
- Some legacy/deprecated config/proxy paths remain and increase maintenance surface: [RCReader.h](/Users/jimg/src/opendap/hyrax_git/libdap4/RCReader.h#L93), [README.dodsrc](/Users/jimg/src/opendap/hyrax_git/libdap4/README.dodsrc).
- Mixed old/new build systems (Autotools + CMake) are both supported; good for portability, but raises drift risk: [Makefile.am](/Users/jimg/src/opendap/hyrax_git/libdap4/Makefile.am#L1), [CMakeLists.txt](/Users/jimg/src/opendap/hyrax_git/libdap4/CMakeLists.txt#L1).

If you want, I can do a second-pass deep dive focused only on one of these: `libdap` internals, `libdapclient` transport/cache behavior, or `libdapserver` handler integration patterns.