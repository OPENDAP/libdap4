Yes — the process you’re describing is a strong way to use Codex.

The clean pattern is:

`AGENTS.md` for durable repo rules, `skills` for repeatable workflows, `agents/openai.yaml` for skill policy and dependencies, MCP for live external context, and Git/CI/review for enforcement. OpenAI’s Codex docs explicitly frame customization in roughly that order: start with `AGENTS.md`, then add skills, then MCP, then subagents. ([OpenAI Developers][1])

## The role of each piece

`AGENTS.md` is your repo’s operating manual for Codex. Codex reads it before doing work, merges global and project-local files, and lets more specific files closer to the working directory override earlier guidance. OpenAI recommends keeping it practical: repo layout, run/build/test/lint commands, conventions, constraints, review expectations, and what “done” means. The CLI `/init` command can scaffold a starter file. ([OpenAI Developers][2])

Skills are the next layer. They package a repeatable workflow in `SKILL.md`, with optional `scripts/`, `references/`, `assets/`, and `agents/openai.yaml`. Codex starts by loading only skill metadata and uses the skill’s `description` as a routing signal, so the description needs to say very clearly when the skill should and should not trigger. OpenAI’s guidance is to keep each skill focused on one job and turn repeated prompts or repeated corrections into skills. ([OpenAI Developers][3])

`agents/openai.yaml` is where a skill declares UI metadata, invocation policy, and tool dependencies. That is the right place to say “this skill can only be run explicitly” or “this skill needs a particular MCP server.” MCP is for live systems and frequently changing context such as docs servers, issue trackers, GitHub, or design tools. ([OpenAI Developers][3])

Subagents are for explicit parallel work, not your default mode. Codex does not spawn them automatically; you ask for them when you want parallel review or decomposition. For example: one subagent for security, one for tests, one for maintainability. ([OpenAI Developers][4])

## The process I’d actually recommend

### 1. Start with a safe repo baseline

Put the repo under Git, make sure lint/typecheck/test commands exist, and treat CI as the final enforcement layer. OpenAI recommends Git checkpoints before and after Codex tasks. Locally, Codex defaults to a sandboxed environment with no network access and workspace-limited writes; avoid bypassing approvals and sandboxing unless you are already inside a hardened environment. ([OpenAI Developers][5])

### 2. Write `AGENTS.md` before you build skills

Do not try to encode everything in prompts. Put the permanent rules in `AGENTS.md`:

- where code lives
- how to run/build/test/lint
- architectural constraints
- compatibility rules
- security/review guidance
- your definition of done
- mandatory skill triggers

A very effective pattern from OpenAI’s own OSS maintenance workflow is to put short operational rules near the top, such as “use `$implementation-strategy` before changing APIs” and “run `$code-change-verification` when runtime code or tests change.” That keeps the repo’s expectations enforceable without repeating them in every thread. ([OpenAI Developers][6])

### 3. Start with 3 skills, not 30

The first skills I would create are:

- `implementation-strategy`: for API/schema/cross-service changes; produces a plan, compatibility notes, and verification steps.
- `code-change-verification`: runs the repo’s required validation stack when code, tests, dependencies, or build/test behavior changes.
- `pr-draft-summary`: prepares the branch name, PR title, and handoff summary when work is ready for review.

That pattern is close to the one OpenAI describes using in the Agents SDK repos, where repo-local skills and `AGENTS.md` are combined to enforce implementation strategy, verification, current-doc lookup, and PR handoff. Use `$skill-creator` to scaffold the first version, keep the scope narrow, and refine only after you see real repeat usage. ([OpenAI Developers][7])

### 4. Use MCP only where repo context is not enough

Add MCP when the needed context lives outside the repo or changes frequently. Good early candidates are:

- current framework/vendor docs
- GitHub issues and PR context
- ticketing systems
- design system or Figma context

If a skill depends on an external system, declare that in `agents/openai.yaml` so the dependency is part of the skill contract instead of tribal knowledge. ([OpenAI Developers][8])

### 5. Run a fixed daily loop

For planning, let Codex do first-pass feasibility and codebase mapping, but keep prioritization, estimates, and tradeoffs human-owned. OpenAI’s “AI-native engineering team” guidance is very clear on that split. A good artifact here is `PLAN.md`: affected components, risks, order of operations, and checks to run. ([OpenAI Developers][9])

For implementation, use the IDE when you want fast exploration around open files, and the CLI when you want a reproducible transcript plus shell commands. The official workflow examples emphasize being explicit about files, repro steps, constraints, and verification. ([OpenAI Developers][10])

For review, use `/review` locally before committing, and use `@codex review` in GitHub PRs when you want Codex to review without pulling the branch locally. In GitHub, Codex follows `Review guidelines` from the closest `AGENTS.md` to each changed file, so you can keep global review rules at the repo root and add stricter ones deeper in the tree. ([OpenAI Developers][10])

### 6. Automate only after the manual workflow is stable

Once a workflow is stable, move it into automations or CI. In the Codex app, automations can run in the local project or in dedicated Git worktrees, and they can invoke skills explicitly with `$skill-name`. For CI/CD, OpenAI provides `openai/codex-action@v1` for GitHub Actions. ([OpenAI Developers][11])

### 7. Add subagents or the Agents SDK only when you need orchestration

Use subagents for bounded parallel work like “security review + test-gap review + maintainability review.” Use the Agents SDK when you want deterministic, reviewable multi-agent workflows with handoffs and traces by exposing Codex CLI as an MCP server. That is the right move for larger release pipelines, migrations, or issue-to-PR systems. ([OpenAI Developers][4])

## A starter `AGENTS.md`

```md
# AGENTS.md

## Repo map

- Web app: apps/web
- API: services/api
- Shared libraries: packages/\*
- Tests: tests/ and packages/\*/test

## Engineering rules

- Preserve public API shapes unless the task explicitly authorizes a breaking change.
- For schema or migration changes, include rollback notes in the final summary.
- For user-visible behavior changes, update docs or release notes.

## Mandatory skill usage

- Use `$implementation-strategy` before changing APIs, schemas, migrations, or cross-service contracts.
- Run `$code-change-verification` when runtime code, tests, dependencies, or build/test behavior changes.
- Use `$pr-draft-summary` when the task is ready for human review.
- Use `$current-docs` when touching vendor APIs or external integrations.

## Commands

- install: pnpm i
- dev: pnpm dev
- lint: pnpm lint
- typecheck: pnpm typecheck
- test: pnpm test

## Definition of done

- Smallest relevant test suite passes
- Lint and typecheck pass
- Changed behavior is documented
- Final response lists files changed, checks run, and remaining risks

## Review guidelines

- Flag auth regressions, data-loss risk, PII leakage, and migration risk as high severity.
```

This follows the structure OpenAI recommends for `AGENTS.md`, but with the operational triggers moved near the top, which is the pattern shown in the OSS-maintenance writeup. ([OpenAI Developers][7])

## A starter skill

```md
---
name: code-change-verification
description: Run the repository verification stack when changes affect runtime code, tests, dependencies, or build/test behavior. Do not use for docs-only edits or comment-only changes.
---

1. Inspect the changed files and classify the change.
2. Decide whether full verification is required.
3. Run the smallest required validation stack in this order:
   - lint
   - typecheck
   - targeted tests
   - broader test suite if the change crosses module boundaries
4. Report:
   - commands run
   - pass/fail status
   - failing output summaries
   - whether the task is ready for review
5. Do not mark the task complete until the required checks pass or the blocker is explained clearly.
```

The important part is the description. In Codex, that description is part of the routing contract. ([OpenAI Developers][3])

## A starter `agents/openai.yaml`

```yaml
policy:
  allow_implicit_invocation: true

dependencies:
  tools:
    - type: 'mcp'
      value: 'github'
      description: 'Pull request and issue context'
    - type: 'mcp'
      value: 'context7'
      description: 'Current framework and library docs'
```

For risky skills such as deploy, release, or production triage, I would set `allow_implicit_invocation: false` so they only run through an explicit `$skill-name` call. ([OpenAI Developers][3])

## Prompts that work well with this setup

Planning:

```text
Read @SPEC.md and the code under @services/payments. Use $implementation-strategy.
Produce PLAN.md with:
- affected components
- compatibility boundaries
- migration risk
- smallest safe implementation order
- checks to run before and after
Do not edit code yet.
```

Implementation:

```text
Implement only step 1 from PLAN.md.
Constraints:
- no API shape changes
- minimal diff
- add a regression test if feasible
After coding, run $code-change-verification.
```

Review:

```text
/review Focus on auth regressions, rollback risk, and missing tests.
```

PR handoff:

```text
Use $pr-draft-summary and produce:
- branch name suggestion
- PR title
- PR body
- concise risk notes
```

## The texts I’d read first

- **Best practices – Codex**: the clearest mental model for treating Codex as a configurable teammate and for the `AGENTS.md -> skills -> MCP -> automations` progression. ([OpenAI Developers][7])
- **Custom instructions with AGENTS.md**: exact precedence rules, file discovery, and how to structure global vs repo vs nested guidance. ([OpenAI Developers][2])
- **Agent Skills – Codex**: skill structure, routing, `$skill-creator`, and `agents/openai.yaml`. ([OpenAI Developers][3])
- **Workflows – Codex**: concrete prompt patterns for explaining code, fixing bugs, writing tests, local review, and docs updates. ([OpenAI Developers][10])
- **Using skills to accelerate OSS maintenance**: the best real-world example of combining repo-local skills, `AGENTS.md`, and GitHub Actions into a repeatable engineering loop. ([OpenAI Developers][6])
- **Use Codex with the Agents SDK**: the reference for moving from repo-local usage to deterministic orchestration and multi-agent pipelines. ([OpenAI Developers][12])
- **Building an AI-Native Engineering Team**: the SDLC-level view of where agents help in planning, design, build, test, review, and ops, and where humans should keep ownership. ([OpenAI Developers][9])

My strongest recommendation is to start small: one good `AGENTS.md`, three narrow skills, one docs MCP, and a review gate. That is enough to make Codex feel like part of your engineering process instead of a chat window. Share your stack and repo shape, and I’ll turn this into a concrete `AGENTS.md` plus three starter skills for it.

[1]: https://developers.openai.com/codex/concepts/customization 'Customization – Codex | OpenAI Developers'
[2]: https://developers.openai.com/codex/guides/agents-md 'Custom instructions with AGENTS.md – Codex | OpenAI Developers'
[3]: https://developers.openai.com/codex/skills 'Agent Skills – Codex | OpenAI Developers'
[4]: https://developers.openai.com/codex/concepts/subagents 'Subagents – Codex | OpenAI Developers'
[5]: https://developers.openai.com/codex/quickstart 'Quickstart – Codex | OpenAI Developers'
[6]: https://developers.openai.com/blog/skills-agents-sdk 'Using skills to accelerate OSS maintenance | OpenAI Developers'
[7]: https://developers.openai.com/codex/learn/best-practices 'Best practices – Codex | OpenAI Developers'
[8]: https://developers.openai.com/codex/mcp 'Model Context Protocol – Codex | OpenAI Developers'
[9]: https://developers.openai.com/codex/guides/build-ai-native-engineering-team 'Building an AI-Native Engineering Team – Codex | OpenAI Developers'
[10]: https://developers.openai.com/codex/workflows 'Workflows – Codex | OpenAI Developers'
[11]: https://developers.openai.com/codex/app/automations 'Automations – Codex app | OpenAI Developers'
[12]: https://developers.openai.com/codex/guides/agents-sdk 'Use Codex with the Agents SDK | OpenAI Developers'
