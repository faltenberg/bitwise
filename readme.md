General Information
===================

This project is a follow-up on the
[Youtube tutorial](https://www.youtube.com/playlist?list=PLU94OURih-CiP4WxKSMt3UcwMSDM3aTtX)
made by [bitwise](https://www.youtube.com/channel/UCguWV1bZg1QiWbY32vGnOLw).
The goal is to design a processor and a corresponding software stack. The original project is
hosted on [github](https://github.com/pervognsen/bitwise/).


Submoudles
----------

**NOTE**: this repository uses submodules. Thus, prior before doing anything you must execute
the following commands so the dependencies are added properly:

```sh
git submodule init
git submodule update
```

The project is divided into the following subprojects:

- Ion: a compiler for a simple C-like programming language that will be used for the software stack
