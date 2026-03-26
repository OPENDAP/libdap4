Prompt: How do I include skills in a repo (as opposed to a directory under my home directory)?

Put them in a **checked-in `.agents/skills/` directory inside the repo**.

A repo-local skill looks like this:

```text
my-repo/
  AGENTS.md
  .agents/
    skills/
      code-change-verification/
        SKILL.md
        scripts/
        references/
        assets/
        agents/
          openai.yaml
```

`SKILL.md` is required; `scripts/`, `references/`, `assets/`, and `agents/openai.yaml` are optional. Codex supports skills in the CLI, IDE extension, and Codex app. ([OpenAI Developers][1])

The key rule is **location**: for repository-scoped skills, Codex scans `.agents/skills` in every directory from your **current working directory up to the repository root**. So you can put skills at the repo root for everyone, or deeper in the tree for module- or service-specific workflows. For example, these are all valid patterns: ([OpenAI Developers][1])

```text
repo/.agents/skills/...
repo/services/payments/.agents/skills/...
repo/apps/web/.agents/skills/...
```

That means:

- use `repo/.agents/skills` for skills that apply to the whole codebase
- use `some/subdir/.agents/skills` for skills that should only be relevant when working in that area of the repo. ([OpenAI Developers][1])

The contrast with home-directory skills is:

- **repo-local/shared:** `.agents/skills` inside the repository
- **user-global/personal:** `$HOME/.agents/skills`
  Use the repo path when you want the skill versioned in Git and shared with teammates. ([OpenAI Developers][1])

A minimal repo skill can be just:

```md
---
name: code-change-verification
description: Run lint, typecheck, and the smallest relevant tests when runtime code, tests, or dependencies change. Do not use for docs-only edits.
---

1. Identify the affected packages and test scope.
2. Run the smallest required validation stack.
3. Report commands run, results, and any blockers.
```

Codex can invoke a skill explicitly with `$skill-name` or `/skills`, and it can also choose it implicitly based on the `description`, so the description should be very specific about when it should and should not trigger. ([OpenAI Developers][1])

Two practical gotchas:

- if two skills have the same `name`, Codex does **not** merge them; both may appear
- if a new or edited skill does not show up right away, restart Codex. ([OpenAI Developers][1])

One more distinction that often trips people up: **custom prompts** in `~/.codex/prompts` are local-only and not shared through the repository; OpenAI now recommends using skills instead when you want something reusable and shareable in a repo. ([OpenAI Developers][2])

A solid default for most teams is:

```text
repo/
  AGENTS.md
  .agents/
    skills/
      implementation-strategy/
      code-change-verification/
      pr-draft-summary/
```

That is also the pattern OpenAI describes in its OSS-maintenance writeup: `AGENTS.md` at repo level plus repo-local skills in `.agents/skills/`. ([OpenAI Developers][3])

If you want, I can draft a concrete `.agents/skills/` layout for a monorepo or a single-service GitHub repo.

[1]: https://developers.openai.com/codex/skills 'Agent Skills – Codex | OpenAI Developers'
[2]: https://developers.openai.com/codex/custom-prompts 'Custom Prompts – Codex | OpenAI Developers'
[3]: https://developers.openai.com/blog/skills-agents-sdk 'Using skills to accelerate OSS maintenance | OpenAI Developers'
