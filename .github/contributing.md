# How to contribute to BlocksDS

Thanks for wanting to spend your time helping this project! The instructions
below are just a guide of how to do things in an ideal world. In practice, you
don't need to follow them perfectly for your bug reports and your pull requests
to be valuable.

## Reporting bugs

Bug reports for any of the repositories in the BlocksDS organization must be
created in the [`blocksds/sdk`](https://github.com/blocksds/sdk/issues)
repository.

Give enough information to reproduce the bug. For example, depending on the
problem, you may need to mention:

- Which OS you use.
- Which version of BlocksDS you use (have you upgraded to the most recent
  version?
- Is your bug report related to BlocksDS (the Nintendo DS libraries and tools in
  the BlocksDS repository) or Wonderful Toolchain (the toolchain, which includes
  the compiler, assembler, etc)? If it's related to Wonderful Toolchain, check
  [this link](https://wonderful.asie.pl/wiki/doku.php?id=community).
- Can you create a minimal example that shows the problem?

## Contributing with pull requests

### Bug fixes

If you want to contribute a bug fix, simply fork the affected repository and
open a pull request! It would be great if you could also provide a test or
example that shows the bug in action before the fix, and the bug gone after the
fix.

### New features

If you're going to contribute a new feature, it's a good idea to discuss it
first with the maintainers rather than creating a pull request with a lot of
code with no warning. The [issue tracker](https://github.com/blocksds/sdk/issues)
of `blocksds/sdk` contains a list of known missing features. Other developers
may have already had a discussion about the feature you want to implement.

### General contribution advice

When writing code, try to match the style of the pre-existing code (use spaces,
not tabs, be careful with your curly braces, etc). Small deviations of the
pre-existing style are generally allowed, but you are expected to match the
style as much as possible.

Please, try to create clean commits with good commit messages. Not every commit
needs to be perfect, but having good commit messages is important.

If you're adding a function, Don't create a pull request with 10 commits with
small incremental changes to the function. Instead, squash them into commits
that contain self-contained changes. For example, a PR that fixes a bug in the
texture allocation code and adds a function to get some information from
textures will probably need two commits (one to fix the bug, another one to add
the new function).

Good commit messages start with the part of the code that is being modified and
is followed by a very short description of the change. For example, `build: Fix
install target in makefile` is a good commit message. If you need inspiration,
check the commit messages that are already in the repository.

If the short message isn't good enough to describe the change, add as much text
as you want to the body of the commit message.

You can read more about how to write good commit messages in
[this link](https://www.freecodecamp.org/news/how-to-write-better-git-commit-messages/).

When you're ready to submit your pull request it's a good idea to rebase it on
top of the current `master` branch: `git fetch origin && git rebase origin/master`

Be prepared to rework your branch if you are requested to change things in the
pull request review process. `git rebase --interactive origin/master` is a very
useful tool when you need to squash commits, reorder them, change their
messages, etc.

## Contacting the developers

Whether you want to report a bug report or contribute code to BlocksDS it may be
a good idea to talk to the developers of BlocksDS first (for example, in
[Discord](https://blocksds.skylyrac.net/docs/introduction/support/)).

In general it's better to report bugs in the GitHub issue tracker than in a
chat. Chat discussions get forgotten quickly, GitHub issues remain there until
they are closed.
