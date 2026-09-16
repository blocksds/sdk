---
title: Contributing guide
weight: 6
---

Licensing
---------

Any code contributed to a repository of the BlocksDS project must be licensed
under the license used in that repository. If you aren't the author of the code,
ask the original author for permission to relicense it.

There can be exceptions for 3rd-party libraries that are vendored in a
repository, but they must be kept to a minimum.

AI tools
--------

Generative AI, including large language models (LLMs), should not be used in any
way when contributing to BlocksDS.

We want our code to be art. We want to interact with real humans. Please don't
submit AI-generated comments or code in bug reports or pull requests. We
understand some people consider AI to be a useful tool, but we want to connect
with you, not your computer.

All contributions to the repositories of the BlocksDS project must be created by
a human.

Coding style
------------

Contributions to a repository should use the coding style of that repository.
There isn't a strict set of style guides, so minor deviations may be accepted.

Maintainers may choose to make style changes to a pull request before merging it
under some circumstances. For example, if a pull request needs too many changes
to be merged, which would slow the merging process, or if the author of the pull
request isn't responsive enough.

Commit messages
---------------

Commit descriptions must start with the name of the affected part of the code
followed by a colon. For small changes that aren't related to any specific part
of the code, `chore` may be used.

The commit description must be as short as possible. As a guideline, aim for at
most 70 character description. If it needs to be longer, consider splitting your
commit into multiple simpler commits.

```
examples: Update certificate to access example.com
```

The body of the message can be as long as needed to explain the changes of the
commit. It should be wrapped at 72 columns, but this isn't a strict limit, and
it may be ignored if it makes the text more readable.

When adding URLs to a commit message, use the following pattern:

```
backgrounds: Fix handling of an I/O register

I've read in [1] that this I/O register doesn't work the way the code
expects. The documentation in [2] is incorrect. This patch fixes this
bug.

[1] www.example.com
[2] www.documentation.com
```

Use the tags `Reported-by:` if you're fixing a bug reported by someone else, and
`Co-authored-by:` if a commit has been created with help of someone else.

Pull requests
-------------

When creating a pull request, make sure that your commits have the full
explanation of why the changes are needed. BlocksDS allows the following merge
strategies, ordered by most preferred to least preferred:

1. Update the `master` branch to point to the changes in the PR. This doesn't
   modify any commit or create a merge commit.
2. If the pull request is small and it involves a small number of commits that
   don't break the build, they are manually rebased on top of `master`, which
   modifies the commiter information of the commits (but preserves the author).
3. If a pull request is big and it involves several commits, it will be merged
   with a merge commit so that the original commits aren't modified. The pull
   request is used as a grouping mechanism. The description of the pull request
   has to be used as the merge commit message.
4. If the pull request is messy and it contains commits such as "fix review
   comments" it will be squashed into a single commit and rebased. It's better
   if the pull request author keeps the branch tidy so that the original commits
   can be preserved.

Interacting with the maintainers
--------------------------------

If you want to implement a big feature, communicate it to the maintainers of
BlocksDS before starting so that the development of the feature can be
coordinated and it doesn't cause any other issue.

Check [this page](../../support) for more details about how to contact the
maintainers.
