Not in the way the public docs describe it.

For the **VS Code extension / local IDE workflow**, OpenAI documents Codex as getting context from:

- **open files and selected code**
- files you explicitly attach with **`@file`**
- commands to add a **selected range** or an **entire file** to the current thread
- repo instructions like **`AGENTS.md`**
- the agent’s ability, in **Agent mode**, to **read files, edit files, and run commands in the working directory**. ([OpenAI Developers][1])

So the best reading of the docs is: **Codex is using local workspace access plus explicit/editor-supplied context and agentic file/tool use**, not a publicly documented “repo-wide RAG index over your GitHub repository.” OpenAI’s docs do **not** say that the VS Code plugin builds a vector index of your repo or does semantic retrieval over GitHub as a separate RAG layer. That does not mean there are no internal retrieval heuristics at all; it means the public product docs do not describe the repo-context mechanism in RAG terms. ([OpenAI Developers][1])

Also, for the IDE extension, the important unit is your **local project / working directory**, not “GitHub” per se. GitHub becomes central when you use **Codex cloud/web**: there, OpenAI says you connect your GitHub account so Codex can work with the code in your repositories and create pull requests. ([OpenAI Developers][2])

One useful distinction: the docs **do** explicitly mention an indexed/cache-based mechanism for **web search** in the IDE extension. For local tasks, web search is on by default and uses an **OpenAI-maintained index of web results** in cached mode. That is about **web retrieval**, not repository-code retrieval. ([OpenAI Developers][1])

The practical takeaway is:

- treat Codex as strongest when you **open/select the relevant files**
- use **`@file`** mentions
- add a **range or full file** to the thread when needed
- keep **`AGENTS.md`** and **skills** in the repo so it starts with the right project context. ([OpenAI Developers][1])

If you want, I can turn this into a concrete mental model of Codex context as: **manual context + repo instructions + agentic code search**, and compare that to a classical RAG pipeline.

[1]: https://developers.openai.com/codex/ide/features 'Features – Codex IDE | OpenAI Developers'
[2]: https://developers.openai.com/codex/cloud 'Web – Codex | OpenAI Developers'
